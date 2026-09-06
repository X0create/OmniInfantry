
//------------------------------------------------------------------------------
/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : bsp_can.c
 * @brief          : bsp can functions
 * @author         : Yan Yuanbin
 * @date           : 2023/04/27
 * @version        : v1.0
 ******************************************************************************
 * @attention      : Pay attention to enable the can filter
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "bsp_can.h"
#include "can.h"
#include "motor.h"
#include "Chassis_Task.h"
#include "Shoot_Task.h"
#include "can_protocol.h"

/* Private variables ---------------------------------------------------------*/
/**
 * @brief the structure that contains the Information of CAN Receive.
 */
CAN_RxFrame_TypeDef CAN1_RxFrame;
CAN_RxFrame_TypeDef CAN2_RxFrame;

/**
 * @brief the array that contains the Information of CAN Receive data.
 */

CAN_TxFrame_TypeDef CAN1_TxFrame = {
    .hcan = &hcan1,
    .header.StdId = 0x00,
    .header.IDE = CAN_ID_STD,
    .header.RTR = CAN_RTR_DATA,
    .header.DLC = 8,
};

CAN_TxFrame_TypeDef CAN2_TxFrame = {
    .hcan = &hcan2,
    .header.StdId = 0x00,
    .header.IDE = CAN_ID_STD,
    .header.RTR = CAN_RTR_DATA,
    .header.DLC = 8,
};

/**
 * @brief  Configures the CAN Filter.
 * @param  None
 * @retval None
 */

void BSP_CAN_Init(void)
{
  CAN_FilterTypeDef CAN1_FilterConfig = {0};
  /* Update the CAN1 filter Conifguration */
  CAN1_FilterConfig.FilterActivation = ENABLE;
  CAN1_FilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  CAN1_FilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  CAN1_FilterConfig.FilterIdHigh = 0x0000;
  CAN1_FilterConfig.FilterIdLow = 0x0000;
  CAN1_FilterConfig.FilterMaskIdHigh = 0x0000;
  CAN1_FilterConfig.FilterMaskIdLow = 0x0000;
  CAN1_FilterConfig.FilterBank = 0;
  CAN1_FilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  CAN1_FilterConfig.SlaveStartFilterBank = 0;
  /* configures the CAN1 filter */
  if (HAL_CAN_ConfigFilter(&hcan1, &CAN1_FilterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* Start the CAN1 module. */
  HAL_CAN_Start(&hcan1);
  /* Enable CAN1 FIFO0 interrupts */
  HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
  /* Update the CAN2 filter Conifguration */
  CAN_FilterTypeDef CAN2_FilterConfig = {0};
  CAN2_FilterConfig.FilterActivation = ENABLE;
  CAN2_FilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  CAN2_FilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  CAN2_FilterConfig.FilterIdHigh = 0x0000;
  CAN2_FilterConfig.FilterIdLow = 0x0000;
  CAN2_FilterConfig.FilterMaskIdHigh = 0x0000;
  CAN2_FilterConfig.FilterMaskIdLow = 0x0000;
  CAN2_FilterConfig.FilterBank = 14;
  CAN2_FilterConfig.FilterFIFOAssignment = CAN_RX_FIFO1;
  CAN2_FilterConfig.SlaveStartFilterBank = 14;
  /* configures the CAN2 filter */
  if (HAL_CAN_ConfigFilter(&hcan2, &CAN2_FilterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* Start the CAN2 module. */
  HAL_CAN_Start(&hcan2);
  /* Enable CAN2 FIFO1 interrupts */
  HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING);
}
//------------------------------------------------------------------------------

/**
 * @brief  USER function to transmit the Specifies message.
 * @param  Instance: pointer to the CAN Register base address
 * @param  data: pointer to the CAN transmit data
 * @retval None
 */
void USER_CAN_TxMessage(CAN_TxFrame_TypeDef *TxHeader)
{
  static uint32_t TxMailbox = 0;
  HAL_CAN_AddTxMessage(TxHeader->hcan, &TxHeader->header, TxHeader->Data, &TxMailbox);
}

//------------------------------------------------------------------------------

/**
 * @brief  USER function to converting the CAN1 received message.
 * @param  Instance: pointer to the CAN Register base address
 * @param  StdId: Specifies the standard identifier.
 * @param  data: array that contains the received massage.
 * @retval None
 */

static void CAN1_RxFifo0RxHandler(uint32_t *StdId, uint8_t Data[8])
{
  DJI_Motor_Info_Update(StdId, Data, &Chassis_Motor[0]);
  DJI_Motor_Info_Update(StdId, Data, &Chassis_Motor[1]);
  DJI_Motor_Info_Update(StdId, Data, &Chassis_Motor[2]);
  DJI_Motor_Info_Update(StdId, Data, &Chassis_Motor[3]);
  DJI_Motor_Info_Update(StdId, Data, &Chassis_Motor[4]);

}
//------------------------------------------------------------------------------

/**
 * @brief  USER function to converting the CAN2 received message.
 * @param  Instance: pointer to the CAN Register base address
 * @param  StdId: Specifies the standard identifier.
 * @param  data: array that contains the received massage.
 * @retval None
 */
static void CAN2_RxFifo1RxHandler(uint32_t *StdId, uint8_t Data[8])
{
  DM_Motor_Info_Update(StdId, Data, &Gimbal_Motor);
  if (*StdId == CAN_ID_GIMBAL_TO_CHASSIS)
  {
    Comm_Info.Move.Vy = -((int16_t)Data[0] << 8 | (int16_t)Data[1]);
    Comm_Info.Move.Vx = ((int16_t)Data[2] << 8 | (int16_t)Data[3]);
		Comm_Info.Shoot_Mode = Data[4]>>6;
    Comm_Info.Gimbal_Mode = Data[4]>>4&0x03;
		Shoot_Info.Fire_Flag= Data[4]>>3&0x01;
		Shoot_Info.IF_Fire_Accept=Data[4]>>2&0x01;
    Comm_Info.LastChassis_Mode = Comm_Info.Chassis_Mode;
    Comm_Info.Chassis_Mode = Data[5];
    remote_ctrl.key.v = ((int16_t)Data[6] << 8 | (int16_t)Data[7]);
  }
	  if (*StdId == CAN_ID_SUPERCAP)
		{
	Chassis_Info.SuperCap.Surplus_Energy=Data[4];
	Chassis_Info.SuperCap.Chassis_Power=	((int16_t)Data[6]<<8|(int16_t)Data[7])/100.f;
		}
}
//------------------------------------------------------------------------------

/**
 * @brief  Rx FIFO 0 message pending callback.
 * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
 *         the configuration information for the specified CANp.
 * @retval None
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  /* Get an CAN frame from the Rx FIFO zone into the message RAM. */
  HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN1_RxFrame.header, CAN1_RxFrame.Data);

  /* judge the instance of receive frame data */
  CAN1_RxFifo0RxHandler(&CAN1_RxFrame.header.StdId, CAN1_RxFrame.Data);
}
//-------------------------------------------------------------------------

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  /* Get an CAN frame from the Rx FIFO zone into the message RAM. */
  HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &CAN2_RxFrame.header, CAN2_RxFrame.Data);

  /* judge the instance of receive frame data */
  CAN2_RxFifo1RxHandler(&CAN2_RxFrame.header.StdId, CAN2_RxFrame.Data);
}

//------------------------------------------------------------------------------
