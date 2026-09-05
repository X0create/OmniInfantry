# CLAUDE.md —— 云台板 (Gimbal)

本文件为 Claude Code (claude.ai/code) 在本目录工作时提供指引。
仓库整体结构见 [`../README.md`](../README.md)，底盘板见 [`../Chassis/CLAUDE.md`](../Chassis/CLAUDE.md)。

## 一、板卡定位

RoboMaster 步兵机器人**云台板**固件。STM32F407IGHx（大疆 C 型开发板），FreeRTOS（CMSIS-OS v1）。

职责：Yaw/Pitch 达妙电机 MIT 控制、BMI088 姿态解算、与 MiniPC 的视觉通信、摩擦轮控制、遥控器与图传键鼠解析。
本板是**操作手指令的入口**——底盘的移动速度、底盘/云台/发射模式全部由云台板算好后经 CAN2 下发给底盘板。

## 二、构建

- **IDE**：Keil MDK-ARM V5，**编译器 ARM Compiler V6.22（ARMCLANG）**
- **工程入口**：`MDK-ARM/HAL_Template.uvprojx`
- **输出**：`MDK-ARM/HAL_Template/HAL_Template.hex`（已在 `.gitignore` 中忽略）
- **CubeMX**：`HAL_Template.ioc`，外设配置自动生成到 `Core/`
- **DSP 库**：必须用带 FPU 的 `arm_cortexM4lf_math.lib`
- 无命令行构建。用 Keil GUI 或 VSCode 的 Keil Assistant 插件

最近一次 Rebuild 基线：

```
Code=83604  RO-data=1576  RW-data=1392  ZI-data=53888
0 Error(s), 2 Warning(s)
```

那 2 条告警是既有的，位于 `Components/Algorithm/Src/fast_math.c` 的 `return NAN;`。**除此之外出现任何告警都是本次改动引入的。**

## 三、目录与分层

```
Application/Tasks   FreeRTOS 任务：控制循环与通信
Application/API     纯算法接口（四元数 EKF、弹道解算），不得定义全局变量
Components/         可复用模块：Device 驱动 / Algorithm 算法 / Controller 控制器
Bsp/                板级支持包，唯一允许直接调用 STM32 HAL 的层
Core/               CubeMX 生成的初始化代码
```

**规则：只能向下调用，不得跨层反向调用；`Bsp/` 之外不得出现 HAL 函数调用。**

## 四、FreeRTOS 任务

创建于 `Core/Src/freertos.c`，**全部为 `osPriorityRealtime`**，栈 256 字。

| 任务 | 周期 | 职责 |
|------|------|------|
| `INS_Task` | 1 kHz (`osDelayUntil`) | BMI088 读取（SPI）、四元数 EKF → `INS_Info` 欧拉角与角速度 |
| `Gimbal_Task` | 1 kHz | Yaw/Pitch 双环 PID + 前馈，输出 DM 电机 MIT 指令 |
| `Comm_Task` | 1 kHz | 指令汇总、模式切换、CAN 收发；电机使能/失能处理处有 30 ms 延时 |
| `Shoot_Task` | 1 kHz | 摩擦轮转速 PID、热量与射频管理 |
| `Vision_Task` | 1 kHz (`osDelayUntil`) | USB CDC 与 MiniPC 收发、自瞄目标解算 |
| `Detect_Task` | 200 Hz (5 ms) | 电池电压监控、蜂鸣器提示音、图传键鼠更新、系统自检与电机掉线判定 |

任务间通过全局结构体共享状态：`Comm_Info`、`INS_Info`、`Gimbal_Info`、`Shoot_Info`、`Vision_Info`。

> 所有任务同优先级，靠 `osDelay` 让出 CPU。新增任务前先想清楚是否会饿死 `INS_Task`。

## 五、电机

| 电机 | 类型 | 总线 | TX ID | RX ID | 说明 |
|------|------|------|-------|-------|------|
| `Gimbal_Motor[Yaw]` | 达妙 DM，MIT 模式 | CAN2 | `0x02` | `0x09` | 与底盘板共用 CAN2，底盘监听 `0x09` 做跟随 |
| `Gimbal_Motor[Pitch]` | 达妙 DM，MIT 模式 | CAN1 | `0x01` | `0x11` | |
| `Shoot_Motor[Shoot_R]` | M3508 | CAN1 | `0x200` | `0x201` | 摩擦轮右 |
| `Shoot_Motor[Shoot_L]` | M3508 | CAN1 | `0x200` | `0x202` | 摩擦轮左 |

DM 电机参数范围（`Components/Device/Src/motor.c`）：`P_MAX = π`，`V_MAX = 30`，`T_MAX = 10`。上电需要先发使能帧，`Comm_Task.c` 里检测到 `Data.State == 0` 会重发 `Motor_Enable`。

`Shoot_Motor` 枚举里还有 `Shoot_F`，但未初始化——**拨弹电机（M2006，`0x205`）在底盘板上**，云台板只发开火标志。

## 六、CAN 协议

### CAN1 —— 本地电机

TX `0x200`：`Data[0..1]` = ShootR 电流，`Data[2..3]` = ShootL 电流。
RX：`0x11`（Pitch DM 反馈）、`0x201`/`0x202`（摩擦轮反馈）。

### CAN2 —— 板间通信 + Yaw 电机

**TX `0x310`（云台 → 底盘，`Comm_Task.c` 的 `Comm()`）**

