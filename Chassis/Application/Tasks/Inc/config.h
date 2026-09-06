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

/* Chassis control parameters ------------------------------------------------*/
/**
  * @brief 底盘控制参数
  * @note  这些是调车时最常动的值。改之前先想清楚影响的是哪一环：
  *        跟随环、小陀螺、还是功率环。
  */
/** 遥控杆量/键盘量 -> 轮子目标转速的缩放系数 */
#define CHASSIS_RC_TO_RPM_SCALE        7
/** 云台跟随的中值角度（度） */
#define CHASSIS_FOLLOW_MID_ANGLE       90.f
/** 全向底盘的安装角偏置（度），跟随误差算完要减回去 */
#define CHASSIS_MOUNT_ANGLE_OFFSET     45
/** 小陀螺转速：无超电 / 有超电 */
#define CHASSIS_SPIN_RPM               6000
#define CHASSIS_SPIN_RPM_SUPERCAP      8000
/** 超电开启时的底盘功率上限（W） */
#define CHASSIS_POWER_MAX_SUPERCAP     150
/** 缓冲能量 PID 的目标值（J） */
#define CHASSIS_POWER_BUFFER_TARGET    60.f
/** RLS 功率辨识：遗忘因子与协方差初值 */
#define CHASSIS_RLS_LAMBDA             0.99999
#define CHASSIS_RLS_P_INIT             1e-5
/** 四轮静摩擦补偿量，轮序 左前/左后/右后/右前
  * @note 当前 Chassis_Info.Output 没有任何读取点，这组值实际未生效。 */
#define CHASSIS_FRICTION_COMP_LF       -1500
#define CHASSIS_FRICTION_COMP_LB       1000
#define CHASSIS_FRICTION_COMP_RB       1400
#define CHASSIS_FRICTION_COMP_RF       -1100

/* Shoot control parameters --------------------------------------------------*/
/**
  * @brief 发射机构参数（拨弹电机 M2006，热量管理）
  */
/** 拨弹射频（发/秒）：默认 / 热量充裕时 / 卡弹反转时 */
#define SHOOT_FEED_RPS_DEFAULT         10
#define SHOOT_FEED_RPS_MAX             20
#define SHOOT_FEED_RPS_REVERSE         -10
/** 射频(发/秒) -> M2006 目标转速(rpm) 的换算系数 */
#define SHOOT_FEED_RPS_TO_RPM          540.f
/** 热量余量档位：高于 FREE 全速打，低于 WARN 进保守射频 */
#define SHOOT_HEAT_SURPLUS_FREE        80
#define SHOOT_HEAT_SURPLUS_WARN        50
/** ShootTime 的限幅范围 */
#define SHOOT_TIME_MIN                 100
#define SHOOT_TIME_MAX                 5600
/** 卡弹判定：拨弹电流低于此值并持续 COUNT 次即判卡弹 */
#define SHOOT_STUCK_CURRENT            -9600
#define SHOOT_STUCK_COUNT              100
/** 反转退出条件：电流回升到此值，或反转超过 TIMEOUT 次 */
#define SHOOT_REVERSE_EXIT_CURRENT     4000
#define SHOOT_REVERSE_TIMEOUT          100


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


