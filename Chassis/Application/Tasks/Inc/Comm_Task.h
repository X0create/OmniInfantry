#ifndef COMM_TASK_AGV
#define COMM_TASK_AGV

#include "bsp_can.h"
#include "Keyboard.h"
#include "cmsis_os.h"
#include "Chassis_Task.h"
#include "Shoot_Task.h"
#include "referee_info.h"
#include "ui.h"
#include "ui_types.h"
#include "Shoot_Task.h"

typedef enum
{
  Chassis_Follow,
  Chassis_Spin,
  Chassis_OFF,
  
} Chassis_Mode_Enum;
typedef enum
{
  Gimbal_ON,
  Gimbal_AotoAim,
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
typedef struct
{
  bool Gimbal_Lost;
  bool Chassis_Lost;

	
	Gimbal_Mode_Enum Gimbal_Mode;
	Shoot_Mode_Enum Shoot_Mode;
  uint8_t Board_Can2_Data[8];
  uint8_t Chassis_Can1_Data[8];
  Chassis_Mode_Enum Chassis_Mode;
	Chassis_Mode_Enum LastChassis_Mode;

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
		
} Comm_Info_Typedef;



extern float receive_bullet_speed(uint8_t *data);
extern Comm_Info_Typedef Comm_Info;
#endif