| 字节 | 内容 |
|------|------|
| 0–1 | `Move.Vx`（int16，大端） |
| 2–3 | `Move.Vy`（int16，大端） |
| 4 | `Shoot_Mode << 6` \| `Gimbal_Mode << 4` \| `Fire_Flag << 3` \| `IF_Fire_Accept << 2` |
| 5 | `Chassis_Mode` |
| 6–7 | 图传键鼠键值 `KeyBoard_Info.key.v` |

**RX `0x210`（底盘 → 云台）**：`Data[5]` bit0 = `Gimbal_Lost`、bit1 = `Chassis_Lost`；`Data[6]` = 弹速 ×10。
**RX `0x09`**：Yaw DM 电机反馈。

`Vx`/`Vy` 的来源是三路叠加：图传键鼠 `KeyBoard_Info`（WASD 每键 ±660）、VT13 遥控 `VT13_Info.RC.Channel[2..3]`、DT7 遥控 `remote_ctrl.rc.ch[2..3]` 及其键鼠。改控制手感时注意三路是相加而非互斥。

解析入口：`Bsp/Src/bsp_can.c` 的 `CAN1_RxFifo0RxHandler` / `CAN2_RxFifo1RxHandler`。

## 七、串口与 USB

| 端口 | 波特率 | 用途 |
|------|--------|------|
| USART3 | 100000 | DT7 遥控接收机，SBUS 协议，DMA 双缓冲 |
| USART6 | 921600 | 图传链路接收（`image_transmission.c`，VT13 键鼠与自定义控制器数据） |
| USART1 | 115200 | 图传自定义数据回传（30 字节）/ 调试浮点输出 |
| USB CDC | — | MiniPC 视觉通信 |

**MiniPC 发送帧（16 字节，`Vision_Task.c`）**

```
[0]  0xFF          帧头
[1]  Vision_Mode   RED/BULE/SMALL*/BIG*
[2:6]  Pitch       float，取 -INS_Info.Pitch_Angle
[6:10] Roll        float
[10:14] Yaw        float
[14] 0x00
[15] 0x0D          帧尾
```

接收侧 `Vision_Receive_Info()` 解出目标 Yaw/Pitch 及其速度、加速度、距离，存入 `Vision_Info`。

## 八、视觉与弹道参数

集中在 `Application/Tasks/Inc/config.h`：

- `Bullet_Coefficient 0.038f` —— 17 mm 弹丸空气阻力系数（42 mm 用 0.019）
- `LittleArmor_HalfWidth 0.07f` / `LargeArmor_HalfWidth 0.1175f`
- `Yaw_Distance_Decision` —— 0 = 选 Yaw 偏差最小的装甲板，1 = 选距离最近的
- `GravityAccel 9.718f` —— 本地重力加速度
- `IMU_*_INDEX_*` —— BMI088 三轴到 Pitch/Yaw/Roll 的映射，**必须与 C 板实际安装朝向一致**，装反了姿态直接错

## 九、未参与编译的文件

`Application/API/Src/api_quaternion.c`、`api_trajectory.c` 在磁盘上但不在 Keil 工程里，改动不影响固件。要启用需在 Keil 对应分组里手动 Add Existing Files。

（云台板的 `Detect_Task.c`、`Vision_Task.c`、`Gimbal_Task.c`、`Keyboard.c`、`minipc.c`、`image_transmission.c` 都是**正常参与编译**的，这点与底盘板相反。）

## 十、常见坑

- **源文件编码不统一**，改动前务必先确认单个文件的编码。全仓库 `.c/.h` 实测：108 个纯 ASCII、26 个 UTF-8、14 个 GBK、2 个任何单一编码都解不出来（两块板的 `quaternion.c`，说明历史上被反复转码损坏过）。本板的 GBK 文件：`Components/Algorithm/{Inc/fast_math.h,Src/fast_math.c,Inc/lpf.h,Src/lpf.c}`、`Components/Controller/{Inc/SMC.h,Src/SMC.c}`。另有部分注释**已经是坏的**（如 `Gimbal_Task.h` 中 `SweepFrequency_Info_Typedef` 的字段注释，文件本身是合法 UTF-8，但内容是被双重转码后的乱码），属历史遗留，不是你的编辑器问题。**不要批量转码**——要统一成 UTF-8 得逐个文件确认后再转
- CubeMX 重新生成会覆盖 `Core/`，用户代码必须写在 `/* USER CODE BEGIN/END */` 之间
- 卡尔曼滤波初始化需要把堆开到默认 `0x0200` 以上（CubeMX → Linker Settings → Minimum Heap Size）
- 改文件名/目录后，`.uvprojx` 与 `.uvoptx` 都要同步，两边分组数量必须一致
- `Gimbal_Task.h` 末尾有大段被注释掉的 `IK_Solver` 运动学解算代码，不是死代码遗留就是未完成功能，动之前先问清楚
- 编译产物、`*.uvguix.*`、`JLinkSettings.ini` 已被 `.gitignore` 忽略，不要再提交

## 十一、代码风格

参考 [Google C++ Style Guide](https://zh-google-styleguide.readthedocs.io/en/latest/google-cpp-styleguide/contents/)（按 C 语言调整）。现状：类型名/结构体用 PascalCase，函数名 snake_case 与 camelCase 混用；新代码请向 PascalCase 类型 + snake_case 函数靠拢，不要为统一风格去改动已验证的旧代码。
