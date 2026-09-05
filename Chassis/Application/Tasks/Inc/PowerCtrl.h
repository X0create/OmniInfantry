#ifndef POWER_CTRL
#define POWER_CTRL

#include "rls.h"
#include "motor.h"
#include "math.h"
#include "pid.h"

#define POWERCTRL_TYPE_NUM 4

typedef enum
{
    MACUNUM,
    STEER,
    CHASSIS_TYPE_NUM,
} Chassis_Tpye_e;

typedef struct
{
    float K1;
    float K2;
    float K3;
    float Err_Lower;
    float Err_Upper;
} PowerCtrl_Parameter_Typedef;

typedef struct
{

    // 经验公式系数

    float A;
    float B;
    float C;
    // 最小二乘法系数
    float Delta;
    float Sqrt;
    // 一元二次方程中间变量

    float K;
    int16_t Power_Max;      // 最大能量
    float Power_Allin;    // 期望输出总功率
    float Power_Limit[4]; // 输出功率限制
    float Menbership[4];  // 隶属度
    float Torque[4];      // 输出力矩

    float Err[4]; // 期望输出电流于电机实际电流
    struct
    {
        float Torque2_Sum;
        float Omiga2_Sum;
        float Power_Sum;
        float Err_Sum;
    } Sum; // 一些数据求和
    struct
    {
        float Power_In[4];
        float Torque[4];
        float Omiga[4];
        float Torque_2[4];
        float Omiga_2[4];
    } Target; // 模型输入的参数
    int16_t Output[4];
    PowerCtrl_Parameter_Typedef Param;
    Chassis_Tpye_e Chassis_Type;
} PowerCtrl_Typedef;

extern void PowerCtrl_Init(PowerCtrl_Typedef *PowerCtrl_Info, Chassis_Tpye_e Type, float Lamda, float P, float PowerCtrl_Param[POWERCTRL_TYPE_NUM]);
extern void PowerCtrl(PowerCtrl_Typedef *PowerCtrl_Info, PID_Info_TypeDef Pid[4], DJI_Motor_Info_Typedef Chassis_Motor[4]);
extern void PowerCtrl_Steer(PowerCtrl_Typedef *PowerCtrl_Info, PID_Info_TypeDef *Pid_Course, PID_Info_TypeDef *Pid_Travel, DJI_Motor_Info_Typedef *Motor_Course, DJI_Motor_Info_Typedef *Motor_Travel);

#endif


