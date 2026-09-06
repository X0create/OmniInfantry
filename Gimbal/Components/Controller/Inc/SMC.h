#include "stm32f4xx_hal.h"
#include "math.h"
#include "common_def.h"
#ifndef _SMC_H_
#define _SMC_H_
#define SMC_PARAMETER_NUM 7

typedef struct
{
	float C;
	float K;
	float error_eps;
	float LimitOutput; // 输出最大值
	float J;	 // 估计惯量
	float epsilon;
} SMC_Parameter_Typedef;

typedef struct _SMC_Typedef
{
	float ref; // 初始目标值
	float error;
	float angle;   // 角度反馈，°
	float ang_vel; // 角速度反馈，°/s
	SMC_Parameter_Typedef Param;

	int16_t Output;
	float error_last;
	float dref;	 // 目标值一阶导
	float ddref; // 目标值二阶导
	float refl;	 // 上一次的目标值
	float S;	 // 滑模面
	void (*SMC_Clear)(struct _SMC_Typedef *SMC);
} SMC_Info_Typedef;

extern void SMC_Calculate(SMC_Info_Typedef *SMC_Info,float Target,float angle_now, float angle_vel);
extern void SMC_Init(SMC_Info_Typedef *SMC_Info, float Param[SMC_PARAMETER_NUM]);

 extern SMC_Info_Typedef Gimbal_SMC;
#endif
