#ifndef SHOOT_TASK_S
#define SHOOT_TASK_S
#include "Gimbal_Task.h"
#include "referee_info.h"
#include "bsp_tim.h"
#include "math.h"
#include "motor.h"
#include "Comm_Task.h"
#include "bsp_uart.h"
#include "tim.h"
#include "config.h"
#include "config.h"

typedef enum
{
    SHOOT_L,
    SHOOT_R,
	SHOOT_FV,
    SHOOT_FA,
} Pid_Shoot_Enum;

typedef struct
{
  
    struct
    {
        float BulletFeed;
    } Target;
    struct
    {
        int16_t BulletFeed;
    } Output;
    bool Reload;
    int16_t Return_Times;
    int16_t Stuck_Times;
    float Frequency;
    bool Fire_Flag;
    bool Flag_Last;
	  bool IF_Fire_Accept;


    float Fire_Speed_Offset;

    struct
    {
			  float Last_Shoot_time;
        float Last_Firespeed;
        float Heat_Surplus;
        float ShootTime;
        float Shoot_Speed;
			int16_t ShootCount;
    } HeatCtrl;
    short ShootMode_last;
} Shoot_Typedef_Info;



extern Shoot_Typedef_Info Shoot_Info;
#endif