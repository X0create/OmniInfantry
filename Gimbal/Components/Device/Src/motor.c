/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : motor.c
 * @brief          : motor interfaces functions
 * @author         : Yan Yuanbin
 * @date           : 2023/04/27
 * @version        : v1.0
 ******************************************************************************
 * @attention      : to be tested
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "motor.h"

/* Private function prototypes -----------------------------------------------*/
DJI_Motor_Info_Typedef Shoot_Motor[Shoot_Motor_Num]={
	[Shoot_L]={
.Type=DJI_M3508,
.CANX=CAN1,
.CANFrame.RxStdId=0x202,
	.ERRORHandler.Status=MOTOR_ERROR_NONE,
},
	[Shoot_R]={
.Type=DJI_M3508,
.CANX=CAN1,
.CANFrame.RxStdId=0x201,
	.ERRORHandler.Status=MOTOR_ERROR_NONE,
},

	
};
DM_Motor_Info_Typedef Gimbal_Motor[Gimbal_Motor_Num]={
		[Yaw]=
		{
			.Control_Mode = MIT,
			.Param_Range ={
			   .P_MAX = 3.141593f,
			   .V_MAX = 30.f,
			   .T_MAX = 10.f		
			},
		  .CANFrame = {
				 .TxStdId = 0x02,
				 .RxStdId = 0x09,
			},
		},
	[Pitch]={
		  .Control_Mode = MIT,
			.Param_Range ={
			   .P_MAX = 3.141593f,
			   .V_MAX = 30.f,
			   .T_MAX = 10.f		
			},
		  .CANFrame = {
				 .TxStdId = 0x01,
				 .RxStdId = 0x11,
			},
		},

	
	
};

static int float_to_uint(float x, float x_min, float x_max, int bits);

static float uint_to_float(int X_int, float X_min, float X_max, int Bits);

/**
 * @brief  transform the encoder(0-8192) to anglesum(3.4E38)
 */
static float encoder_to_anglesum(Motor_GeneralInfo_Typedef *, float, uint16_t);
/**
 * @brief  transform the encoder(0-8192) to angle(-180-180)
 */
float encoder_to_angle(Motor_GeneralInfo_Typedef *, float, uint16_t);
/**
 * @brief  Judge the DJI Motor state
 */
static void DJI_Motor_ErrorHandler(DJI_Motor_Info_Typedef *DJI_Motor);

/**
 * @brief  Update the DJI motor Information
 * @param  StdId  pointer to the specifies the standard identifier.
 * @param  rxBuf  pointer to the can receive data
 * @param  DJI_Motor pointer to a DJI_Motor_Info_t structure
 *         that contains the information of DJI motor
 * @retval None
 */
