#include "Gimbal_Task.h"

Gimbal_Typedef_Info Gimbal_Info;

static void Gimbal_Init();
static void Gimbal_On();
static void Gimbal_Off();
static void Gimbal_PidCal();

PID_Info_TypeDef Pid_Yaw_A;
PID_Info_TypeDef Pid_Yaw_V;
PID_Info_TypeDef Pid_Pitch_A;
PID_Info_TypeDef Pid_Pitch_V;
PID_Info_TypeDef Pid_AotoYaw_A;
PID_Info_TypeDef Pid_AotoYaw_V;
PID_Info_TypeDef Pid_AotoPitch_A;
PID_Info_TypeDef Pid_AotoPitch_V;
PID_Info_TypeDef Pid_DaFuPitch_A;
PID_Info_TypeDef Pid_DaFuPitch_V;
LowPassFilter1p_Info_TypeDef Velocity;


    float Vofadata[5];
    float SMC_Param[6]={0,0,0,0,0,0};
    float Gimbal_Pid_Param[10][7] =
        {
            [YAW_A] = {17.f, 0, 0.5, 0, 0, 0, 100},
            [YAW_V] = {250, 3, 1, 0, 0, 500, 23000},
            [PITCH_A] = {14.9, 0, 0.001, 0, 0, 0, 500},
            [PITCH_V] = {450.f, 4.95, 0, 0, 0, 200, 22000},
            // 15

            ////zimiaoff
            [AOTOYAW_A] = {40.f, 0, 1, 0, 0, 0, 100},
            [AOTOYAW_V] = {310, 3, 1, 0, 0, 200, 23000},
            [AOTOPITCH_A] = {25, 0, 0.001, 0, 0, 0, 500},
            [AOTOPITCH_V] = {300.f, 2, 0, 0, 0, 200, 22000},
            [DAFUPITCH_A] = {30, 0, 0.001, 0, 0, 0, 500},
            [DAFUPITCH_V] = {450.f, 3, 0, 0, 0, 300, 22000},

    };

    void Gimbal_Task(void const *argument)
    {
        /* USER CODE BEGIN Gimbal_Task */

        Gimbal_Init();
        LowPassFilter1p_Init(&Velocity, 0.1);
        /* Infinite loop */
        for (;;)
        {
            if (Comm_Info.Gimbal_Mode != Gimbal_OFF)
            {
                Gimbal_On();
                Gimbal_PidCal();
                LowPassFilter1p_Update(&Velocity, Comm_Info.Referee.Shoot_Velocity);
                Vofadata[0] = Gimbal_Info.Target.Yaw;
                Vofadata[1] = INS_Info.Yaw_Angle;
                Vofadata[2] = Gimbal_Info.Target.Pitch;
                Vofadata[3] = INS_Info.Roll_Angle;
                Vofadata[4] = Vision_Info.IF_Fire_Accept;

            }
            else if (Comm_Info.Gimbal_Mode == Gimbal_OFF)
                Gimbal_Off();

            osDelay(1);
        }
        /* USER CODE END Gimbal_Task */
    }
void Gimbal_Init()
{
    PID_Init(&Pid_Yaw_A, PID_SEPARATION, Gimbal_Pid_Param[YAW_A], NULL, NULL);
    PID_Init(&Pid_Yaw_V, PID_SEPARATION, Gimbal_Pid_Param[YAW_V], NULL, NULL);
    PID_Init(&Pid_Pitch_A, PID_POSITION, Gimbal_Pid_Param[PITCH_A], NULL, NULL);
    PID_Init(&Pid_Pitch_V, PID_POSITION, Gimbal_Pid_Param[PITCH_V], NULL, NULL);
    PID_Init(&Pid_AotoYaw_A, PID_SEPARATION, Gimbal_Pid_Param[AOTOYAW_A], NULL, NULL);
    PID_Init(&Pid_AotoYaw_V, PID_SEPARATION, Gimbal_Pid_Param[AOTOYAW_V], NULL, NULL);
    PID_Init(&Pid_AotoPitch_A, PID_POSITION, Gimbal_Pid_Param[AOTOPITCH_A], NULL, NULL);
    PID_Init(&Pid_AotoPitch_V, PID_POSITION, Gimbal_Pid_Param[AOTOPITCH_V], NULL, NULL);
    PID_Init(&Pid_DaFuPitch_A, PID_POSITION, Gimbal_Pid_Param[DAFUPITCH_A], NULL, NULL);
    PID_Init(&Pid_DaFuPitch_V, PID_POSITION, Gimbal_Pid_Param[DAFUPITCH_V], NULL, NULL);


    Gimbal_Info.Target.Yaw = INS_Info.Yaw_Angle;
    Gimbal_Info.Target.Pitch = INS_Info.Roll_Angle;
    Gimbal_Info.Pitch_Max = 20.f;
    Gimbal_Info.Pitch_Min = -15.f;
    Gimbal_Info.Forward.K_Yaw = 0.f;
    Gimbal_Info.Forward.K_Pitch = 50000.f;
    Pid_Yaw_V.Err_Lower = 10;
}

