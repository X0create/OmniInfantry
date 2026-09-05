/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : Detect_Task.c
 * @brief          : Detect task
 * @author         : Yan Yuanbin
 * @date           : 2023/04/27
 * @version        : v1.0
 ******************************************************************************
 * @attention      : None
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"
#include "Detect_Task.h"
#include "remote_control.h"
#include "bsp_buzzer.h"
#include "bsp_adc.h"
#include "Keyboard.h"

/* USER CODE BEGIN Header_Detect_Task */
/**
 * @brief Function implementing the StartDetectTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Detect_Task */
void Detect_Task(void const *argument)
{
  /* USER CODE BEGIN Detect_Task */
    TickType_t systick = 0;
  InitVoltageMonitor(&Voltage_Monitor, 20.0f, 26.0f);
  /* Infinite loop */
  for (;;)
  {
    systick = osKernelSysTick();
    updateMusicPlayer(&music_player);
    UpdateVoltageMonitor(&Voltage_Monitor);

    Keyboard_Info_Update(&KeyBoard_Info);  /* Infinite loop */
if(Voltage_Monitor.status!=Voltage_Monitor.Last_status)
    switch (GetVoltageStatus(&Voltage_Monitor))
    {
    case VOLTAGE_NORMAL:
      break;
    case VOLTAGE_LOW:
      ItsMyGo();
      break;
    case VOLTAGE_CRITICAL:
      ItsMyGo();
      break;
	    }
      osDelay(5);
   }
    /* USER CODE END Detect_Task */
  }
