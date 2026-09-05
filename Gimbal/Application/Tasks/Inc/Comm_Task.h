#ifndef COMM_TASK_AGV
#define COMM_TASK_AGV

#include "Gimbal_Task.h"
#include "bsp_can.h"
#include "motor.h"
#include "Image_Transmission.h"
#include "Keyboard.h"
#include "Shoot_Task.h"
#include "tim.h"

typedef enum
{
  Chassis_Follow,
  Chassis_Spin,
  Chassis_OFF,
  Chassis_Mode_Num
} Chassis_Mode_Enum;
typedef enum
{
  Gimbal_ON,
  Gimbal_AutoAim,
  Gimbal_OFF,
  Gimbal_Mode_Num
} Gimbal_Mode_Enum;
typedef enum
{
	Shoot_OFF,
  Shoot_Repeat,
  Shoot_Single,
  Shoot_Mode_Num
} Shoot_Mode_Enum;
typedef enum
{
  Remote_Control,
  KeyBoard_Control,
  Control_Mode_Num
} Control_Mode_Enum;

typedef enum
{
SmallFu,
BigFu,
}Dafu_Mode_Enum;

typedef enum
{
  DT7_Ctrl,
  VT13_Ctrl,
  Remote_Type_Num
} Remote_Type_Enum;
typedef struct
{
  bool Gimbal_Lost;
	bool Gimbal_Lost_Last;
  bool Chassis_Lost;
	bool Robot_Lost;
  uint8_t Board_Can2_Data[8];
  uint8_t Gimbal_Can1_Data[8];
  uint8_t Gimbal_Can2_Data[8];
  Chassis_Mode_Enum Chassis_Mode;
  Gimbal_Mode_Enum Gimbal_Mode;
	Gimbal_Mode_Enum Last_Gimbal_Mode;
  Shoot_Mode_Enum Shoot_Mode;
  Control_Mode_Enum Control_Mode;
  Remote_Type_Enum Remote_State;
	Dafu_Mode_Enum DaFu_Mode;
  struct
  {
    int16_t Vx;
    int16_t Vy;
  } Move;
    struct
    {
      bool Referee_Update_Flag;
      uint8_t Robot_Level;
      uint8_t Cooling_Buff;
      int16_t barrel_cooling_value;
      float barrel_heat_17mm;
      float barrel_heat_limit;
      float Shoot_Velocity;
    } Referee;
		struct{
					union
	{
		float Angle;
		int8_t angle[4];
	} Yaw;
		union
	{
		float Angle;
		int8_t angle[4];
	} Pitch;
		union
	{
		float Angle;
		int8_t angle[4];
	} Roll;
	
			
		}Chassis;
		
}Comm_Info_Typedef;



extern float receive_bullet_speed(uint8_t *data);
extern Comm_Info_Typedef Comm_Info;
#endif
