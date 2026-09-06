/**
  ******************************************************************************
  * @file           : can_protocol.h
  * @brief          : 板间通信 CAN 报文 ID 与字节布局
  ******************************************************************************
  *
  * 【这个文件是干什么的】
  *   底盘板与云台板通过 CAN2 交换指令和状态。本文件集中定义这条链路上
  *   的报文 ID，并用注释记录每个报文的字节布局。
  *
  * 【怎么用】
  *   需要收发板间报文的文件（Comm_Task.c、bsp_can.c）直接
  *   #include "can_protocol.h"。
  *
  *   电机自身的 CAN ID（M3508/M2006 的 0x201~0x205、达妙的 0x01/0x02/
  *   0x09/0x11）不在这里，它们属于设备地址，定义在 motor.c 的电机实例
  *   里；DJI 电调的电流帧 ID 用 motor.h 的 DJI_TxFrame_Low / _High 枚举。
  *
  * 【！！两块板必须一致 ！！】
  *   底盘板和云台板各有一份本文件，内容**必须逐字节相同**。
  *   改 ID 或改字节布局时两边同时改，只改一边会让板间通信静默错乱——
  *   CAN 不会报错，只会把错的数字当成对的用。
  *
  ******************************************************************************
  */

#ifndef CAN_PROTOCOL_H
#define CAN_PROTOCOL_H

/* Exported constants --------------------------------------------------------*/

/**
  * @brief 云台板 -> 底盘板：操作手指令
  *
  *   Byte 0~1  Move.Vx      int16，大端
  *   Byte 2~3  Move.Vy      int16，大端
  *   Byte 4    bit7~6 Shoot_Mode | bit5~4 Gimbal_Mode
  *             bit3   Fire_Flag  | bit2   IF_Fire_Accept
  *   Byte 5    Chassis_Mode
  *   Byte 6~7  图传键鼠键值 KeyBoard_Info.key.v
  *
  * @note 底盘侧解包时把 Byte0~1 取负后存进 Move.Vy、Byte2~3 存进 Move.Vx，
  *       即两块板的 Vx/Vy 定义是互换且反号的。这是既有约定，不是笔误，
  *       改动任一侧前先确认另一侧。
  */
#define CAN_ID_GIMBAL_TO_CHASSIS   0x310

/**
  * @brief 底盘板 -> 云台板：裁判系统状态
  *
  *   Byte 0~1  chassis_power_limit  大端
  *   Byte 2    buffer_energy
  *   Byte 3    （语义不明，与 Byte4 由一条链式赋值同时写入）
  *   Byte 4    SuperCap.EnableCap
  *   Byte 5    bit1 Chassis_Lost | bit0 Gimbal_Lost
  *   Byte 6    弹速 x10
  *   Byte 7    （未赋值，DLC 固定为 8，发的是上一帧残留）
  *
  * @note Byte6 目前是 (uint8_t)(initial_speed * 10.f)，17mm 弹速 30m/s
  *       算出 300 会溢出成 44。修这个要两块板同时改。
  */
#define CAN_ID_CHASSIS_TO_GIMBAL   0x210

/**
  * @brief 超级电容 -> 底盘板
  *
  *   Byte 4    剩余能量 Surplus_Energy
  *   Byte 6~7  底盘功率 x100，大端
  */
#define CAN_ID_SUPERCAP            0x211

#endif /* CAN_PROTOCOL_H */
