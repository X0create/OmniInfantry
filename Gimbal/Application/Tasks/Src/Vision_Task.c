/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : Vision_Task.c
  * @brief          : Vision task
  * @author         : Yan Yuanbin
  * @date           : 2023/07/23
  * @version        : v2.1
  ******************************************************************************
  * @attention      : None
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"
#include "Vision_Task.h"
#include "INS_Task.h"
#include "api_trajectory.h"
#include "usbd_cdc_if.h"
#include "Gimbal_Task.h"
#include "config.h"

/* Private variables -----------------------------------------------------------*/

static void Vision_Send_Info(Vision_Info_Typedef *Vision_Info);

/* 最近一次收到 MiniPC 数据的时刻，用于超时老化 */
static volatile uint32_t Vision_LastRx_Tick = 0;

/**
 * @brief structure that contains the information for the Vision.
 */
Vision_Info_Typedef Vision_Info;

/* USER CODE BEGIN Header_Vision_Task */
/**
* @brief Function implementing the StartVisionTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Vision_Task */
void Vision_Task(void const * argument)
{
  /* USER CODE BEGIN Vision_Task */
  TickType_t Vision_Task_SysTick = 0;
Vision_Info.Mode=RED; 

  /* Infinite loop */
  for (;;) 
  {
    Vision_Task_SysTick = osKernelSysTick();
    Vision_Send_Info(&Vision_Info);

    /* MiniPC 掉线老化：超时后清掉目标与开火许可，让自瞄回退到手动跟随 */
    if ((uint32_t)(osKernelSysTick() - Vision_LastRx_Tick) > VISION_TIMEOUT_MS)
    {
      Vision_Info.Distance       = 0;
      Vision_Info.IF_Fire_Accept = false;
    }

		
    osDelayUntil(&Vision_Task_SysTick,1);
  }
  /* USER CODE END Vision_Task */
}
//------------------------------------------------------------------------------

static void Vision_Send_Info(Vision_Info_Typedef *Vision_Info){

  uint8_t *Yaw_Pointer, *Pitch_Pointer, *Roll_Pointer, *Shoot_Velocity;
float Pitch_Angle;
	Pitch_Angle=-INS_Info.Pitch_Angle;
  Yaw_Pointer   = (uint8_t *)&INS_Info.Yaw_Angle;
  Pitch_Pointer = (uint8_t *)&Pitch_Angle;
	Roll_Pointer  = (uint8_t *)&INS_Info.Roll_Angle;


  Vision_Info->MiniPC_SendBuf[0] = 0xFF;
  Vision_Info->MiniPC_SendBuf[1] = Vision_Info->Mode;
	Vision_Info->MiniPC_SendBuf[2] = * Pitch_Pointer;
	Vision_Info->MiniPC_SendBuf[3] = *(Pitch_Pointer + 1);
	Vision_Info->MiniPC_SendBuf[4] = *(Pitch_Pointer + 2);
	Vision_Info->MiniPC_SendBuf[5] = *(Pitch_Pointer + 3);
	Vision_Info->MiniPC_SendBuf[6] = * Roll_Pointer;
	Vision_Info->MiniPC_SendBuf[7] = *(Roll_Pointer  + 1);
	Vision_Info->MiniPC_SendBuf[8] = *(Roll_Pointer  + 2);
	Vision_Info->MiniPC_SendBuf[9] = *(Roll_Pointer  + 3);
	Vision_Info->MiniPC_SendBuf[10] = * Yaw_Pointer;
	Vision_Info->MiniPC_SendBuf[11] = *(Yaw_Pointer  + 1);
  Vision_Info->MiniPC_SendBuf[12] = *(Yaw_Pointer  + 2);
  Vision_Info->MiniPC_SendBuf[13] = *(Yaw_Pointer  + 3);
  Vision_Info->MiniPC_SendBuf[14] = 0x00;
  Vision_Info->MiniPC_SendBuf[15] = 0x0D;
	CDC_Transmit_FS(Vision_Info->MiniPC_SendBuf,16);
}

static float bit8TOfloat32(uint8_t change_info[4])
{
	union
	{
    float float32;
		uint8_t  byte[4];
	}u32val;

  u32val.byte[0] = change_info[0];
  u32val.byte[1] = change_info[1];
  u32val.byte[2] = change_info[2];
  u32val.byte[3] = change_info[3];

	return u32val.float32;
}

void Vision_Receive_Info(Vision_Info_Typedef *Vision_Info,uint8_t* Buf, const uint32_t *Len)
{
	Vision_LastRx_Tick = osKernelSysTick();   /* 收到新帧，刷新时间戳 */

	Vision_Info->IF_Fire_Accept = Buf[1];
	
	Vision_Info->Int.Pitch[0] = Buf[2];
	Vision_Info->Int.Pitch[1] = Buf[3];
	Vision_Info->Int.Pitch[2] = Buf[4];
	Vision_Info->Int.Pitch[3] = Buf[5];

	
	Vision_Info->Int.Yaw[0] = Buf[6];
	Vision_Info->Int.Yaw[1] = Buf[7];
	Vision_Info->Int.Yaw[2] = Buf[8];
	Vision_Info->Int.Yaw[3] = Buf[9];
	
	Vision_Info->Int.Distance[0]= Buf[10];
	Vision_Info->Int.Distance[1]= Buf[11];
  Vision_Info->Int.Distance[2]= Buf[12];
	Vision_Info->Int.Distance[3]= Buf[13];
	
//	Vision_Info->Int.Pitch_Now[0]= Buf[30];
//	Vision_Info->Int.Pitch_Now[1]= Buf[31];
//  Vision_Info->Int.Pitch_Now[2]= Buf[32];
//	Vision_Info->Int.Pitch_Now[3]= Buf[33];
//	
//	Vision_Info->Int.Yaw_Now[0]= Buf[34];
//	Vision_Info->Int.Yaw_Now[1]= Buf[35];
//  Vision_Info->Int.Yaw_Now[2]= Buf[36];
//	Vision_Info->Int.Yaw_Now[3]= Buf[37];
	
	Vision_Info->Target_Pitch = bit8TOfloat32(Vision_Info->Int.Pitch);
	Vision_Info->Target_Yaw = bit8TOfloat32(Vision_Info->Int.Yaw);
	Vision_Info->Distance = bit8TOfloat32(Vision_Info->Int.Distance);
	
//	Vision_Info->Pitch_Now=bit8TOfloat32(Vision_Info->Int.Pitch_Now);
//	Vision_Info->Yaw_Now=bit8TOfloat32(Vision_Info->Int.Yaw_Now);
	
}