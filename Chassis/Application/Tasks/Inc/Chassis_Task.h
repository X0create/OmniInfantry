#ifndef CHASSIS_TASK_M
#define CHASSIS_TASK_M
#include "cmsis_os.h"
#include "pid.h"
#include "Comm_Task.h"
#include "motor.h"
#include "arm_math.h"
#include "INS_Task.h"
#include "PowerCtrl.h"
#include "bsp_uart.h"

typedef enum
{
	FL,
	BL,
	BR,
	FR,
	CHASSIS_NUM
}Chassis_Enum_Typedef;
typedef struct
{
	struct{
	  float Vx;
		float Vy;
		float Vw;
		float Wheelrpm[4];
		float Follow_Err;
	}Target;
	
	struct 
	{
		float MidAngle;
		float Angle;
		float COS;
		float SIN;
	} Transform;
	
	struct{
		float Vx;
		float Vy;
		float Vw;
	}Row;
	
	struct{
	float resistance[4];
	}
Feedback;
	
struct
{
bool SuperCap_Flag;
bool EnableCap;
float Chassis_Power;
float Surplus_Energy;
	
}SuperCap;
int16_t Output[4];


}Chassis_Info_Typedef;


extern Chassis_Info_Typedef Chassis_Info;
extern PowerCtrl_Typedef Chassis_PowerCtrl;
































#endif