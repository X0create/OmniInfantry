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


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DEVICE_MOTOR_H
#define DEVICE_MOTOR_H


/* Includes ------------------------------------------------------------------*/
#include "common_def.h"
#include "stm32f4xx.h"
#include "pid.h"
#include "bsp_can.h"

/* Exported constants --------------------------------------------------------*/
/**
  * @brief 电机物理常量
  * @note  这些值由电机型号决定，与装在哪台车上无关，两块板应保持一致。
  *        过温阈值一类的"策略"参数不在这里（两板取值不同）。
  */
/** DJI 电调编码器一圈的计数值 */
#define DJI_ENCODER_RESOLUTION   8192
/** RMD 电机编码器一圈的计数值 */
#define RMD_ENCODER_RESOLUTION   32768
/** M3508 减速比，3591/187 约等于 19.2:1 */
#define M3508_REDUCTION_RATIO    3591.f / 187.f
/** M2006 编码器到输出轴角度的换算系数
  * @note 手册标称减速比为 36:1，原代码取 90.f。此处只是把数值具名化，
  *       未做任何改动；若发现角度读数不对，先查这个值。 */
#define M2006_REDUCTION_RATIO    90.f
/** 达妙电机 MIT 模式下 KP 的取值上限（电机固件常量） */
#define DM_KP_MAX                500
/** 达妙电机 MIT 模式下 KD 的取值上限（电机固件常量） */
#define DM_KD_MAX                5


/* Exported types ------------------------------------------------------------*/
enum 
{
	Yaw,
Pitch,
Gimbal_Motor_Num
};
	
enum 
{
Shoot_L,
Shoot_R,
Shoot_F,
Shoot_Motor_Num
};



/**
 * @brief typedef enum that contains the Frame Identifier for DJI Motor Device.
 */
typedef enum
{
  DJI_TxFrame_Low = 0x200,
  DJI_RxFrame_Middle = 0x204,
	DJI_TxFrame_Voltage = 0x1fe,
	DJI_TxFrame_High=0x1ff,
	DJI_TxFrame_Current =0x1ff,
  DJI_MotorFrameId_NUM,
} DJI_MotorFrameId_e;

/**
 * @brief typedef enum that contains the Error status for Motor Device.
 */
typedef enum
{
  MOTOR_ERROR_NONE = 0x00U,   /*!< no error */
  MOTOR_CAN_OFFLINE = 0x01U,    /*!< CAN transfer failed */
  MOTOR_OVER_TEMPERATURE = 0x02U,   /*!< abnormal motor temperature */
}Motor_Status_e;


/**
 * @brief typedef enum that contains the type of RMD Motor Device.
 */
typedef enum{
	  RMD_L9025,
    RMD_MOTOR_TYPE_NUM,
}RMD_Motor_Type_e;

/**
 * @brief typedef enum that contains the type of DJI Motor Device.
 */
typedef enum{
    DJI_GM6020,
    DJI_M3508,
    DJI_M2006,
    DJI_MOTOR_TYPE_NUM,
}DJI_Motor_Type_e;

/**
 * @brief typedef structure that contains the information for the Motor Error handler.
 */
typedef struct 
{
  uint16_t Temp_ErrorCount;    /*!< Error status judgment count */
  uint16_t CAN_ErrorCount; 
  Motor_Status_e Status;  /*!< Error status */
Motor_Status_e Last_status;
}Motor_ErrorrHandler_Typedef;

/**
 * @brief typedef structure that contains the information for the Motor CAN Transfer.
 */
typedef struct
{
  uint32_t TxStdId;   /*!< Specifies CAN transfer identifier */
  uint32_t RxStdId;   /*!< Specifies CAN transfer identifier */
  uint8_t FrameIndex;   /* index for motor transmit frame */
}Motor_CANFrameInfo_typedef;

/**
 * @brief typedef structure that contains the General information for the Motor Device.
 */
typedef struct 
{
  bool Initlized;   /*!< init flag */
  int16_t  current;   /*!< Motor electric current */
	int16_t Lastcurrent;
  int16_t  velocity;    /*!< Motor rotate velocity */
  int16_t  encoder;   /*!< Motor encoder angle */
  int16_t  last_encoder;   /*!< previous Motor encoder angle */
  float    angle;   /*!< Motor angle in degree */
  uint8_t  temperature;   /*!< Motor Temperature */
}Motor_GeneralInfo_Typedef;

