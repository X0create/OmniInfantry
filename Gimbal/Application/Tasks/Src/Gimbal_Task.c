#include "Gimbal_Task.h"
#include "config.h"    /* 云台与发射控制参数 */

Gimbal_Typedef_Info Gimbal_Info;
SweepFrequency_Info_Typedef SF_Info;
RLS_Info_TypeDef RLS_Gimbal;

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


float yaw;
float pitch;
float roll;
float x;
float y;
float z;
TickType_t Gimbal_Systick = 0;

float Vofadata[5];
float SMC_Param[6] = {0, 0, 0, 0, 0, 0};
float Gimbal_Pid_Param[10][7] =
    {
        [YAW_A] = {0.42, 0, 0, 0, 0, 0, 7},
        [YAW_V] = {1.0, 0.1, 0, 0, 0, 1.5, 7},
        [PITCH_A] = {0.3, 0, 0, 0, 0, 0, 4},
        [PITCH_V] = {0.3, 0.05, 0, 0, 0, 0.5, 9},
        [AOTOYAW_A] = {0.42, 0, 0, 0, 0, 0, 7},
        [AOTOYAW_V] = {1.5, 0.1, 0, 0, 0, 1.5, 9},
        [AOTOPITCH_A] = {0.3, 0, 0, 0, 0, 0, 4},
        [AOTOPITCH_V] = {0.4, 0.05, 0, 0, 0, 0.5, 9},

};

void Gimbal_Task(void const *argument)
{
    /* USER CODE BEGIN Gimbal_Task */

    Gimbal_Init();
    /* Infinite loop */
    for (;;)
    {
        Gimbal_Systick = osKernelSysTick();
        if (Comm_Info.Gimbal_Mode != Gimbal_OFF)
        {
            Gimbal_On();
            Gimbal_PidCal();
//			Vofadata[0]=INS_Info.Yaw_Angle;
//			Vofadata[1]=Gimbal_Info.Target.Yaw;
//			Vofadata[2]=INS_Info.Pitch_Angle;
//			Vofadata[3]=Gimbal_Info.Target.Pitch;
//			Vofadata[4]=Vision_Info.IF_Fire_Accept;
//			Vofa_JustFloat(Vofadata,5);
        }
        else if (Comm_Info.Gimbal_Mode == Gimbal_OFF)
            Gimbal_Off();


        osDelay(1);
    }
    /* USER CODE END Gimbal_Task */
}

void Gimbal_Init()
{
    PID_Init(&Pid_Yaw_A, PID_POSITION, Gimbal_Pid_Param[YAW_A]);
    PID_Init(&Pid_Yaw_V, PID_POSITION, Gimbal_Pid_Param[YAW_V]);
    PID_Init(&Pid_Pitch_A, PID_POSITION, Gimbal_Pid_Param[PITCH_A]);
    PID_Init(&Pid_Pitch_V, PID_POSITION, Gimbal_Pid_Param[PITCH_V]);
    PID_Init(&Pid_AotoYaw_A, PID_POSITION, Gimbal_Pid_Param[AOTOYAW_A]);
    PID_Init(&Pid_AotoYaw_V, PID_POSITION, Gimbal_Pid_Param[AOTOYAW_V]);
    PID_Init(&Pid_AotoPitch_A, PID_POSITION, Gimbal_Pid_Param[AOTOPITCH_A]);
    PID_Init(&Pid_AotoPitch_V, PID_POSITION, Gimbal_Pid_Param[AOTOPITCH_V]);
    Gimbal_Info.Target.Yaw = INS_Info.Yaw_Angle;
    Gimbal_Info.Target.Pitch = INS_Info.Pitch_Angle;
	  Gimbal_Info.Forward.K_Pitch=GIMBAL_FORWARD_K_PITCH;
	  Gimbal_Info.Forward.K_Yaw=GIMBAL_FORWARD_K_YAW;
    Gimbal_Info.Pitch_Max = GIMBAL_PITCH_MAX;
    Gimbal_Info.Pitch_Min = GIMBAL_PITCH_MIN;
}

