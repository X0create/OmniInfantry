#ifndef SHOOT_TASK_S
#define SHOOT_TASK_S
#include "Gimbal_Task.h"
#include "referee_info.h"
#include "bsp_tim.h"
#include "math.h"
#include "motor.h"
#include "Comm_Task.h"
#include "bsp_uart.h"
#include "Vision_Task.h"
#include "config.h"

typedef enum
{
    SHOOT_L,
    SHOOT_R,
	SHOOT_FV,
    SHOOT_FA,
} Pid_Shoot_Enum;

typedef enum
{
    SILENT,
    POSSIBLE,
    SHOOTING,
    FINISHED
} Heat_State_e;
typedef struct
{
    Heat_State_e state;
    float suspect_time_start;
    uint16_t Qnow;
} Heat_State_s;
#define SUSPECT_DURATION_MS 20 // unit: ms

typedef enum
{
Qmax,
Qwarn,
Qsatu,
Qthrsh,
Q_TYPE_NUM,
}FIRE_QUALITY_ENUM;

typedef struct
{

    struct
    {
        float Shoot;
        float BulletFeed;

    } Target;
    struct
    {
        int16_t ShootL;
        int16_t ShootR;
        int16_t BulletFeed;
    } Output;
    bool Reload;
	  bool Fire_Flag;
		bool Flag_Last;
		short ShootMode_last;
    int16_t Return_Times;
    int16_t Stuck_Times;
    float Frequency;
    float Fire_Speed_Offset;
		float Last_Firespeed;
} Shoot_Typedef_Info;


extern Shoot_Typedef_Info Shoot_Info;

#endif