void DJI_Motor_Info_Update(uint32_t *StdId, uint8_t *rxBuf, DJI_Motor_Info_Typedef *DJI_Motor)
{
  /* check the StdId */
  if (*StdId != DJI_Motor->CANFrame.RxStdId)
    return;

  /* transforms the  general motor data */
  DJI_Motor->Data.temperature = rxBuf[6];
  DJI_Motor->Data.encoder = ((int16_t)rxBuf[0] << 8 | (int16_t)rxBuf[1]);
  DJI_Motor->Data.velocity = ((int16_t)rxBuf[2] << 8 | (int16_t)rxBuf[3]);
  DJI_Motor->Data.current = ((int16_t)rxBuf[4] << 8 | (int16_t)rxBuf[5]);

  /* Judge the motor error	*/
  DJI_Motor_ErrorHandler(DJI_Motor);

  /* update the txframe id and index */
  if (DJI_Motor->Data.Initlized != true)
  {
    if (DJI_Motor->CANFrame.RxStdId > DJI_RxFrame_Middle)
    {
			if(DJI_Motor->Type==DJI_GM6020)
			{
      DJI_Motor->CANFrame.TxStdId = DJI_TxFrame_Voltage;
      DJI_Motor->CANFrame.FrameIndex = 2 * (DJI_Motor->CANFrame.RxStdId - DJI_RxFrame_Middle - 0x01U);
			}
			else 
			{
			DJI_Motor->CANFrame.TxStdId = DJI_TxFrame_High;
      DJI_Motor->CANFrame.FrameIndex = 2 * (DJI_Motor->CANFrame.RxStdId - DJI_RxFrame_Middle - 0x01U);
			}
    }
    else if (DJI_Motor->CANFrame.RxStdId > DJI_TxFrame_Low)
    {
      DJI_Motor->CANFrame.TxStdId = DJI_TxFrame_Low;
      DJI_Motor->CANFrame.FrameIndex = 2 * (DJI_Motor->CANFrame.RxStdId - DJI_TxFrame_Low - 0x01U);
    }
  }

  /* transform the encoder to anglesum */
  switch (DJI_Motor->Type)
  {
  case DJI_GM6020:
    DJI_Motor->Data.angle = encoder_to_angle(&DJI_Motor->Data, 1.f, DJI_ENCODER_RESOLUTION);
    break;

  case DJI_M3508:
    DJI_Motor->Data.angle = encoder_to_anglesum(&DJI_Motor->Data, M3508_REDUCTION_RATIO, DJI_ENCODER_RESOLUTION);
    break;

  case DJI_M2006:
    DJI_Motor->Data.angle = encoder_to_anglesum(&DJI_Motor->Data, M2006_REDUCTION_RATIO, DJI_ENCODER_RESOLUTION);
    break;

  default:
    break;
  }
	
}
//------------------------------------------------------------------------------

/**
 * @brief  Update the RMD motor Information
 * @param  StdId  pointer to the specifies the standard identifier.
 * @param  rxBuf  pointer to the can receive data
 * @param  RMD_Motor pointer to a RMD_L9025_Info_Typedef structure that contains the information of RMD motor
 * @retval None
 */
void RMD_Motor_Info_Update(uint32_t *StdId, uint8_t *rxBuf, RMD_L9025_Info_Typedef *RMD_Motor)
{
  /* Judge the StdId */
  if (*StdId != RMD_Motor->CANFrame.RxStdId)
  {
    return;
  }

  /* Update the receive order */
  RMD_Motor->order = rxBuf[0];

  /* transforms the  general motor data */
  RMD_Motor->Data.temperature = rxBuf[1];
  RMD_Motor->Data.current = ((int16_t)(rxBuf[2]) | (int16_t)(rxBuf[3] << 8));
  RMD_Motor->Data.velocity = ((int16_t)(rxBuf[4]) | (int16_t)(rxBuf[5] << 8));
  RMD_Motor->Data.encoder = ((int16_t)(rxBuf[6]) | (int16_t)(rxBuf[7] << 8));

  /* transform the encoder to anglesum */
  switch (RMD_Motor->Type)
  {
  case RMD_L9025:
    RMD_Motor->Data.angle = encoder_to_anglesum(&RMD_Motor->Data, 1.f, RMD_ENCODER_RESOLUTION);
    break;

  default:
    break;
  }
}
void DM_Motor_Command(CAN_TxFrame_TypeDef *CAN_TxFrame,DM_Motor_Info_Typedef *DM_Motor,uint8_t CMD){

	 CAN_TxFrame->header.StdId = DM_Motor->CANFrame.TxStdId;
  	
	 CAN_TxFrame->Data[0] = 0xFF;
   CAN_TxFrame->Data[1] = 0xFF;
 	 CAN_TxFrame->Data[2] = 0xFF;
	 CAN_TxFrame->Data[3] = 0xFF;
	 CAN_TxFrame->Data[4] = 0xFF;
	 CAN_TxFrame->Data[5] = 0xFF;
	 CAN_TxFrame->Data[6] = 0xFF;
	
	 switch(CMD){
		 
		  case Motor_Enable :
	        CAN_TxFrame->Data[7] = 0xFC; 
	    break;
      
			case Motor_Disable :
	        CAN_TxFrame->Data[7] = 0xFD; 
      break;
      
			case Motor_Save_Zero_Position :
	        CAN_TxFrame->Data[7] = 0xFE; 
			break;
			
			default:
	    break;   
	}
	
   USER_CAN_TxMessage(CAN_TxFrame);

}

