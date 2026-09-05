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
#include "motor.h"

static void Motor_TempCheck();
static void System_SelfCheck();


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
    UpdateMusicPlayer(&music_player);
    UpdateVoltageMonitor(&Voltage_Monitor);
    Keyboard_Info_Update(&KeyBoard_Info);  
    System_SelfCheck();
    osDelay(5);
   }
    /* USER CODE END Detect_Task */
  }

	
Motor_Status_e GetMortorState(DJI_Motor_Info_Typedef *DJI_Motor)
{
		  if (DJI_Motor->Data.current==DJI_Motor->Data.Lastcurrent)
  {
    DJI_Motor->ERRORHandler.CAN_ErrorCount++;

    if (DJI_Motor->ERRORHandler.CAN_ErrorCount > 500)
    {
      DJI_Motor->ERRORHandler.Status = MOTOR_CAN_OFFLINE;
      DJI_Motor->ERRORHandler.CAN_ErrorCount = 0;
    }
  }
  else
  {
    DJI_Motor->ERRORHandler.CAN_ErrorCount = 0;
  }
    DJI_Motor->Data.Lastcurrent=DJI_Motor->Data.current;
	
	

	if(DJI_Motor->ERRORHandler.Last_status!=DJI_Motor->ERRORHandler.Status)
		{
				DJI_Motor->ERRORHandler.Last_status=DJI_Motor->ERRORHandler.Status;
return DJI_Motor->ERRORHandler.Status;
	}
	else
	{
		DJI_Motor->ERRORHandler.Status=MOTOR_ERROR_NONE;
return MOTOR_ERROR_NONE;
	}
}


	
	
void System_SelfCheck()
{
    switch (GetVoltageStatus(&Voltage_Monitor))
    {
    case VOLTAGE_NORMAL:
      break;
    case VOLTAGE_LOW:
      LowVoltage_Music();
    case VOLTAGE_CRITICAL:
      LowVoltage_Music();
      break;
	  }
	for(int i=0;i<2;i++)
switch(GetMortorState(&Shoot_Motor[i]))
{
	case MOTOR_ERROR_NONE:
		break;
	case MOTOR_CAN_OFFLINE:
CAN_Lost_Music();
		break;
	case MOTOR_OVER_TEMPERATURE:
HighHeat_Music();
	break;
}
		



		
			
			
			



}

	
	