#include "Shoot_Task.h"

static void Shoot_Init();
static void Shoot_On();
static float SpeedAdapt(float real_S, float min_S, float max_S, float up_num, float down_num);
static void ShootSpeed_Ctrl();
static void Shoot_Off();

Shoot_Typedef_Info Shoot_Info;

PID_Info_TypeDef Pid_Shoot_L;
PID_Info_TypeDef Pid_Shoot_R;
PID_Info_TypeDef Pid_Shoot_FV;
PID_Info_TypeDef Pid_Shoot_FA;
float suspicion_duration;


const float BulletHeat17 = 10;
float Shoot_Data[4];
float Shoot_Pid_Param[4][7] =
    {
        [SHOOT_L] = {50, 0.6, 5, 0, 0, 1500, 10000},
        [SHOOT_R] = {50, 1, 0, 0, 0, 1500, 10000},
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
            if (Shoot_SysTick % 100 == 0)
            ShootSpeed_Ctrl(); // 射速控制降频至10Hz与裁判系统一致
            Shoot_On();            // 摩擦轮以及拨盘控制
            Shoot_Data[0] = Shoot_Motor[Shoot_L].Data.current;
            Shoot_Data[1] = Shoot_Motor[Shoot_R].Data.current;
           
        }
        else if (Comm_Info.Shoot_Mode == Shoot_OFF)
            Shoot_Off();
				 Vofa_JustFloat(Shoot_Data, 4);
        osDelay(1);
        /* USER CODE END Shoot_Task */
    }
}

void Shoot_Init()
{
    Shoot_Info.Target.BulletFeed = 18;
    Shoot_Info.Target.Shoot = 6550;
    PID_Init(&Pid_Shoot_L, PID_VELOCITY, Shoot_Pid_Param[SHOOT_L]);
    PID_Init(&Pid_Shoot_R, PID_VELOCITY, Shoot_Pid_Param[SHOOT_R]);
    PID_Init(&Pid_Shoot_FV, PID_POSITION, Shoot_Pid_Param[SHOOT_FV]);
    PID_Init(&Pid_Shoot_FA, PID_POSITION, Shoot_Pid_Param[SHOOT_FA]);
    Shoot_Info.Fire_Flag = true;
    Shoot_Info.Flag_Last = Shoot_Info.Fire_Flag;
}

static float SpeedAdapt(float real_S, float min_S, float max_S, float up_num, float down_num)
{
    float res = 0;
    static uint8_t SpeedErr_cnt = 0;

    if (real_S < min_S && real_S > 8)

        SpeedErr_cnt++;

    else if (real_S >= min_S && real_S <= max_S)

        SpeedErr_cnt = 0;

    if (SpeedErr_cnt == 1)
    {
        SpeedErr_cnt = 0;
        res += up_num;
    }
    if (real_S > max_S)
        res -= down_num;

    return res;
}
static void ShootSpeed_Ctrl()
{
    if (Comm_Info.Referee.Shoot_Velocity != Shoot_Info.Last_Firespeed)
    {
        Shoot_Info.Fire_Speed_Offset += SpeedAdapt(Comm_Info.Referee.Shoot_Velocity, 22.5f, 23.5f, 9.f, 23.f);
        Shoot_Info.Target.Shoot += Shoot_Info.Fire_Speed_Offset;
    }
    Shoot_Info.Last_Firespeed = Comm_Info.Referee.Shoot_Velocity;
}





												
static void Shoot_On()
{
    VAL_LIMIT(Shoot_Info.Target.Shoot, 6550, 6700);
    Shoot_Info.Output.ShootL = PID_Calculate(&Pid_Shoot_L,+Shoot_Info.Target.Shoot, Shoot_Motor[Shoot_L].Data.velocity);
    Shoot_Info.Output.ShootR = PID_Calculate(&Pid_Shoot_R,-Shoot_Info.Target.Shoot, Shoot_Motor[Shoot_R].Data.velocity);

}
static void Shoot_Off()
{
    Shoot_Info.Output.BulletFeed = 0;
    Shoot_Info.Fire_Flag = false;
    Comm_Info.Referee.Referee_Update_Flag = false;

    if (abs(Shoot_Motor[Shoot_L].Data.velocity) > 1000)
        Shoot_Info.Output.ShootL = PID_Calculate(&Pid_Shoot_L, 0, Shoot_Motor[Shoot_L].Data.velocity);
    else
        Shoot_Info.Output.ShootL = 0;
    if (abs(Shoot_Motor[Shoot_R].Data.velocity) > 1000)
        Shoot_Info.Output.ShootR = PID_Calculate(&Pid_Shoot_R, 0, Shoot_Motor[Shoot_R].Data.velocity);
    else
        Shoot_Info.Output.ShootR = 0;
}
