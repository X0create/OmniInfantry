 /**
  * @file bsp_adc.c
  * @brief ADC voltage measurement encapsulates implementation.
  * @author Gao Xing
  * @date 2025/3/8
  * @version 1.0
  */

 #include "bsp_adc.h"
 #include "main.h"

 /** @brief 外部ADC句柄声明 */
 extern ADC_HandleTypeDef hadc1;
 extern ADC_HandleTypeDef hadc3;
Voltage_Monitor_s Voltage_Monitor;
 /** @brief 电压值参考内部比例常量 */
 volatile float Voltage_Vrefint_Proportion = 8.0586080586080586080586080586081e-4f;

 /** @brief 电池电压分压系数 (22KΩ + 200KΩ) / 22KΩ = 10.09 */
 const float BATTERY_VOLTAGE_DIVIDER = 10.090909090909090909090909090909f;

 //------------------------------------------------------------------------------

 /**
  * @brief 获取指定ADC通道的原始值
  * @param ADCx ADC句柄指针
  * @param ch ADC通道
  * @return 通道原始值
  */
 static uint16_t GetAdcChannelValue(ADC_HandleTypeDef *ADCx, uint32_t ch)
 {
     static ADC_ChannelConfTypeDef sConfig = {0};
     sConfig.Channel = ch;
     sConfig.Rank = 1;
     sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;

     if (HAL_ADC_ConfigChannel(ADCx, &sConfig) != HAL_OK)
     {
         Error_Handler();
     }

     HAL_ADC_Start(ADCx);
     HAL_ADC_PollForConversion(ADCx, 10);

     return (uint16_t)HAL_ADC_GetValue(ADCx);
 }

 /**
  * @brief 初始化内部参考电压校准值
  * @details 通过多次采样内部参考电压，计算转换比例
  */
 void InitVrefintCalibration(void)
 {
     uint8_t i = 0;
     uint32_t total_adc = 0;

     // 多次采样以提高准确性
     for (i = 0; i < 200; i++)
     {
         total_adc += GetAdcChannelValue(&hadc1, ADC_CHANNEL_VREFINT);
     }

     // 计算比例系数 (1.2V为内部参考电压标准值)
     Voltage_Vrefint_Proportion = 200 * 1.2f / total_adc;
 }

 /**
  * @brief 获取电池电压值
  * @return 电池电压值(V)
  * @details 通过ADC3的通道8读取分压后的电池电压，并转换回实际值
  */
 float GetBatteryVoltage(void)
 {
     float voltage;
     uint16_t adc_value = 0;

     // 读取ADC原始值
     adc_value = GetAdcChannelValue(&hadc3, ADC_CHANNEL_8);

     // 转换为实际电压值: 原始值 * 内部参考比例 * 分压比例
     voltage = (float)adc_value * Voltage_Vrefint_Proportion * BATTERY_VOLTAGE_DIVIDER;

     return voltage;
 }

 float GetTemprate(void) 
{ 
    uint16_t adcx = 0; 
    float temperate; 
 
    adcx = GetAdcChannelValue(&hadc1, ADC_CHANNEL_TEMPSENSOR); 
    temperate = (float)adcx * Voltage_Vrefint_Proportion; 
    temperate = (temperate - 0.76f) * 400.0f + 25.0f; 
 
    return temperate; ;
}
 
 
 
 //------------------------------------------------------------------------------
 /*以下为电压监测相关实现*/

 /** @brief 全局电压监测实例 */
 /**
  * @brief 初始化电压监测器
  * @param monitor 电压监测器指针
  * @param low_threshold 低电量阈值(V)
  * @param critical_threshold 危险电量阈值(V)
  */
 void InitVoltageMonitor(Voltage_Monitor_s *monitor, float low_threshold, float critical_threshold)
 {
     monitor->low_threshold = low_threshold;
     monitor->critical_threshold = critical_threshold;
     monitor->last_check_time = 0;
     monitor->status = VOLTAGE_NORMAL;

     // 初始化参考电压校准
     InitVrefintCalibration();
 }

 /**
  * @brief 更新电压监测器状态
  * @param monitor 电压监测器指针
  * @details 定期检查电池电压，并更新状态
  * @note 需要在主循环中定期调用
  */
 void UpdateVoltageMonitor(Voltage_Monitor_s *monitor)
 {
     uint32_t current_time = HAL_GetTick();

     // 每隔1000ms检查一次电压
     if (current_time - monitor->last_check_time >= VOLTAGE_CHECK_INTERVAL)
     {
         monitor->last_check_time = current_time;
			   monitor->Last_status = monitor->status;
         monitor->current_voltage = GetBatteryVoltage();
			   monitor->temperate=GetTemprate();
         
         // 更新电压状态
         if (monitor->current_voltage > monitor->critical_threshold)
         {
             monitor->status = VOLTAGE_CRITICAL;
         }
         else if (monitor->current_voltage < monitor->low_threshold)
         {
             monitor->status = VOLTAGE_LOW;
         }
         else
         {
             monitor->status = VOLTAGE_NORMAL;
         }
     }
 }

 /**
  * @brief 获取当前电压状态
  * @param monitor 电压监测器指针
  * @return 电压状态枚举值
  */
 Voltage_Status_e GetVoltageStatus(Voltage_Monitor_s *monitor)
 {
	 
     return monitor->status;
 }

 /**
  * @brief 获取当前电压值
  * @param monitor 电压监测器指针
  * @return 当前电压值(V)
  */
 float GetCurrentVoltage(Voltage_Monitor_s *monitor)
 {
     return monitor->current_voltage;
 }