/**
  * @brief  CAN Transmit DM motor Information
  * @param  *CAN_TxFrame  pointer to the CAN_TxFrame_TypeDef.
  * @param  *DM_Motor  pointer to the DM_Motor
  * @param  Postion Velocity KP KD Torgue 
  * @retval None
  */
void DM_Motor_CAN_TxMessage(CAN_TxFrame_TypeDef *CAN_TxFrame,DM_Motor_Info_Typedef *DM_Motor,float Postion, float Velocity, float KP, float KD, float Torque){
	
   if(DM_Motor->Control_Mode == MIT){
		 
		 uint16_t Postion_Tmp,Velocity_Tmp,Torque_Tmp,KP_Tmp,KD_Tmp;
		 
		 Postion_Tmp  =  float_to_uint(Postion, -DM_Motor->Param_Range.P_MAX,DM_Motor->Param_Range.P_MAX,16) ;
		 Velocity_Tmp =  float_to_uint(Velocity,-DM_Motor->Param_Range.V_MAX,DM_Motor->Param_Range.V_MAX,12);
		 Torque_Tmp   =  float_to_uint(Torque,  -DM_Motor->Param_Range.T_MAX,DM_Motor->Param_Range.T_MAX,12);
		 KP_Tmp = float_to_uint(KP,0,DM_KP_MAX,12);
		 KD_Tmp = float_to_uint(KD,0,DM_KD_MAX,12);
		
		 CAN_TxFrame->header.StdId = DM_Motor->CANFrame.TxStdId;
		 CAN_TxFrame->Data[0] = (uint8_t)(Postion_Tmp>>8);
		 CAN_TxFrame->Data[1] = (uint8_t)(Postion_Tmp);
		 CAN_TxFrame->Data[2] = (uint8_t)(Velocity_Tmp>>4);
		 CAN_TxFrame->Data[3] = (uint8_t)((Velocity_Tmp&0x0F)<<4) | (uint8_t)(KP_Tmp>>8);
		 CAN_TxFrame->Data[4] = (uint8_t)(KP_Tmp);
		 CAN_TxFrame->Data[5] = (uint8_t)(KD_Tmp>>4);
		 CAN_TxFrame->Data[6] = (uint8_t)((KD_Tmp&0x0F)<<4) | (uint8_t)(Torque_Tmp>>8);
		 CAN_TxFrame->Data[7] = (uint8_t)(Torque_Tmp);

	}else if(DM_Motor->Control_Mode == POSITION_VELOCITY){
	
		 uint8_t *Postion_Tmp,*Velocity_Tmp;
		
		 Postion_Tmp  = (uint8_t*) & Postion;
		 Velocity_Tmp = (uint8_t*) & Velocity;
		
	   CAN_TxFrame->header.StdId = DM_Motor->CANFrame.TxStdId;
		 CAN_TxFrame->Data[0] = *(Postion_Tmp);
		 CAN_TxFrame->Data[1] = *(Postion_Tmp + 1);
		 CAN_TxFrame->Data[2] = *(Postion_Tmp + 2);
		 CAN_TxFrame->Data[3] = *(Postion_Tmp + 3);
	   CAN_TxFrame->Data[4] = *(Velocity_Tmp);
		 CAN_TxFrame->Data[5] = *(Velocity_Tmp + 1);
		 CAN_TxFrame->Data[6] = *(Velocity_Tmp + 2);
		 CAN_TxFrame->Data[7] = *(Velocity_Tmp + 3);
		
	}else if(DM_Motor->Control_Mode == VELOCITY){
	
	  uint8_t *Velocity_Tmp;
		Velocity_Tmp = (uint8_t*) & Velocity;
		
    CAN_TxFrame->header.StdId = DM_Motor->CANFrame.TxStdId;
		CAN_TxFrame->Data[0] = *(Velocity_Tmp);
		CAN_TxFrame->Data[1] = *(Velocity_Tmp + 1);
		CAN_TxFrame->Data[2] = *(Velocity_Tmp + 2);
		CAN_TxFrame->Data[3] = *(Velocity_Tmp + 3);

	}
	 
	  USER_CAN_TxMessage(CAN_TxFrame);	

}

