# CLAUDE.md —— 底盘板 (Chassis)

本文件为 Claude Code (claude.ai/code) 在本目录工作时提供指引。
仓库整体结构见 [`../README.md`](../README.md)，云台板见 [`../Gimbal/CLAUDE.md`](../Gimbal/CLAUDE.md)。

## 一、板卡定位

RoboMaster 步兵机器人**底盘板**固件。STM32F407IGHx（大疆 C 型开发板），FreeRTOS（CMSIS-OS v1），四轮麦克纳姆底盘。

职责：麦轮逆运动学与四轮 PID、功率限制（RLS 在线辨识）、拨弹电机、裁判系统串口与 UI 图层绘制。
姿态、视觉、云台电机控制均在云台板，两板通过 **CAN2** 交换指令与状态。

## 二、构建

- **IDE**：Keil MDK-ARM V5，**编译器 ARM Compiler V6.22（ARMCLANG）**——不是 AC5，`-Wnan-infinity-disabled` 之类的告警来自 clang 前端
- **工程入口**：`MDK-ARM/HAL_Template.uvprojx`
- **输出**：`MDK-ARM/HAL_Template/HAL_Template.hex`（已在 `.gitignore` 中忽略）
- **CubeMX**：`HAL_Template.ioc`，外设配置自动生成到 `Core/`
- **DSP 库**：必须用带 FPU 的 `arm_cortexM4lf_math.lib`，不是 `arm_cortexM4l_math.lib`
- 无命令行构建。用 Keil GUI 或 VSCode 的 Keil Assistant 插件

最近一次 Rebuild 基线（可用于判断改动是否引入新问题）：

```
Code=98448  RO-data=4620  RW-data=1336  ZI-data=57160
0 Error(s), 2 Warning(s)
```

那 2 条告警是既有的，位于 `Components/Algorithm/Src/fast_math.c` 的 `return NAN;`（当前浮点选项下 NaN 属未定义行为）。**除此之外出现任何告警都是本次改动引入的。**

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

创建于 `Core/Src/freertos.c`，栈均为 256 字。

| 任务 | 优先级 | 周期 | 职责 |
|------|--------|------|------|
| `INS_Task` | Realtime | 1 kHz (`osDelayUntil`) | BMI088 读取（SPI）、四元数 EKF 融合 → 欧拉角 |
| `Comm_Task` | Realtime | 1 kHz | CAN 收发、模式管理、UI 图层刷新 |
| `Chassis_Task` | AboveNormal（静态线程） | 1 kHz | 麦轮逆解、四轮 PID、功率限制 |
| `Shoot_Task` | AboveNormal（静态线程） | 1 kHz | 拨弹电机 PID、热量管理状态机 |

任务间通过全局结构体共享状态：`Comm_Info`、`INS_Info`、`Chassis_Info`、`Shoot_Info`。

## 五、CAN 协议

### CAN1 —— 底盘本地电机

| 方向 | ID | 内容 |
|------|-----|------|
| TX | `0x200` | 四个麦轮 M3508 电流（`Chassis_Motor[0..3]`） |
| TX | `0x1FF` | 拨弹 M2006 电流（`Chassis_Motor[4]`） |
| RX | `0x201`–`0x204` | 麦轮反馈（编码器、转速、电流、温度） |
| RX | `0x205` | 拨弹电机反馈 |

### CAN2 —— 板间通信 + Yaw 电机总线

| 方向 | ID | 内容 |
|------|-----|------|
| TX | `0x210` | 发往云台板：`Chassis_Lost` / `Gimbal_Lost` 标志、弹速 |
| RX | `0x310` | 来自云台板：`Move.Vx`/`Vy`、`Shoot_Mode`、`Gimbal_Mode`、`Fire_Flag`、`IF_Fire_Accept`、`Chassis_Mode`、键盘键值 `remote_ctrl.key.v` |
| RX | `0x211` | 超级电容：剩余能量 `Surplus_Energy`、底盘功率 `Chassis_Power`（÷100） |
| RX | `0x09` | Yaw 达妙电机反馈，底盘**只监听**，用于云台跟随 |

> ⚠️ `Components/Device/Src/motor.c` 里 `Gimbal_Motor` 定义了 `TxStdId = 0x02`，但底盘板**不会发送**该帧——发 Yaw 指令的是云台板，且底盘的 `Gimbal_Task.c` 未参与编译。改动 CAN2 时不要误以为底盘在控 Yaw。

解析入口：`Bsp/Src/bsp_can.c` 的 `CAN1_RxFifo0RxHandler` / `CAN2_RxFifo1RxHandler`。

## 六、串口外设

