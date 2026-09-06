#include "Chassis_Task.h"
#include "config.h"    /* 底盘与发射控制参数 */

static void Chassis_Init(void);
static void Chassis_Target(void);
static void Chassis_Update(void);
static void Chassis_PIDCal(void);
static void State_Judge(void);
static void Chassis_Off(void);


int32_t Counter;
Chassis_Info_Typedef Chassis_Info;
PID_Info_TypeDef PID_Chassis[CHASSIS_NUM];
PID_Info_TypeDef PID_Follow[2];
PID_Info_TypeDef Pid_PowerBuff;
PowerCtrl_Typedef Chassis_PowerCtrl;
float ChassisPID_Param[7] = {19, 0.3, 0, 0, 0,1000, 13000};
float FollowPID_Param[2][7] = {
    {5, 0, 0, 0, 0, 0, 100},
    {15, 0.03, 0, 0, 0, 1000, 8000},
};
float PowerCtrl_Param[5]={1.86021054, 8.58910716e-05, 3, 0.1, 5000};
float Pid_PowerBuff_Param[7] = {5, 0, 0, 0, 0, 0, 30};
float VofaData[3];

void Chassis_Task(void const *argument)
{
  /* USER CODE BEGIN Chassis_Task */
  Chassis_Init();
	
  /* Infinite loop */ 
  for (;;) 
  {
	State_Judge();
    if (Comm_Info.Chassis_Mode != Chassis_OFF)
    {
      Chassis_Target();
      Chassis_PIDCal();

			PowerCtrl(&Chassis_PowerCtrl,PID_Chassis,Chassis_Motor);
			
			VofaData[0]=Chassis_PowerCtrl.Power_Allin;
			VofaData[1]=Chassis_Info.SuperCap.Chassis_Power;
    }
    else
      Chassis_Off();
   Vofa_JustFloat(VofaData,3);
    osDelay(1);
  }
  /* USER CODE END Chassis_Task */
}
void Chassis_Init(void)
{
  for (int i = 0; i < 4; i++)
  {
    PID_Init(&PID_Chassis[i], PID_POSITION, ChassisPID_Param);
  }
	PowerCtrl_Init(&Chassis_PowerCtrl,MACUNUM,CHASSIS_RLS_LAMBDA,CHASSIS_RLS_P_INIT,PowerCtrl_Param);
  PID_Init(&PID_Follow[0], PID_POSITION, FollowPID_Param[0]);
  PID_Init(&PID_Follow[1], PID_POSITION, FollowPID_Param[1]);
	PID_Init(&Pid_PowerBuff,PID_POSITION,Pid_PowerBuff_Param);
  Chassis_Info.Transform.MidAngle = CHASSIS_FOLLOW_MID_ANGLE;
	Chassis_Info.Feedback.resistance[0]=CHASSIS_FRICTION_COMP_LF;
	Chassis_Info.Feedback.resistance[1]=CHASSIS_FRICTION_COMP_LB;
	Chassis_Info.Feedback.resistance[2]=CHASSIS_FRICTION_COMP_RB;
	Chassis_Info.Feedback.resistance[3]=CHASSIS_FRICTION_COMP_RF;
	
	

}
void Chassis_Target()
{
  Chassis_Info.Row.Vx = Comm_Info.Move.Vx * CHASSIS_RC_TO_RPM_SCALE;
  Chassis_Info.Row.Vy = Comm_Info.Move.Vy * CHASSIS_RC_TO_RPM_SCALE;

  Chassis_Info.Target.Follow_Err = Chassis_Info.Transform.MidAngle - Gimbal_Motor.Data.Angle+CHASSIS_MOUNT_ANGLE_OFFSET;
  if (Chassis_Info.Target.Follow_Err > 180.f)
    Chassis_Info.Target.Follow_Err -= 360.f;
  else if (Chassis_Info.Target.Follow_Err < -180.f)
    Chassis_Info.Target.Follow_Err += 360.f;
  Chassis_Info.Transform.Angle = Chassis_Info.Target.Follow_Err-CHASSIS_MOUNT_ANGLE_OFFSET;
  Chassis_Info.Transform.COS = arm_cos_f32(Chassis_Info.Transform.Angle * DegreesToRadians);
  Chassis_Info.Transform.SIN = arm_sin_f32(Chassis_Info.Transform.Angle * DegreesToRadians);
  Chassis_Info.Target.Vy = -(Chassis_Info.Row.Vx * Chassis_Info.Transform.COS - Chassis_Info.Row.Vy * Chassis_Info.Transform.SIN);
  Chassis_Info.Target.Vx = -(Chassis_Info.Row.Vx * Chassis_Info.Transform.SIN + Chassis_Info.Row.Vy * Chassis_Info.Transform.COS);
  
  

  switch (Comm_Info.Chassis_Mode)
  {
  case Chassis_Follow:
  if (Chassis_Info.Target.Follow_Err > 90.f)
    Chassis_Info.Target.Follow_Err -= 180.f;
  else if (Chassis_Info.Target.Follow_Err < -90.f)
    Chassis_Info.Target.Follow_Err += 180.f;
	
    PID_Calculate(&PID_Follow[0], 0, Chassis_Info.Target.Follow_Err);
    PID_Calculate(&PID_Follow[1], PID_Follow[0].Output, Gimbal_Motor.Data.Velocity);
    Chassis_Info.Target.Vw = PID_Follow[1].Output;
    break;
  case Chassis_Spin:
  if(Chassis_Info.SuperCap.SuperCap_Flag!=true)
    Chassis_Info.Target.Vw = CHASSIS_SPIN_RPM;
	else
		Chassis_Info.Target.Vw = CHASSIS_SPIN_RPM_SUPERCAP;
    break;

  case Chassis_OFF:
    Chassis_Info.Target.Vw = 0;
    Chassis_Info.Target.Vx = 0;
    Chassis_Info.Target.Vy = 0;
    break;
  }

  Chassis_Info.Target.Wheelrpm[0] = -Chassis_Info.Target.Vx - Chassis_Info.Target.Vy - Chassis_Info.Target.Vw;
  Chassis_Info.Target.Wheelrpm[1] = Chassis_Info.Target.Vx - Chassis_Info.Target.Vy - Chassis_Info.Target.Vw;
  Chassis_Info.Target.Wheelrpm[2] = Chassis_Info.Target.Vx + Chassis_Info.Target.Vy - Chassis_Info.Target.Vw;
  Chassis_Info.Target.Wheelrpm[3] = -Chassis_Info.Target.Vx + Chassis_Info.Target.Vy - Chassis_Info.Target.Vw;
}

