/**
  ******************************************************************************
  * @file           : config.h
  * @brief          : 本车专属参数：弹道、装甲板尺寸、IMU 轴序
  * @author         : Yan Yuanbin
  * @date           : 2023/05/21
  * @version        : v1.0
  ******************************************************************************
  *
  * 【这个文件是干什么的】
  *   只放"换一台车就要改"的参数。与车无关的数学/物理常量和通用宏在
  *   Components/Algorithm/Inc/common_def.h，本文件开头已经包含了它，
  *   所以包含 config.h 就等于同时拿到两边的内容。
  *
  * 【怎么用】
  *   Application/ 下的 .c/.h 直接 #include "config.h" 即可，
  *   不需要再单独包含 stdint / math 等标准库头。
  *
  *   ！Components/ 层禁止包含本文件。算法库一旦依赖应用层参数，
  *     就没法在两块板之间复用了。需要通用常量请用 common_def.h。
  *
  * 【往哪加新参数】
  *   换车要改的  ->  这里（弹道系数、装甲板尺寸、PID 参数、限位角……）
  *   换车不用改  ->  common_def.h
  *
  * 【注意】
  *   底盘板和云台板各有一份本文件，内容**本来就应该不同**，
  *   不要拿一块板的覆盖另一块。尤其是下面的 IMU 轴序索引：
  *   它跟 C 板在这块车上的实际安装朝向绑定，装反了姿态解算直接是错的。
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "common_def.h"
/* 通用宏与数学/物理常量在 common_def.h，本文件只放与本车相关的参数 */

/* Vision reslove constants -------------------------------------------------*/

/**
 * @brief  Decision Marking mode
 *         0: select the minimum yaw armor
 *         1: select the minimum distance armor
 */
#define Yaw_Distance_Decision  0

/**
 * @brief ballistic coefficient
 * @note  17mm: 0.038
 *        42mm: 0.019
 */
#define Bullet_Coefficient  0.038f

/**
 * @brief the half width of little armor
 */
#define LittleArmor_HalfWidth   0.07f

/**
 * @brief the half width of Large armor
 */
#define LargeArmor_HalfWidth   0.1175f

/* Gimbal control parameters -------------------------------------------------*/
/**
  * @brief 云台控制参数
  * @note  灵敏度四个系数在"遥控/键鼠"和"自瞄丢目标回退"两条分支里各用一次，
  *        改这里两处会同时生效。
  */
/** Pitch 轴机械限位（度） */
#define GIMBAL_PITCH_MAX                 25.f
#define GIMBAL_PITCH_MIN                 -30.f
/** 前馈增益 */
#define GIMBAL_FORWARD_K_PITCH           1.f
#define GIMBAL_FORWARD_K_YAW             0.01f
/** 遥控器摇杆灵敏度：通道值 -> 目标角增量（度） */
#define GIMBAL_RC_YAW_SENSITIVITY        0.0007f
#define GIMBAL_RC_PITCH_SENSITIVITY      0.00025f
/** 图传键鼠灵敏度：鼠标增量 -> 目标角增量（度）
  * @note 鼠标增量由 Detect_Task 以 200Hz 刷新，而云台任务以更高频率消费，
  *       同一个增量会被重复累加，实际灵敏度高于此处标称值。 */
#define GIMBAL_MOUSE_YAW_SENSITIVITY     0.002f
#define GIMBAL_MOUSE_PITCH_SENSITIVITY   0.0032f

/* Shoot control parameters --------------------------------------------------*/
/**
  * @brief 发射机构参数（摩擦轮 M3508 x2）
  */
/** 摩擦轮目标转速的限幅范围；上电初值取下限 */
#define SHOOT_FRICTION_RPM_MIN           6550
#define SHOOT_FRICTION_RPM_MAX           6700
/** 拨盘目标值 */
#define SHOOT_FEED_TARGET                18
/** 射速自适应：期望弹速窗口(m/s) 与 上调/下调步进 */
#define SHOOT_SPEED_ADAPT_MIN            22.5f
#define SHOOT_SPEED_ADAPT_MAX            23.5f
#define SHOOT_SPEED_ADAPT_UP             9.f
#define SHOOT_SPEED_ADAPT_DOWN           23.f
/** 低于此弹速认为是无效采样（未开火/裁判系统未上报） */
#define SHOOT_SPEED_VALID_MIN            8
/** 摩擦轮"仍在转"的判据转速，低于此值直接输出 0 */
#define SHOOT_FRICTION_STOP_RPM          1000


/* IMU reslove constants ---------------------------------------------------*/
/**
 * @brief the flag of bmi088 Calibration
 *        0: DISABLE
 *        1: ENABLE
 */

/**
 * @brief the index of pitch angle update
 */
#define IMU_ANGLE_INDEX_PITCH  2U
/**
 * @brief the index of yaw angle update
 */
#define IMU_ANGLE_INDEX_YAW   0U
/**
 * @brief the index of roll angle update
 */
#define IMU_ANGLE_INDEX_ROLL   1U

/**
 * @brief the index of pitch gyro update
 */
#define IMU_GYRO_INDEX_PITCH  0U
/**
 * @brief the index of yaw gyro update
 */
#define IMU_GYRO_INDEX_YAW   2U
/**
 * @brief the index of roll gyro update
 */
#define IMU_GYRO_INDEX_ROLL   1U

/**
 * @brief the index of pitch accel update
 */
#define IMU_ACCEL_INDEX_PITCH  0U
/**
 * @brief the index of yaw accel update
 */
#define IMU_ACCEL_INDEX_YAW   2U
/**
 * @brief the index of roll accel update
 */
#define IMU_ACCEL_INDEX_ROLL   1U


#endif //ROBOT_CONFIG_H


