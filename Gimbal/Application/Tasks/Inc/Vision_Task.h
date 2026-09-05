/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : Vision_Task.h
 * @brief          : Vision task
 * @author         : Yan Yuanbin
 * @date           : 2023/05/21
 * @version        : v1.0
 ******************************************************************************
 * @attention      : None
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef VISION_TASK_H
#define VISION_TASK_H

/* Includes ------------------------------------------------------------------*/
#include "config.h"

/* Exported types ------------------------------------------------------------*/
/**
 * @brief typedef structure that contains the information for the Vision.
 */
typedef enum
{
  RED,
  BULE,
  SMALLRED,
  SMALLBULE,
  BIGRED,
  BIGBULE,
} Vision_Mode;

typedef struct
{
  uint8_t MiniPC_SendBuf[16];

  struct
  {
    uint8_t Yaw[4];
    uint8_t Pitch[4];
		uint8_t Yaw_Vel[4];
    uint8_t Pitch_Vel[4];
		uint8_t Yaw_Acc[4];
    uint8_t Pitch_Acc[4];
		uint8_t Distance[4];
		uint8_t Pitch_Now[4];
		uint8_t Yaw_Now[4];
  } Int;

float Distance;
Vision_Mode Mode;
bool Color;
bool IF_Aiming_Enable; 

bool IF_Fire_Accept; 



float Target_Yaw;   
float Target_Yaw_Vel; 
float Target_Yaw_Acc; 
float Target_Pitch; 
float Target_Pitch_Vel; 
float Target_Pitch_Acc; 
float Pitch_Now;
float Yaw_Now;


} Vision_Info_Typedef;


/* Exported variables --------------------------------------------------------*/
extern Vision_Info_Typedef Vision_Info;

extern void Vision_Receive_Info(Vision_Info_Typedef *Vision_Info, uint8_t *Buf, const uint32_t *Len);
/* Exported functions prototypes --------------------------------------------*/

#endif
