#include "Shoot_Task.h"

static void Shoot_Init();
static void Shoot_On();
static void Fire_Ctrl();
static void Shoot_Off();

Shoot_Typedef_Info Shoot_Info;
PID_Info_TypeDef Pid_Shoot_L;
PID_Info_TypeDef Pid_Shoot_R;
PID_Info_TypeDef Pid_Shoot_FV;
PID_Info_TypeDef Pid_Shoot_FA;
const float BulletHeat17 = 10;
float Shoot_Data[6];
float Shoot_Pid_Param[4][7] =
    {
        [SHOOT_L] = {40, 0.7, 0, 0, 0, 2000, 1000},
        [SHOOT_R] = {40, 0.7, 0, 0, 0, 2000, 1000},
        [SHOOT_FV] = {150, 0.5, 0, 0, 0, 200, 9900},
        [SHOOT_FA] = {70, 0, 10, 0, 0, 2000, 1000},

};
TickType_t Shoot_SysTick = 0;
void Shoot_Task(void const *argument)
{
    /* USER CODE BEGIN Shoot_Task */
    Shoot_Init();
    /* Infinite loop */
    for (;;)
    {
        Shoot_SysTick = osKernelSysTick();
        if (Comm_Info.Shoot_Mode != Shoot_OFF)
        {
					


            Shoot_On();            // 摩擦轮以及拨盘控制
					  Fire_Ctrl();           // 热量控制
        }
        else if (Comm_Info.Shoot_Mode == Shoot_OFF)
            Shoot_Off();

        osDelay(1);
        /* USER CODE END Shoot_Task */
    }
}

void Shoot_Init()
{
    Shoot_Info.Target.BulletFeed =10;
    PID_Init(&Pid_Shoot_L, PID_VELOCITY, Shoot_Pid_Param[SHOOT_L]);
    PID_Init(&Pid_Shoot_R, PID_VELOCITY, Shoot_Pid_Param[SHOOT_R]);
    PID_Init(&Pid_Shoot_FV, PID_POSITION, Shoot_Pid_Param[SHOOT_FV]);
    PID_Init(&Pid_Shoot_FA, PID_POSITION, Shoot_Pid_Param[SHOOT_FA]);
    Shoot_Info.Fire_Flag = false;
    Shoot_Info.Flag_Last = Shoot_Info.Fire_Flag;
}