void Chassis_PIDCal(void)
{
  for (int i = 0; i < 4; i++)
  {
    PID_Calculate(&PID_Chassis[i], Chassis_Info.Target.Wheelrpm[i], Chassis_Motor[i].Data.velocity);
    	Chassis_Info.Output[i] = PID_Chassis[i].Output+Chassis_Info.Feedback.resistance[i];
  }
}

void State_Judge()
{

	 if ((Referee_System_Info.robot_status.mains_power_gimbal_output == 0) || (Referee_System_Info.robot_status.current_HP == 0))
    {
        Comm_Info.Gimbal_Lost = true;
    }
    else
    {
        Comm_Info.Gimbal_Lost = false;
    }

    if ((Referee_System_Info.robot_status.mains_power_chassis_output == 0) || (Referee_System_Info.robot_status.current_HP == 0))
    {
        Comm_Info.Chassis_Lost = true;
    }
    else
    {
        Comm_Info.Chassis_Lost = false;
    }

    if ((Referee_System_Info.robot_status.mains_power_gimbal_output == 0) || Referee_System_Info.robot_status.current_HP == 0)
        Comm_Info.Gimbal_Lost = true;
    else
        Comm_Info.Gimbal_Lost = false;
		    
    if (Chassis_Info.SuperCap.SuperCap_Flag == true)
    {
      Chassis_PowerCtrl.Power_Max = CHASSIS_POWER_MAX_SUPERCAP;

    }
    else
    {
        PID_Calculate(&Pid_PowerBuff, CHASSIS_POWER_BUFFER_TARGET, Referee_System_Info.power_heat_data.buffer_energy);
        Chassis_PowerCtrl.Power_Max = (Referee_System_Info.robot_status.chassis_power_limit - Pid_PowerBuff.Output);
    }
   
}
void Chassis_Off()
{
	for (int i = 0; i < 4; i++)
		Chassis_PowerCtrl.Output[i]=0;
}


//void Get_3508_data(DJI_Motor_Info_Typedef motor[4])
//{
//  Counter++;
//  if (Counter < 2000)
//  {
//    Chassis_Info.Target.Wheelrpm[0] = -200.0f;
//  }
//  else if (Counter < 4000)
//  {
//    Chassis_Info.Target.Wheelrpm[0] = -100.0f;
//  }
//  else if (Counter < 6000)
//  {
//    Chassis_Info.Target.Wheelrpm[0] = -50.0f;
//  }
//  else if (Counter < 8000)
//  {
//    Chassis_Info.Target.Wheelrpm[0] = -20.0f;
//  }
//  else if (Counter < 10000)
//  {
//    Chassis_Info.Target.Wheelrpm[0] = -10.0f;
//  }
//  else if (Counter < 12000)
//  {
//    Chassis_Info.Target.Wheelrpm[0] = 0.0f;
//  }
//  else if (Counter < 14000)
//  {
//    Chassis_Info.Target.Wheelrpm[0] = 10.0f;
//  }
//  else if (Counter < 16000)
//  {
//    Chassis_Info.Target.Wheelrpm[0] = 20.0f;
//  }
//  else if (Counter < 18000)
//  {
//    Chassis_Info.Target.Wheelrpm[0] = 50.0f;
//  }
//  else if (Counter < 20000)
//  {
//    Chassis_Info.Target.Wheelrpm[0] = 100.0f;
//  }
//  else if (Counter < 22000)
//  {
//    Chassis_Info.Target.Wheelrpm[0] = 200.0f;
//  }
//  else if (Counter < 26000)
//  {
//    Chassis_Info.Target.Wheelrpm[0] = (1.0f * sinf(10.0f * PI * (Counter - 22000) / 1000.0f));
//  }
//  else if (Counter < 30000)
//  {
//    Chassis_Info.Target.Wheelrpm[0] = (2.0f * sinf(10.0f * PI * (Counter - 22000) / 1000.0f));
//  }
//  else if (Counter < 34000)
//  {
//    Chassis_Info.Target.Wheelrpm[0] = (5.0f * sinf(10.0f * PI * (Counter - 22000) / 1000.0f));
//  }
//  else
//  {
//    Counter = 0;
//  }
//  if (Counter > 0)
//  {
//		VofaData[0]=Chassis_Info.SuperCap.Chassis_Power;
//		VofaData[1]=motor[3].Data.current*RAW_TO_AMPS;
//		VofaData[2]=motor[3].Data.velocity*RPM_TO_RADS;
//    Vofa_JustFloat(VofaData, 3);
//  }
//  PID_Calculate(&PID_Chassis[0], Chassis_Info.Target.Wheelrpm[0], motor[3].Data.velocity);
//  Chassis_Info.Output[3] = PID_Chassis[0].Output;
//}