| 端口 | 波特率 | 用途 |
|------|--------|------|
| USART6 | 115200 | **裁判系统**：DMA 双缓冲接收；UI 绘制帧也从这里发出（`ui_interface.h` 的 `SEND_MESSAGE` → `HAL_UART_Transmit_IT(&huart6, ...)`） |
| USART3 | 100000 | 遥控接收机，SBUS 协议，DMA 双缓冲 |
| USART1 | 115200 | 调试浮点输出（`bsp_uart.c` 里的 VOFA 风格发送） |

底盘板没有接 MiniPC，USB CDC 未用于视觉（视觉在云台板）。

## 七、麦轮解算（`Chassis_Task.c`）

轮序：`Wheelrpm[0..3]` = 左前 / 左后 / 右后 / 右前。

```
Wheelrpm[0] = -Vx - Vy - Vw
Wheelrpm[1] =  Vx - Vy - Vw
Wheelrpm[2] =  Vx + Vy - Vw
Wheelrpm[3] = -Vx + Vy - Vw
```

进入逆解前，机体速度先按云台 Yaw 偏角做旋转变换（`Transform.SIN/COS`）；`Vw` 在跟随模式下由跟随 PID 给出，小陀螺模式下为固定值（6000 / 8000）。

## 八、功率控制（`PowerCtrl.c`）

RLS 在线辨识电机功率模型系数 `K1`/`K2`，再对每个电机解二次方程 `A·I² + B·I + C = 0`，把电流压到裁判系统功率预算内。实测功率来自 CAN2 `0x211` 的超级电容反馈。

> 当前 `RLS_Update()` 被注释掉了（`PowerCtrl.c` 第 89 行附近），系数用的是初值。动功率环之前先确认这是有意为之还是遗留。

## 九、裁判系统 UI

- 位置：`Components/Device/{Inc,Src}/ui*`
- 由 **RM UI Designer 生成，Dynamic Edition**。结构体字段带生成器自带的拼写错误：`operate_tpyel`、`figure_tpye`——不要手改，会和下次生成的代码对不上
- 参与编译的只有 `ui_g.c`、`ui_interface.c`（在 Keil 的 `Components/Device` 分组里），`ui.h` 只是把 `ui_interface.h` 和 `ui_g.h` 串起来
- 图层的初始化/更新调用在 `Comm_Task.c`：`ui_init_g_Super_chassis_buffer()`、`ui_init_g_Text_Status()`、`ui_init_g_Xhair()` 等
- **重新生成 UI 时直接覆盖这个目录**。历史上这套代码同时存在于 `Ui/`、`Components/Device/` 和仓库根 `UI/` 三份且互相不一致，编译时实际混用了两份（源文件取一份、头文件取另一份），已在重构中清理，不要再建新目录

## 十、未参与编译的文件

以下文件在磁盘上，但**不在 Keil 工程里**，改它们不会影响固件：

`Application/API/Src/api_quaternion.c`、`api_trajectory.c`、
`Application/Tasks/Src/Gimbal_Task.c`、`Vision_Task.c`、`Detect_Task.c`、
`Components/Device/Src/Keyboard.c`、`minipc.c`、`image_transmission.c`

这些能力大多由云台板承担。要启用其中某个，必须在 Keil 的对应分组里手动 Add Existing Files。

## 十一、常见坑

- **源文件编码不统一**，改动前务必先确认单个文件的编码。全仓库 `.c/.h` 实测：108 个纯 ASCII、26 个 UTF-8、14 个 GBK、2 个任何单一编码都解不出来（两块板的 `quaternion.c`，说明历史上被反复转码损坏过）。本板的 GBK 文件：`Application/Tasks/Inc/config.h`、`PowerCtrl.h`、`Src/PowerCtrl.c`、`Src/Shoot_Task.c`、`Components/Algorithm/{Inc/lpf.h,Src/lpf.c}`、`Components/Controller/{Inc/SMC.h,Src/SMC.c}`。**不要批量转码**——要统一成 UTF-8 得逐个文件确认后再转，并同步把 Keil 编辑器编码设为 UTF-8
- CubeMX 重新生成会覆盖 `Core/`，用户代码必须写在 `/* USER CODE BEGIN/END */` 之间
- 卡尔曼滤波初始化需要把堆开到默认 `0x0200` 以上（CubeMX → Linker Settings → Minimum Heap Size，最终落在 `startup_stm32f407xx.s`）
- 改文件名/目录后，`MDK-ARM/HAL_Template.uvprojx` 与 `.uvoptx` 都要同步，两边的分组数量必须一致，否则 Keil 打开时会重排文件树
- 编译产物、`*.uvguix.*`、`JLinkSettings.ini` 已被 `.gitignore` 忽略，不要再提交

## 十二、代码风格

参考 [Google C++ Style Guide](https://zh-google-styleguide.readthedocs.io/en/latest/google-cpp-styleguide/contents/)（按 C 语言调整）。现状：类型名/结构体用 PascalCase，函数名 snake_case 与 camelCase 混用；新代码请向 PascalCase 类型 + snake_case 函数靠拢，不要为统一风格去改动已验证的旧代码。