static void Fire_Ctrl()
{

    Shoot_Info.HeatCtrl.Heat_Surplus =  Referee_System_Info.robot_status.shooter_barrel_heat_limit-Referee_System_Info.power_heat_data.shooter_17mm_barrel_heat;

if(Shoot_Info.HeatCtrl.Heat_Surplus>80)
	Shoot_Info.Target.BulletFeed=20;
	else
    if (Shoot_Info.HeatCtrl.ShootCount == 0)
    {
        Shoot_Info.HeatCtrl.ShootTime = (Shoot_Info.HeatCtrl.Heat_Surplus + 2 * Referee_System_Info.robot_status.shooter_barrel_cooling_value) * 10;
        VAL_LIMIT(Shoot_Info.HeatCtrl.ShootTime, 100, 5600);
        if (Shoot_Info.HeatCtrl.Heat_Surplus < 50)
        {

            Shoot_Info.HeatCtrl.Shoot_Speed = (BulletHeat17 * Shoot_Info.HeatCtrl.Heat_Surplus - Referee_System_Info.robot_status.shooter_barrel_cooling_value - 3 * BulletHeat17) / (BulletHeat17 * (Shoot_Info.HeatCtrl.ShootTime / 100.0f)) + Comm_Info.Referee.barrel_cooling_value / BulletHeat17;
         }
         else
         {
             Shoot_Info.HeatCtrl.Shoot_Speed = (BulletHeat17 * Shoot_Info.HeatCtrl.Heat_Surplus - Referee_System_Info.robot_status.shooter_barrel_cooling_value - 7 * BulletHeat17) / (BulletHeat17 * (Shoot_Info.HeatCtrl.ShootTime / 100.0f)) + Comm_Info.Referee.barrel_cooling_value / BulletHeat17;
         }
     }
     else if (0 < Shoot_Info.HeatCtrl.ShootCount && Shoot_Info.HeatCtrl.ShootCount < Shoot_Info.HeatCtrl.ShootTime)
     {
         Shoot_Info.Target.BulletFeed = (Shoot_Info.HeatCtrl.Shoot_Speed);
         VAL_LIMIT(Shoot_Info.Target.BulletFeed, 0.0f, 20.0f);
     }
   else
     {
         Shoot_Info.Target.BulletFeed = ((Referee_System_Info.robot_status.shooter_barrel_cooling_value / BulletHeat17));
         if (Shoot_Info.Target.BulletFeed < 1.f)
             Shoot_Info.Target.BulletFeed = 0;
         VAL_LIMIT(Shoot_Info.Target.BulletFeed, 0.0f, 20.0f);
     }
     if (Shoot_Info.HeatCtrl.ShootCount < Shoot_Info.HeatCtrl.ShootTime)
     {
         Shoot_Info.HeatCtrl.ShootCount++;
     }
     Shoot_Info.HeatCtrl.Last_Shoot_time = Shoot_Info.HeatCtrl.ShootTime;
     if (Shoot_Info.HeatCtrl.Heat_Surplus >= 50)
     {
         if (Shoot_Info.HeatCtrl.ShootCount >= Shoot_Info.HeatCtrl.ShootTime)
         {
             Shoot_Info.HeatCtrl.ShootCount = 0;
         }
     }
     else if (Shoot_Info.HeatCtrl.Heat_Surplus <= 30)
     {
         Shoot_Info.HeatCtrl.ShootCount = Shoot_Info.HeatCtrl.Last_Shoot_time;
     }
}
static void Shoot_On()
{

if(Comm_Info.Shoot_Mode==Shoot_Repeat)
{
        if ((Chassis_Motor[4].Data.current <= -9600) && !Shoot_Info.Reload)
        {
            Shoot_Info.Stuck_Times++;
            if (Shoot_Info.Stuck_Times > 100)
            {
                Shoot_Info.Reload = 1;
                Shoot_Info.Stuck_Times = 0;
            }
        } // 卡弹检测
        if (Shoot_Info.Reload == 1)
        {
            Shoot_Info.Target.BulletFeed = -10;
            Shoot_Info.Return_Times++;
            if ((Chassis_Motor[4].Data.current > 4000) || Shoot_Info.Return_Times >= 100)
            {
                Shoot_Info.Return_Times = 0;
                Shoot_Info.Reload = 0;
            }
        } // 反转
        PID_Calculate(&Pid_Shoot_FV, Shoot_Info.Target.BulletFeed * 540.f, Chassis_Motor[4].Data.velocity);

        // 更新双环期望角度，防止连发切单发会持续反转
        if (Comm_Info.Gimbal_Mode == Gimbal_AotoAim)
        {
            if ((Shoot_Info.Fire_Flag == true) && (Shoot_Info.IF_Fire_Accept == true))
                Shoot_Info.Output.BulletFeed = Pid_Shoot_FV.Output;
            else
            {
                Shoot_Info.Return_Times = 0;
                Shoot_Info.Output.BulletFeed = 0;
                Shoot_Info.Stuck_Times = 0;
            }
        }
        else
        {
            if (Shoot_Info.Fire_Flag == true)
                Shoot_Info.Output.BulletFeed = Pid_Shoot_FV.Output;
            else
            {
                Shoot_Info.Return_Times = 0;
                Shoot_Info.Output.BulletFeed = 0;
                Shoot_Info.Stuck_Times = 0;
            }
        }
        // 根据不同云台模式来采取不同火控策略
}

else
{
        Shoot_Info.Output.BulletFeed = 0;
}
}
static void Shoot_Off()
{
    Shoot_Info.Output.BulletFeed = 0;
    Shoot_Info.Fire_Flag = false;
}
