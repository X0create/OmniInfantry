/**
  ******************************************************************************
  * @file           : common_def.h
  * @brief          : 与具体机器人无关的通用定义：限幅宏、数学与物理常量
  ******************************************************************************
  *
  * 【这个文件是干什么的】
  *   Components/ 层（算法、控制器、设备驱动）需要的、与"这台车是什么车"
  *   无关的东西，全部放在这里。它属于 Components 层，**不得** include
  *   Application/ 层的任何头文件，否则会重新形成反向依赖。
  *
  * 【怎么用】
  *   - Components/ 下的文件：直接 #include "common_def.h"
  *   - Application/ 下的文件：不用管，config.h 已经帮你包含了
  *   - Bsp/ 下的文件：一般通过 Components 的头文件间接拿到
  *   本文件已经带上 stdint / stdbool / stdlib / string / math，
  *   包含它之后不必再重复包含这几个标准库头。
  *
  * 【往哪加新常量】
  *   问一句：换一台车（步兵换英雄、换哨兵）这个值要不要改？
  *     不用改  ->  放这里（数学常数、单位换算、通用限幅宏）
  *     要改    ->  放 Application/Tasks/Inc/config.h
  *   例："重力加速度"放这里，"弹丸空气阻力系数"放 config.h。
  *
  * 【注意】
  *   底盘板和云台板各有一份本文件，内容应当保持完全一致。
  *   改动其中一份后记得同步另一份，否则两板行为会悄悄分叉。
  *
  ******************************************************************************
  */

#ifndef COMMON_DEF_H
#define COMMON_DEF_H

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"

/* Exported macro ------------------------------------------------------------*/
/**
  * @brief 将 x 限制在 [min, max] 区间内
  */
 #define VAL_LIMIT(x,min,max)  do{ \
                                    if ((x) > (max)) {(x) = (max);} \
                                    else if ((x) < (min)) {(x) = (min);} \
                                }while(0U)

/**
  * @brief 符号函数：正数返回 1，负数返回 -1，零返回 0
  */
#define SIGN(x)  (((x) > 0) - ((x) < 0))   // 返回 1（正）、-1（负）、0（零）

/**
  * @brief 本地重力加速度
  */
#define GravityAccel  9.718f  

/**
  * @brief 自然常数 e
  */
#define Euler_Number 2.718281828459045f

/**
  * @brief 弧度制转角度制，180/PI
  */
#define RadiansToDegrees 57.295779513f

/**
  * @brief 角度制转弧度制，PI/180
  */
#define DegreesToRadians 0.01745329251f

#endif /* COMMON_DEF_H */