/**
  * @brief  Update the DM_Motor Information
  * @param  StdId  pointer to the specifies the standard identifier.
  * @param  Rx_Buf  pointer to the can receive data
  * @param  DM_Motor pointer to a DM_Motor_Info_Typedef structure that contains the information of DM_Motor
  * @retval None
  */
void DM_Motor_Info_Update(uint32_t *StdId,uint8_t *Rx_Buf,DM_Motor_Info_Typedef *DM_Motor)
{
	/* Judge the StdId */
	 if(*StdId != DM_Motor->CANFrame.RxStdId) return;
	
	  DM_Motor->Data.State = Rx_Buf[0]>>4;
		DM_Motor->Data.P_int = ((uint16_t)(Rx_Buf[1]) <<8) | ((uint16_t)(Rx_Buf[2]));
		DM_Motor->Data.V_int = ((uint16_t)(Rx_Buf[3]) <<4) | ((uint16_t)(Rx_Buf[4])>>4);
		DM_Motor->Data.T_int = ((uint16_t)(Rx_Buf[4]&0xF) <<8) | ((uint16_t)(Rx_Buf[5]));
		DM_Motor->Data.Torque=  uint_to_float(DM_Motor->Data.T_int,-DM_Motor->Param_Range.T_MAX,DM_Motor->Param_Range.T_MAX,12);
		DM_Motor->Data.Position=uint_to_float(DM_Motor->Data.P_int,-DM_Motor->Param_Range.P_MAX,DM_Motor->Param_Range.P_MAX,16);
    DM_Motor->Data.Velocity=uint_to_float(DM_Motor->Data.V_int,-DM_Motor->Param_Range.V_MAX,DM_Motor->Param_Range.V_MAX,12);
    DM_Motor->Data.Angle=DM_Motor->Data.Position*RadiansToDegrees;
    DM_Motor->Data.Temperature_MOS   = (float)(Rx_Buf[6]);
		DM_Motor->Data.Temperature_Rotor = (float)(Rx_Buf[7]);

}


//------------------------------------------------------------------------------

/**
 * @brief  transform the encoder(0-8192) to anglesum(3.4E38)
 * @param  *Info        pointer to a Motor_GeneralInfo_Typedef structure that
 *					             contains the infomation for the specified motor
 * @param  torque_ratio the specified motor torque ratio
 * @param  MAXencoder   the specified motor max encoder number
 * @retval anglesum
 */
static float encoder_to_anglesum(Motor_GeneralInfo_Typedef *Info, float torque_ratio, uint16_t MAXencoder)
{
  float res1 = 0, res2 = 0;

  if (Info == NULL)
    return 0;

  /* Judge the motor Initlized */
  if (Info->Initlized != true)
  {
    /* update the last encoder */
    Info->last_encoder = Info->encoder;

    /* reset the angle */
    Info->angle = 0;

    /* Set the init flag */
    Info->Initlized = true;
  }

  /* get the possiable min encoder err */
  if (Info->encoder < Info->last_encoder)
  {
    res1 = Info->encoder - Info->last_encoder + MAXencoder;
  }
  else if (Info->encoder > Info->last_encoder)
  {
    res1 = Info->encoder - Info->last_encoder - MAXencoder;
  }
  res2 = Info->encoder - Info->last_encoder;

  /* update the last encoder */
  Info->last_encoder = Info->encoder;

  /* transforms the encoder data to tolangle */
  if (fabsf(res1) > fabsf(res2))
  {
    Info->angle += (float)res2 / (MAXencoder * torque_ratio) * 360.f;
  }
  else
  {
    Info->angle += (float)res1 / (MAXencoder * torque_ratio) * 360.f;
  }

  return Info->angle;
}
//------------------------------------------------------------------------------

