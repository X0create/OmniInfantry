#include "SMC.h"
static float SAT(float y)
{

	if (fabs(y) <= 1)
	{
		return y;
	}
	// 符号函数
	else
	{
		if (y > 0)
		{
			return 1.0f; // y > 1 时返回 1
		}
		else if (y == 0)
		{

			return 0.0f;
		}
		else
		{
			return -1.0f; // y < -1 时返回 -1
		}
	}
}
static void SMC_Clear(SMC_Info_Typedef *SMC_Info)
{
	SMC_Info->ref = 0;
	SMC_Info->Param.C = 20;
	SMC_Info->Param.K = 120;
	SMC_Info->Param.error_eps = 0.001;
	SMC_Info->Param.LimitOutput = 25000;
	SMC_Info->Param.J = 0.8;
	SMC_Info->Param.epsilon = 0.5;
}

void SMC_Init(SMC_Info_Typedef *SMC_Info,float Param[SMC_PARAMETER_NUM])
{
	SMC_Info->SMC_Clear = SMC_Clear;
	SMC_Info->Param.C = Param[0];
	SMC_Info->Param.K = Param[1];
	SMC_Info->Param.error_eps = Param[2];
	SMC_Info->Param.LimitOutput = Param[3];
	SMC_Info->Param.J = Param[4];
	SMC_Info->Param.epsilon = Param[5];
}

void SMC_Calculate(SMC_Info_Typedef *SMC_Info,float Target ,float angle, float angle_vel)
{

	SMC_Info->ref = Target;
	SMC_Info->angle = angle;
	SMC_Info->ang_vel = angle_vel;
	SMC_Info->error = SMC_Info->angle - SMC_Info->ref;
	SMC_Info->ddref = (SMC_Info->ref - SMC_Info->refl) - SMC_Info->dref; // 这里对前馈进行了处理，没有严格单位统一
	SMC_Info->dref = (SMC_Info->ref - SMC_Info->refl);
	

	if (fabs(SMC_Info->error) < SMC_Info->Param.error_eps)
	{
		SMC_Info->Output = 0;
		return;
	}
	SMC_Info->S = SMC_Info->Param.C * SMC_Info->error + (SMC_Info->ang_vel - SMC_Info->dref);
	SMC_Info->Output = -SMC_Info->Param.J * (SMC_Info->ddref - SMC_Info->Param.C * (SMC_Info->ang_vel - SMC_Info->dref) - SMC_Info->Param.epsilon * SAT(SMC_Info->S) - SMC_Info->Param.K * SMC_Info->S);
	// 控制量限幅
		VAL_LIMIT(SMC_Info->Output, -SMC_Info->Param.LimitOutput, SMC_Info->Param.LimitOutput);
	// 参数更新
    SMC_Info->refl = SMC_Info->ref;
};