/**
 * @brief typedef structure that contains the information for the DJI Motor Device.
 */
typedef struct
{
	DJI_Motor_Type_e Type;   /*!< Type of Motor */
  Motor_CANFrameInfo_typedef CANFrame;    /*!< information for the CAN Transfer */
	Motor_GeneralInfo_Typedef Data;   /*!< information for the Motor Device */
	Motor_ErrorrHandler_Typedef ERRORHandler;   /*!< information for the Motor Error */
  CAN_TypeDef *CANX;
}DJI_Motor_Info_Typedef;

/**
 * @brief typedef structure that contains the information for the DJI Motor Device.
 */
typedef struct
{
	uint8_t order;   /*!< Motor feedback order */
	RMD_Motor_Type_e Type;   /*!< Type of Motor */
  Motor_CANFrameInfo_typedef CANFrame;    /*!< information for the CAN Transfer */
	Motor_GeneralInfo_Typedef Data;   /*!< information for the Motor Device */
	Motor_ErrorrHandler_Typedef ERRORHandler;   /*!< information for the Motor Error */
}RMD_L9025_Info_Typedef;

/**
 * @brief  typedef enum that control mode the type of DMMotor Device..
 */
typedef enum
{
  MIT,
	POSITION_VELOCITY,
	VELOCITY,
}DM_Motor_Control_Mode_Type_e;

typedef enum{

  Motor_Enable,
  Motor_Disable,
  Motor_Save_Zero_Position,
  DM_Motor_CMD_Type_Num,

}DM_Motor_CMD_Type_e;
/**
 * @brief typedef structure that contains the Param_Range for the DM_Motor .
 */
typedef struct 
{

  float  P_MAX;
	float  V_MAX;
	float  T_MAX;

}DM_Motor_Param_Range_Typedef;

/**
 * @brief typedef structure that contains the information for the DJI Motor Device.
 */
typedef struct 
{
  bool Initlized;   /*!< init flag */
  uint8_t  State; 	/*!< Motor ERROR Message */
  uint16_t  P_int;
	uint16_t  V_int;
	uint16_t  T_int;
	float  Position;   /*!< Motor Positon */
  float  Velocity;   /*!< Motor Velocity  */
  float  Torque;  /*!< Motor Torque */
  float  Temperature_MOS;   /*!< Motor Temperature_MOS */
	float  Temperature_Rotor;   /*!< Motor Temperature_Rotor */
  float  Angle;	
}DM_Motor_GeneralInfo_Typedef;


/**
 * @brief typedef structure that contains the information for the DJI Motor Device.
 */
typedef struct
{
  DM_Motor_Control_Mode_Type_e	Control_Mode;
  Motor_CANFrameInfo_typedef CANFrame;    /*!< information for the CAN Transfer */
	DM_Motor_Param_Range_Typedef Param_Range;
	DM_Motor_GeneralInfo_Typedef Data;   /*!< information for the Motor Device */
}DM_Motor_Info_Typedef;

typedef struct
{
  float Position;
	float Velocity;
	float KP;
	float KD;
	float Torque;
	float Angle;
}DM_Motor_Contorl_Info_Typedef;


/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief  Update the DJI motor Information
  */
extern void DJI_Motor_Info_Update(uint32_t *StdId, uint8_t *rxBuf,DJI_Motor_Info_Typedef *DJI_Motor);
extern void DM_Motor_CAN_TxMessage(CAN_TxFrame_TypeDef *CAN_TxFrame,DM_Motor_Info_Typedef *DM_Motor,float Postion, float Velocity, float KP, float KD, float Torque);
extern void DM_Motor_Command(CAN_TxFrame_TypeDef *CAN_TxFrame,DM_Motor_Info_Typedef *DM_Motor,uint8_t CMD);
/**
  * @brief  Update the RMD motor Information
	*/
extern void RMD_Motor_Info_Update(uint32_t *StdId, uint8_t *rxBuf,RMD_L9025_Info_Typedef *RMD_Motor);

extern void DM_Motor_Info_Update(uint32_t *StdId,uint8_t *Rx_Buf,DM_Motor_Info_Typedef *DM_Motor);
extern DJI_Motor_Info_Typedef Motor;
extern DM_Motor_Info_Typedef Gimbal_Motor[Gimbal_Motor_Num];
extern DJI_Motor_Info_Typedef Shoot_Motor[Shoot_Motor_Num];
#endif //DEVICE_MOTOR_H