/**
 * @brief  float loop constrain
 * @param  Input    the specified variables
 * @param  minValue minimum number of the specified variables
 * @param  maxValue maximum number of the specified variables
 * @retval variables
 */
static float f_loop_constrain(float Input, float minValue, float maxValue)
{
  if (maxValue < minValue)
  {
    return Input;
  }

  float len = maxValue - minValue;

  if (Input > maxValue)
  {
    do
    {
      Input -= len;
    } while (Input > maxValue);
  }
  else if (Input < minValue)
  {
    do
    {
      Input += len;
    } while (Input < minValue);
  }
  return Input;
}
//------------------------------------------------------------------------------

/**
 * @brief  transform the encoder(0-8192) to angle(-180-180)
 * @param  *Info        pointer to a Motor_GeneralInfo_Typedef structure that
 *					             contains the infomation for the specified motor
 * @param  torque_ratio the specified motor torque ratio
 * @param  MAXencoder   the specified motor max encoder number
 * @retval angle
 */
float encoder_to_angle(Motor_GeneralInfo_Typedef *Info, float torque_ratio, uint16_t MAXencoder)
{
  float encoder_err = 0.f;

  /* check the motor init */
  if (Info->Initlized != true)
  {
    /* update the last encoder */
    Info->last_encoder = Info->encoder;

    /* reset the angle */
    Info->angle = Info->encoder/(MAXencoder*torque_ratio)*360.f;;

    /* config the init flag */
    Info->Initlized = true;
  }

  encoder_err = Info->encoder - Info->last_encoder;

  /* 0 -> MAXencoder */
  if (encoder_err > MAXencoder * 0.5f)
  {
    Info->angle += (float)(encoder_err - MAXencoder) / (MAXencoder * torque_ratio) * 360.f;
  }
  /* MAXencoder-> 0 */
  else if (encoder_err < -MAXencoder * 0.5f)
  {
    Info->angle += (float)(encoder_err + MAXencoder) / (MAXencoder * torque_ratio) * 360.f;
  }
  else
  {
    Info->angle += (float)(encoder_err) / (MAXencoder * torque_ratio) * 360.f;
  }

  /* update the last encoder */
  Info->last_encoder = Info->encoder;

  /* loop constrain */
  f_loop_constrain(Info->angle, -180.f, 180.f);

  return Info->angle;
}

static float uint_to_float(int X_int, float X_min, float X_max, int Bits){
	
    float span = X_max - X_min;
    float offset = X_min;
    return ((float)X_int)*span/((float)((1<<Bits)-1)) + offset;
}

static int float_to_uint(float x, float x_min, float x_max, int bits){
	
    float span = x_max - x_min;
    float offset = x_min;
    return (int) ((x-offset)*((float)((1<<bits)-1))/span);
}


//------------------------------------------------------------------------------

/**
 * @brief  Judge the DJI Motor state
 * @param  *DJI_Motor pointer to a DJI_Motor_Info_Typedef structure that contains
 *                    the configuration information for the specified motor.
 * @retval None
 */
static void DJI_Motor_ErrorHandler(DJI_Motor_Info_Typedef *DJI_Motor)
{
  /* Judge the DJI motor temperature */

  if ((DJI_Motor->Data.temperature > 50)||(DJI_Motor->Data.temperature < 0))
  {
    DJI_Motor->ERRORHandler.Temp_ErrorCount++;

    if (DJI_Motor->ERRORHandler.Temp_ErrorCount > 500)
    {
      DJI_Motor->ERRORHandler.Status = MOTOR_OVER_TEMPERATURE;
      DJI_Motor->ERRORHandler.Temp_ErrorCount = 0;
    }
  }
  else
  {
    DJI_Motor->ERRORHandler.Temp_ErrorCount = 0;
  }


}
//------------------------------------------------------------------------------
