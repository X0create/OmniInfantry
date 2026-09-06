#include "PowerCtrl.h"
#include "referee_info.h"
#include "Chassis_Task.h"  
#include "LPF.h"
RLS_Info_TypeDef RLS_Power_Info;
LowPassFilter1p_Info_TypeDef LPF_Current[4];

static float fm_sqrt_fast(float x) {
    if (x <= 0.0f) return 0.0f;
    
    /* 使用改进的Quake III算法 */
    int32_t u;
    float f = x;
    u = 0x5f375a86 - (u >> 1);  /* 比原始常量更精确 */
    
    /* 一次牛顿迭代 */
    float xhalf = 0.5f * x;
    f = f * (1.5f - xhalf * f * f);
    
    /* 根据精度需求决定是否进行第二次迭代 */
#if FM_PRECISE_MODE
    u.f = u.f * (1.5f - xhalf * u.f * u.f);
#endif
    
    return x * f;
}

void PowerCtrl_Init(PowerCtrl_Typedef *PowerCtrl_Info, Chassis_Tpye_e Type, float Lamda, float P, float PowerCtrl_Param[POWERCTRL_TYPE_NUM])
{
    if (Type == MACUNUM)
    {
        RLS_Init(&RLS_Power_Info, 2, 1, Lamda, P);
        PowerCtrl_Info->Param.K1 = PowerCtrl_Param[0];
        PowerCtrl_Info->Param.K2 = PowerCtrl_Param[1];
        PowerCtrl_Info->Param.K3 = PowerCtrl_Param[2];
        PowerCtrl_Info->Param.Err_Lower = PowerCtrl_Param[3];
        PowerCtrl_Info->Param.Err_Upper = PowerCtrl_Param[4];
        RLS_Power_Info.Data.W[0] = PowerCtrl_Info->Param.K1;
        RLS_Power_Info.Data.W[1] = PowerCtrl_Info->Param.K2;
    }
    else if (Type == STEER)
    {
        RLS_Init(&RLS_Power_Info, 4, 1, Lamda, P);
        PowerCtrl_Info[0].Param.K1 = PowerCtrl_Param[0];
        PowerCtrl_Info[0].Param.K2 = PowerCtrl_Param[1];
        PowerCtrl_Info[0].Param.K3 = PowerCtrl_Param[2];
        PowerCtrl_Info[0].Param.Err_Lower = PowerCtrl_Param[3];
        PowerCtrl_Info[0].Param.Err_Upper = PowerCtrl_Param[4];
        PowerCtrl_Info[1].Param.K1 = PowerCtrl_Param[0];
        PowerCtrl_Info[1].Param.K2 = PowerCtrl_Param[1];
        PowerCtrl_Info[1].Param.K3 = PowerCtrl_Param[2];
        PowerCtrl_Info[1].Param.Err_Lower = PowerCtrl_Param[3];
        PowerCtrl_Info[1].Param.Err_Upper = PowerCtrl_Param[4];
        RLS_Power_Info.Data.W[0] = PowerCtrl_Info[0].Param.K1;
        RLS_Power_Info.Data.W[1] = PowerCtrl_Info[0].Param.K2;
        RLS_Power_Info.Data.W[2] = PowerCtrl_Info[1].Param.K1;
        RLS_Power_Info.Data.W[3] = PowerCtrl_Info[1].Param.K2;
			for (int i = 0; i < 4; i++)
			  LowPassFilter1p_Init(&LPF_Current[i],0.7);
		
    }
}
void PowerCtrl(PowerCtrl_Typedef *PowerCtrl_Info, PID_Info_TypeDef Pid[4], DJI_Motor_Info_Typedef Chassis_Motor[4])
{
	  
    for (int i = 0; i < 4; i++)
    {
			  LowPassFilter1p_Update(&LPF_Current[i],Chassis_Motor[i].Data.current);
        PowerCtrl_Info->Target.Omiga[i] = (Chassis_Motor[i].Data.velocity / 9.55f);    // 转速转角速度
        PowerCtrl_Info->Target.Torque[i] = (LPF_Current[i].Output * 1.927e-5); // 电流转转矩
        PowerCtrl_Info->Target.Omiga_2[i] = powf(PowerCtrl_Info->Target.Omiga[i], 2.f);
        PowerCtrl_Info->Target.Torque_2[i] = powf(PowerCtrl_Info->Target.Torque[i], 2.f);
        PowerCtrl_Info->Target.Power_In[i] = (PowerCtrl_Info->Param.K1 * PowerCtrl_Info->Target.Torque_2[i] + PowerCtrl_Info->Param.K2 * PowerCtrl_Info->Target.Omiga_2[i] +PowerCtrl_Info->Target.Omiga[i] * PowerCtrl_Info->Target.Torque[i]); // 根据模型预测单个电机的功率
    }

    PowerCtrl_Info->Sum.Torque2_Sum = PowerCtrl_Info->Target.Torque_2[0] + PowerCtrl_Info->Target.Torque_2[1] + PowerCtrl_Info->Target.Torque_2[2] + PowerCtrl_Info->Target.Torque_2[3];                          // 计算输出总力矩平方和
    PowerCtrl_Info->Sum.Omiga2_Sum = PowerCtrl_Info->Target.Omiga_2[0] + PowerCtrl_Info->Target.Omiga_2[1] + PowerCtrl_Info->Target.Omiga_2[2] + PowerCtrl_Info->Target.Omiga_2[3];                               // 计算输出总角速度平方和
    PowerCtrl_Info->Sum.Power_Sum = PowerCtrl_Info->Target.Power_In[0] + PowerCtrl_Info->Target.Power_In[1] + PowerCtrl_Info->Target.Power_In[2] + PowerCtrl_Info->Target.Power_In[3] + PowerCtrl_Info->Param.K3; // 计算实际功率
    PowerCtrl_Info->Power_Allin=PowerCtrl_Info->Sum.Power_Sum;
		VAL_LIMIT(PowerCtrl_Info->Power_Allin,0,PowerCtrl_Info->Power_Max);
		
    RLS_Power_Info.Data.X[0] = PowerCtrl_Info->Sum.Torque2_Sum;
    RLS_Power_Info.Data.X[1] = PowerCtrl_Info->Sum.Omiga2_Sum;
    RLS_Power_Info.Data.U[0] = PowerCtrl_Info->Power_Allin;
    RLS_Power_Info.Data.Y[0] =Chassis_Info.SuperCap.Chassis_Power;
		
		
    // RLS输入参数更新	
	//	RLS_Update(&RLS_Power_Info);
    PowerCtrl_Info->Param.K1 = RLS_Power_Info.Data.W[0];
    PowerCtrl_Info->Param.K2 = RLS_Power_Info.Data.W[1];
    // RLS输出参数更新

    PowerCtrl_Info->Sum.Power_Sum = PowerCtrl_Info->Target.Power_In[0] + PowerCtrl_Info->Target.Power_In[1] + PowerCtrl_Info->Target.Power_In[2] + PowerCtrl_Info->Target.Power_In[3] + PowerCtrl_Info->Param.K3; // 根据更新的参数，重新计算实际功率

    for (int i = 0; i < 4; i++)
        PowerCtrl_Info->Err[i] = Pid[i].Err[0];
    // 输入PID误差
    PowerCtrl_Info->Sum.Err_Sum = fabsf(PowerCtrl_Info->Err[0]) + fabsf(PowerCtrl_Info->Err[1]) + fabsf(PowerCtrl_Info->Err[2]) + fabsf(PowerCtrl_Info->Err[3]);
    // 计算PID误差和
    if (PowerCtrl_Info->Sum.Err_Sum > PowerCtrl_Info->Param.Err_Upper)
        PowerCtrl_Info->K = 1;
    else if (PowerCtrl_Info->Sum.Err_Sum < PowerCtrl_Info->Param.Err_Lower)
        PowerCtrl_Info->K = 0;
    else
        PowerCtrl_Info->K = (PowerCtrl_Info->Sum.Err_Sum - PowerCtrl_Info->Param.Err_Lower) / (PowerCtrl_Info->Param.Err_Upper - PowerCtrl_Info->Param.Err_Lower);
    // 计算分配因子K

    for (int i = 0; i < 4; i++)
    {
        PowerCtrl_Info->Menbership[i] = (PowerCtrl_Info->K * (fabs(PowerCtrl_Info->Err[i]) / PowerCtrl_Info->Sum.Err_Sum) + (1 - PowerCtrl_Info->K) * (fabs(PowerCtrl_Info->Target.Power_In[i]) / PowerCtrl_Info->Sum.Power_Sum)); // 港科大P分配
        PowerCtrl_Info->Power_Limit[i] = PowerCtrl_Info->Menbership[i] * PowerCtrl_Info->Power_Allin;
    }

    if (PowerCtrl_Info->Sum.Power_Sum>= PowerCtrl_Info->Power_Max)
    {
	
        for (int i = 0; i < 4; i++)
        {
            PowerCtrl_Info->A = PowerCtrl_Info->Param.K1;
            PowerCtrl_Info->B = PowerCtrl_Info->Target.Omiga[i];
            PowerCtrl_Info->C = PowerCtrl_Info->Target.Omiga_2[i] * PowerCtrl_Info->Param.K2 + PowerCtrl_Info->Param.K3 * 0.25 - PowerCtrl_Info->Power_Limit[i];
            PowerCtrl_Info->Delta = powf(PowerCtrl_Info->B, 2.f) - 4 * PowerCtrl_Info->A * PowerCtrl_Info->C;
            // 二次方程系数带入
            if (isnan(PowerCtrl_Info->Delta) || isinf(PowerCtrl_Info->Delta))
                PowerCtrl_Info->Delta = 0;

            if (PowerCtrl_Info->Delta >= 0)
            {
                PowerCtrl_Info->Sqrt = fm_sqrt_fast(PowerCtrl_Info->Delta);
                if (Pid[i].Output >= 0)
                {
                    PowerCtrl_Info->Torque[i] = (-PowerCtrl_Info->B + PowerCtrl_Info->Sqrt) / (2 * PowerCtrl_Info->A);
                    PowerCtrl_Info->Output[i]  = (PowerCtrl_Info->Torque[i] / 1.927e-5);
                }
                else
                {
                    PowerCtrl_Info->Torque[i] = (-PowerCtrl_Info->B - PowerCtrl_Info->Sqrt) / (2 * PowerCtrl_Info->A);
                   PowerCtrl_Info->Output[i]  = (PowerCtrl_Info->Torque[i] / 1.927e-5);
                }
            } // 有实数解
            else
            {
                if (Pid[i].Output >= 0)
                {
                    PowerCtrl_Info->Torque[i] = (-PowerCtrl_Info->B) / (2 * PowerCtrl_Info->A);
                    PowerCtrl_Info->Output[i]  = (PowerCtrl_Info->Torque[i] / 1.927e-5);
                }
                else
                {
                    PowerCtrl_Info->Torque[i] = (PowerCtrl_Info->B) / (2 * PowerCtrl_Info->A);
                    PowerCtrl_Info->Output[i] = (PowerCtrl_Info->Torque[i] / 1.927e-5);
                }
            } // 无实数解
        }
    }
    else
		{
        for (int i = 0; i < 4; i++)
		{
            PowerCtrl_Info->Output[i] = Pid[i].Output;
		}
	}
}
