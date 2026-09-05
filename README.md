# OmniInfantry —— RoboMaster 全向轮步兵机器人固件

DJI C 型开发板（STM32F407IGHx）双板方案，FreeRTOS + STM32 HAL。
底盘板与云台板通过 CAN 总线交换指令与状态。

| 板卡 | 目录 | 职责 |
|------|------|------|
| 底盘板 Chassis | [`Chassis/`](Chassis/) | 麦轮逆运动学、4×M3508 PID、功率限制（RLS）、拨弹与摩擦轮、裁判系统 UI |
| 云台板 Gimbal | [`Gimbal/`](Gimbal/) | DM 电机 MIT 控制、IMU 姿态解算、视觉解算与弹道补偿 |

## 目录结构

```
OmniInfantry/
├── Chassis/                  底盘板工程（Keil MDK-ARM）
│   ├── Application/
│   │   ├── API/              纯算法接口（四元数 EKF、弹道解算），不含全局状态
│   │   └── Tasks/            FreeRTOS 任务 + config.h
│   ├── Bsp/                  板级支持包，唯一允许直接调用 STM32 HAL 的层
│   ├── Components/
│   │   ├── Algorithm/        crc / kalman / lpf / ramp / quaternion / rls / fast_math
│   │   ├── Controller/       pid / SMC
│   │   └── Device/           bmi088 / motor / remote_control / referee_info / ui*
│   ├── Core/                 CubeMX 生成的初始化代码
│   ├── Drivers/  Middlewares/  USB_DEVICE/
│   ├── MDK-ARM/              HAL_Template.uvprojx（Keil 工程入口）
│   ├── HAL_Template.ioc      CubeMX 工程文件
│   └── CLAUDE.md             底盘板架构说明
├── Gimbal/                   云台板工程，层次结构同上
├── Docs/                     共用文档（四元数推导、模板工程 README、Mode_Com 注释版）
├── keilkill.bat              清理 Keil 编译中间文件（在本目录运行）
└── .gitignore
```

## 分层规则

```
Application/Tasks   FreeRTOS 任务：控制循环与通信
Application/API     纯算法，无全局变量
Components/         可复用模块：设备驱动、算法库、控制器
Bsp/                板级支持包，唯一可直接调用 HAL
Core/               CubeMX 自动生成
```

**只能向下调用，不允许跨层反向调用；`Bsp/` 之外不得出现 HAL 函数调用。**

## 编译

- IDE：Keil MDK-ARM V5（V5.38a），编译器 ARM Compiler V5.06u7
- 工程入口：`Chassis/MDK-ARM/HAL_Template.uvprojx` / `Gimbal/MDK-ARM/HAL_Template.uvprojx`
- 输出：`MDK-ARM/HAL_Template/HAL_Template.hex`（已在 .gitignore 中忽略）
- DSP 库须使用带 FPU 的 `arm_cortexM4lf_math.lib`
- 无命令行构建，请用 Keil GUI 或 VSCode 的 Keil Assistant 插件

## 裁判系统 UI

UI 代码由 RM UI Designer 生成（Dynamic Edition），位于
`Chassis/Components/Device/{Inc,Src}/ui*`，仅底盘板参与编译。
重新生成后请直接覆盖该目录下的文件，不要另建目录，以免出现多份不一致的副本。

## 调试器说明

仓库不包含 `JLinkARM.dll`。若使用 J-Link 且需要替换 Keil 自带的旧版驱动，请自行到
[SEGGER 官网](https://www.segger.com/downloads/jlink/) 下载对应版本，复制到
`Keil\ARM\Segger\` 目录下。该文件为 SEGGER 专有软件，不随本仓库分发。