void Gimbal_On()
{

    if (Comm_Info.Gimbal_Mode == Gimbal_ON)
    {
        Gimbal_Info.Target.Yaw -= ((VT13_Info.RC.Channel[0] + remote_ctrl.rc.ch[0]) * 0.0007f + (KeyBoard_Info.mouse.x) * 0.002f);
        Gimbal_Info.Target.Pitch += (VT13_Info.RC.Channel[1] * 0.00025f + remote_ctrl.rc.ch[1] * 0.00025f + (KeyBoard_Info.mouse.y) * 0.0032f);
    }
    else if (Comm_Info.Gimbal_Mode == Gimbal_AotoAim)
    {
        if (Vision_Info.Distance > 0)
        {
            Gimbal_Info.Target.Yaw = Vision_Info.Target_Yaw;
            Gimbal_Info.Target.Pitch = Vision_Info.Target_Pitch;
            Gimbal_Info.Forward.Yaw = (Gimbal_Info.Target.Yaw - Gimbal_Info.Forward.LastYaw) * Gimbal_Info.Forward.K_Yaw;
            Gimbal_Info.Forward.Pitch = (Gimbal_Info.Target.Pitch - Gimbal_Info.Forward.LastPitch) * Gimbal_Info.Forward.K_Pitch;
            Gimbal_Info.Forward.LastYaw = Gimbal_Info.Target.Yaw;
            Gimbal_Info.Forward.LastPitch = Gimbal_Info.Target.Pitch;
        }
        else
        {
            Gimbal_Info.Target.Yaw = INS_Info.Yaw_Angle;
            Gimbal_Info.Target.Pitch = INS_Info.Roll_Angle;
            Gimbal_Info.Target.Yaw -= ((VT13_Info.RC.Channel[0] + remote_ctrl.rc.ch[0]) * 0.0007f + KeyBoard_Info.mouse.x * 0.002f);
            Gimbal_Info.Target.Pitch += (VT13_Info.RC.Channel[1] * 0.00025f + remote_ctrl.rc.ch[1] * 0.00025f + KeyBoard_Info.mouse.y * 0.0032f);
            ;
        }
    }

    if (Gimbal_Info.Target.Yaw > 180.f)
        Gimbal_Info.Target.Yaw -= 360.f;
    else if (Gimbal_Info.Target.Yaw < -180.f)
        Gimbal_Info.Target.Yaw += 360.f;
    if ((Comm_Info.Gimbal_Lost == true) || (Gimbal_Info.Last_Gimbal_Mode != Comm_Info.Gimbal_Mode))
    {
        Gimbal_Info.Target.Yaw = INS_Info.Yaw_Angle;
        Gimbal_Info.Target.Pitch = INS_Info.Roll_Angle;
        Gimbal_Info.Err = 0;
    }
    else
    {
        Gimbal_Info.Err = Gimbal_Info.Target.Yaw - INS_Info.Yaw_Angle;
        if (Gimbal_Info.Err > 180.f)
            Gimbal_Info.Err -= 360.f;
        else if (Gimbal_Info.Err < -180.f)
            Gimbal_Info.Err += 360.f;
    }
    VAL_LIMIT(Gimbal_Info.Target.Pitch, Gimbal_Info.Pitch_Min, Gimbal_Info.Pitch_Max);

    Gimbal_Info.Pitch_Max = 30.f;
    Gimbal_Info.Pitch_Min = -15.f;
}
void Gimbal_PidCal()
{
    if (Comm_Info.Gimbal_Mode == Gimbal_ON)
    {
        PID_Calculate(&Pid_Yaw_A, Gimbal_Info.Err, 0);
        PID_Calculate(&Pid_Yaw_V, Pid_Yaw_A.Output, INS_Info.Yaw_Gyro);
        Gimbal_Info.Output.Yaw = Pid_Yaw_V.Output;
       
        PID_Calculate(&Pid_Pitch_A, Gimbal_Info.Target.Pitch, INS_Info.Roll_Angle);
        PID_Calculate(&Pid_Pitch_V, Pid_Pitch_A.Output, Gimbal_Motor[GIMBAL_MOTOR_PITCH].Data.velocity);
        Gimbal_Info.Output.Pitch = Pid_Pitch_V.Output;
    }//使能云台
    else if (Comm_Info.Gimbal_Mode == Gimbal_AotoAim)
    {
        PID_Calculate(&Pid_AotoYaw_A, Gimbal_Info.Err, 0);
        PID_Calculate(&Pid_AotoYaw_V, Pid_AotoYaw_A.Output, INS_Info.Yaw_Gyro);
        Gimbal_Info.Output.Yaw = Pid_AotoYaw_V.Output + Gimbal_Info.Forward.Yaw;
        if (Vision_Info.Mode == RED || Vision_Info.Mode == BULE)
        {
            PID_Calculate(&Pid_AotoPitch_A, Gimbal_Info.Target.Pitch, INS_Info.Roll_Angle);
            PID_Calculate(&Pid_AotoPitch_V, Pid_AotoPitch_A.Output, Gimbal_Motor[GIMBAL_MOTOR_PITCH].Data.velocity);
        }
        else
        {
            PID_Calculate(&Pid_DaFuPitch_A, Gimbal_Info.Target.Pitch, INS_Info.Roll_Angle);
            PID_Calculate(&Pid_DaFuPitch_V, Pid_DaFuPitch_A.Output, Gimbal_Motor[GIMBAL_MOTOR_PITCH].Data.velocity);
        }
        Gimbal_Info.Output.Pitch = Pid_AotoPitch_V.Output + Pid_DaFuPitch_V.Output ;
    }//自瞄模式
    else
        Comm_Info.Gimbal_Mode = Gimbal_OFF;
}
void Gimbal_Off()
{
    Gimbal_Info.Output.Yaw = 0;
    Gimbal_Info.Output.Pitch = 0;
}