void Gimbal_On()
{
    if (Comm_Info.Gimbal_Mode == Gimbal_ON)
    {
			if(Comm_Info.Control_Mode==Remote_Control)
			{
        Gimbal_Info.Target.Yaw -= (VT13_Info.RC.Channel[0] + remote_ctrl.rc.ch[0]) * GIMBAL_RC_YAW_SENSITIVITY ;
        Gimbal_Info.Target.Pitch -= (VT13_Info.RC.Channel[1] * GIMBAL_RC_PITCH_SENSITIVITY + remote_ctrl.rc.ch[1] * GIMBAL_RC_PITCH_SENSITIVITY );
			}
			else{
			Gimbal_Info.Target.Yaw -=  (KeyBoard_Info.mouse.x) * GIMBAL_MOUSE_YAW_SENSITIVITY;
				Gimbal_Info.Target.Pitch -= (KeyBoard_Info.mouse.y) * GIMBAL_MOUSE_PITCH_SENSITIVITY;
			}
    }
    else if (Comm_Info.Gimbal_Mode == Gimbal_AutoAim)
    {
        if (Vision_Info.Distance > 0)
        {
            Gimbal_Info.Target.Yaw = Vision_Info.Target_Yaw;
            Gimbal_Info.Target.Pitch = -Vision_Info.Target_Pitch;
        }
        else
        {
            Gimbal_Info.Target.Yaw = INS_Info.Yaw_Angle;
            Gimbal_Info.Target.Pitch = INS_Info.Pitch_Angle;
            Gimbal_Info.Target.Yaw -= ((VT13_Info.RC.Channel[0] + remote_ctrl.rc.ch[0]) * GIMBAL_RC_YAW_SENSITIVITY + KeyBoard_Info.mouse.x * GIMBAL_MOUSE_YAW_SENSITIVITY);
            Gimbal_Info.Target.Pitch-= (VT13_Info.RC.Channel[1] * GIMBAL_RC_PITCH_SENSITIVITY + remote_ctrl.rc.ch[1] * GIMBAL_RC_PITCH_SENSITIVITY + KeyBoard_Info.mouse.y * GIMBAL_MOUSE_PITCH_SENSITIVITY);
        }
    }
	  if (Gimbal_Info.Target.Yaw > 180.f)
       Gimbal_Info.Target.Yaw -= 360.f;
    else if (Gimbal_Info.Target.Yaw < -180.f)
       Gimbal_Info.Target.Yaw += 360.f;

		VAL_LIMIT(Gimbal_Info.Target.Pitch, Gimbal_Info.Pitch_Min, Gimbal_Info.Pitch_Max);


    if ((Comm_Info.Gimbal_Lost == true) || (Comm_Info.Last_Gimbal_Mode != Comm_Info.Gimbal_Mode))
    {
        Gimbal_Info.Target.Yaw = INS_Info.Yaw_Angle;
        Gimbal_Info.Target.Pitch = INS_Info.Pitch_Angle;
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

}
void Gimbal_PidCal()
{

    if (Comm_Info.Gimbal_Mode == Gimbal_ON)
    {
        PID_Calculate(&Pid_Yaw_A, Gimbal_Info.Err, 0);
				Gimbal_Info.Forward.Yaw_V = (Pid_Yaw_A.Output - Gimbal_Info.Forward.LastYaw) * Gimbal_Info.Forward.K_Yaw;
        PID_Calculate(&Pid_Yaw_V, Pid_Yaw_A.Output+Gimbal_Info.Forward.Yaw_V, INS_Info.Yaw_Gyro / RadiansToDegrees);
        Gimbal_Info.Output.Yaw = Pid_Yaw_V.Output;
				Gimbal_Info.Forward.LastYaw = Pid_Yaw_A.Output;

        PID_Calculate(&Pid_Pitch_A, Gimbal_Info.Target.Pitch, INS_Info.Pitch_Angle);
				Gimbal_Info.Forward.Pitch_V = (Pid_Pitch_A.Output - Gimbal_Info.Forward.LastPitch) * Gimbal_Info.Forward.K_Pitch;
        PID_Calculate(&Pid_Pitch_V, Pid_Pitch_A.Output+Gimbal_Info.Forward.Pitch_V, Gimbal_Motor[Pitch].Data.Velocity);
        Gimbal_Info.Output.Pitch = Pid_Pitch_V.Output;
        Gimbal_Info.Forward.LastPitch = Pid_Pitch_A.Output;
    } // 浣胯兘浜戝彴
    else if (Comm_Info.Gimbal_Mode == Gimbal_AutoAim)
    {
        PID_Calculate(&Pid_AotoYaw_A, Gimbal_Info.Err, 0);
				Gimbal_Info.Forward.Yaw_V = (Pid_AotoYaw_A.Output-Gimbal_Info.Forward.LastYaw)* Gimbal_Info.Forward.K_Yaw;
        PID_Calculate(&Pid_AotoYaw_V, Pid_AotoYaw_A.Output+Gimbal_Info.Forward.Yaw_V, INS_Info.Yaw_Gyro / RadiansToDegrees);
        Gimbal_Info.Output.Yaw = Pid_AotoYaw_V.Output;
			  Gimbal_Info.Forward.LastYaw = Pid_AotoYaw_A.Output;

        PID_Calculate(&Pid_AotoPitch_A, Gimbal_Info.Target.Pitch, INS_Info.Pitch_Angle);
				Gimbal_Info.Forward.Pitch_V = (Pid_AotoPitch_A.Output-Gimbal_Info.Forward.LastPitch )* Gimbal_Info.Forward.K_Pitch;
        PID_Calculate(&Pid_AotoPitch_V, Pid_AotoPitch_A.Output+Gimbal_Info.Forward.Pitch_V, Gimbal_Motor[Pitch].Data.Velocity);
        Gimbal_Info.Output.Pitch = Pid_AotoPitch_V.Output;
			  Gimbal_Info.Forward.LastPitch = Pid_AotoPitch_A.Output;
    } // 鑷瀯妯″紡
    DM_Motor_CAN_TxMessage(&CAN1_TxFrame, &Gimbal_Motor[Pitch], 0, 0, 0, 0, Gimbal_Info.Output.Pitch);
		osDelay(1);
    DM_Motor_CAN_TxMessage(&CAN2_TxFrame, &Gimbal_Motor[Yaw], 0, 0, 0, 0, Gimbal_Info.Output.Yaw);

}
void Gimbal_Off()
{
    Gimbal_Info.Output.Yaw = 0;
    Gimbal_Info.Output.Pitch = 0;
}