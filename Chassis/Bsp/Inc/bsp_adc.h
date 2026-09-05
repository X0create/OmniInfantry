/**
 * @file bsp_adc.h
 * @brief ADC voltage measurement encapsulation.
 * @author Gao Xing
 * @date 2025/3/8
 * @version 1.0
 */

#ifndef BSP_ADC_H
#define BSP_ADC_H

#include "main.h"

/** @brief 电压检测间隔时间(ms) */
#define VOLTAGE_CHECK_INTERVAL 1000

/** @brief 电压状态枚举 */
typedef enum
{
    VOLTAGE_NORMAL,  ///< 正常电压
    VOLTAGE_LOW,     ///< 低电压
    VOLTAGE_CRITICAL ///< 危险电压
} Voltage_Status_e;

/** @brief 电压监测器结构体 */
typedef struct
{
    float current_voltage;       ///< 当前电压值
    float low_threshold;         ///< 低电压阈值
    float critical_threshold;    ///< 危险电压阈值	  
	  float temperate;             ///< 板载温度
	  uint32_t last_check_time;    ///< 上次检查时间
    Voltage_Status_e status;     ///< 当前电压状态
	  Voltage_Status_e Last_status;///< 上次电压状态

} Voltage_Monitor_s;

/** @brief 全局电压监测实例 */
extern Voltage_Monitor_s Voltage_Monitor;

/**
 * @brief 初始化内部参考电压校准值
 */
void InitVrefintCalibration(void);

/**
 * @brief 获取电池电压值
 * @return 电池电压值(V)
 */
float GetBatteryVoltage(void);

/**
 * @brief 初始化电压监测器
 * @param monitor 电压监测器指针
 * @param low_threshold 低电量阈值(V)
 * @param critical_threshold 危险电量阈值(V)
 */
void InitVoltageMonitor(Voltage_Monitor_s *monitor, float low_threshold, float critical_threshold);

/**
 * @brief 更新电压监测器状态
 * @param monitor 电压监测器指针
 */
void UpdateVoltageMonitor(Voltage_Monitor_s *monitor);

/**
 * @brief 获取当前电压状态
 * @param monitor 电压监测器指针
 * @return 电压状态枚举值
 */
Voltage_Status_e GetVoltageStatus(Voltage_Monitor_s *monitor);

/**
 * @brief 获取当前电压值
 * @param monitor 电压监测器指针
 * @return 当前电压值(V)
 */
float GetCurrentVoltage(Voltage_Monitor_s *monitor);

#endif /* BSP_ADC_H */
