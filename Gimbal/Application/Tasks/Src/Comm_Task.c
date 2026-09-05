#include "Comm_Task.h"

Comm_Info_Typedef Comm_Info;
static void Mode_Com();
static void Comm();
TickType_t Comm_Task_SysTick = 0;
bool pitch_ok;
bool yaw_ok;
void Comm_Task(void const *argument)
{
  /* USER CODE BEGIN Comm_Task */
  Comm_Info.Control_Mode = Remote_Control;
  Comm_Info.Shoot_Mode = Shoot_OFF;
  Comm_Info.Gimbal_Mode = Gimbal_ON;
  Comm_Info.Remote_State = DT7_Ctrl;
  /* Infinite loop */
  for (;;)
  {
    Comm_Task_SysTick = osKernelSysTick();
    Comm();     // 发送数据
    Mode_Com(); // 模式判断

    osDelay(1);
  }
  /* USER CODE END Comm_Task */
}

float uint8_to_float(uint8_t *data)
{
  union
  {
    uint8_t bullet_speed_u8[4];
    float bullet_speed_f;
  } to_gimbal;
  for (int i = 0; i < 4; i++)
  {
    to_gimbal.bullet_speed_u8[i] = data[i];
  }
  float received_speed = to_gimbal.bullet_speed_f;
  return received_speed;
}
void Comm()
{
  Comm_Info.Move.Vx = KeyBoard_Info.key.set.W * 660 - KeyBoard_Info.key.set.S * 660 + VT13_Info.RC.Channel[2] + remote_ctrl.rc.ch[3] + remote_ctrl.key.set.W * 660 - remote_ctrl.key.set.S * 660;
  Comm_Info.Move.Vy = KeyBoard_Info.key.set.D * 660 - KeyBoard_Info.key.set.A * 660 + VT13_Info.RC.Channel[3] + remote_ctrl.rc.ch[2] + remote_ctrl.key.set.D * 660 - remote_ctrl.key.set.A * 660;


  CAN2_TxFrame.header.StdId = 0x310;
  CAN2_TxFrame.Data[0] = (uint8_t)(Comm_Info.Move.Vx >> 8);
  CAN2_TxFrame.Data[1] = (uint8_t)(Comm_Info.Move.Vx);
  CAN2_TxFrame.Data[2] = (uint8_t)(Comm_Info.Move.Vy >> 8);
  CAN2_TxFrame.Data[3] = (uint8_t)(Comm_Info.Move.Vy);
  CAN2_TxFrame.Data[4] = ((uint8_t)(Comm_Info.Shoot_Mode << 6) | (uint8_t)(Comm_Info.Gimbal_Mode << 4) | (uint8_t)(Shoot_Info.Fire_Flag << 3) | (uint8_t)((Vision_Info.IF_Fire_Accept) << 2) );
  CAN2_TxFrame.Data[5] = (uint8_t)(Comm_Info.Chassis_Mode);
	CAN2_TxFrame.Data[6] = (uint8_t)(KeyBoard_Info.key.v>>8);
	CAN2_TxFrame.Data[7] = (uint8_t)(KeyBoard_Info.key.v);
  USER_CAN_TxMessage(&CAN2_TxFrame);

  CAN1_TxFrame.header.StdId = 0x200;
  CAN1_TxFrame.Data[0] = (uint8_t)(Shoot_Info.Output.ShootR >> 8);
  CAN1_TxFrame.Data[1] = (uint8_t)(Shoot_Info.Output.ShootR);
  CAN1_TxFrame.Data[2] = (uint8_t)(Shoot_Info.Output.ShootL >> 8);
  CAN1_TxFrame.Data[3] = (uint8_t)(Shoot_Info.Output.ShootL);
  USER_CAN_TxMessage(&CAN1_TxFrame);
};

void Mode_Com()
{
  if (Comm_Info.Last_Gimbal_Mode != Comm_Info.Gimbal_Mode)
  {
		
    if (Comm_Info.Gimbal_Mode != Gimbal_OFF)
    {
      DM_Motor_Command(&CAN1_TxFrame, &Gimbal_Motor[Pitch], Motor_Enable);
      osDelay(30);
      DM_Motor_Command(&CAN2_TxFrame, &Gimbal_Motor[Yaw], Motor_Enable);
      osDelay(30);
    }
    else
    {
      DM_Motor_Command(&CAN1_TxFrame, &Gimbal_Motor[Pitch], Motor_Disable);
      osDelay(30);
      DM_Motor_Command(&CAN2_TxFrame, &Gimbal_Motor[Yaw], Motor_Disable);
      osDelay(30); 
    }
  }
	
	
  if (Comm_Info.Remote_State == DT7_Ctrl)
  {
    Comm_Info.Last_Gimbal_Mode = Comm_Info.Gimbal_Mode;
    if (remote_ctrl.rc_lost != true)
    {
      if (Comm_Info.Control_Mode == Remote_Control)
      {
        if (remote_ctrl.rc.ch[4] >= 66)
        {
          Shoot_Info.Fire_Flag = true;
        }
        else
        {
          Shoot_Info.Fire_Flag = 0;
        }
        switch (remote_ctrl.rc.s[1])
        {
        case 1:
          switch (remote_ctrl.rc.s[0])
          {
          case 1:
          {
            Comm_Info.Chassis_Mode = Chassis_Follow;
            Comm_Info.Gimbal_Mode = Gimbal_ON;
            Comm_Info.Shoot_Mode = Shoot_Repeat;
            Comm_Info.Control_Mode = KeyBoard_Control;
          }
          break;
          case 3:
          {
            Comm_Info.Chassis_Mode = Chassis_Spin;
            Comm_Info.Gimbal_Mode = Gimbal_AutoAim;
            Comm_Info.Shoot_Mode = Shoot_Repeat;
            Comm_Info.Control_Mode = Remote_Control;
          }
          break;
          case 2:
          {
            Comm_Info.Chassis_Mode = Chassis_OFF;
            Comm_Info.Gimbal_Mode = Gimbal_OFF;
            Comm_Info.Shoot_Mode = Shoot_OFF;
            Comm_Info.Control_Mode = Remote_Control;
          }
          break;
          }
          break;

        case 3:
          switch (remote_ctrl.rc.s[0])
          {
          case 1:
          {
            Comm_Info.Chassis_Mode = Chassis_Spin;
            Comm_Info.Gimbal_Mode = Gimbal_ON;
            Comm_Info.Shoot_Mode = Shoot_OFF;
            Comm_Info.Control_Mode = Remote_Control;
          }
          break;
          case 3:
          {
            Comm_Info.Chassis_Mode = Chassis_Follow;
            Comm_Info.Gimbal_Mode = Gimbal_ON;
            Comm_Info.Shoot_Mode = Shoot_OFF;
            Comm_Info.Control_Mode = Remote_Control;
          }
          break;
          case 2:
          {
            Comm_Info.Chassis_Mode = Chassis_OFF;
            Comm_Info.Gimbal_Mode = Gimbal_ON;
            Comm_Info.Shoot_Mode = Shoot_OFF;
            Comm_Info.Control_Mode = Remote_Control;
          }
          break;
          }
          break;

        case 2:
          switch (remote_ctrl.rc.s[0])
          {
          case 1:
          {
            Comm_Info.Control_Mode = KeyBoard_Control;
          }
          break;
          case 3:
          {
            Comm_Info.Chassis_Mode = Chassis_OFF;
            Comm_Info.Gimbal_Mode = Gimbal_OFF;
            Comm_Info.Shoot_Mode = Shoot_OFF;
            Comm_Info.Control_Mode = Remote_Control;
          }
          break;
          case 2:
          {
            Comm_Info.Chassis_Mode = Chassis_OFF;
            Comm_Info.Gimbal_Mode = Gimbal_OFF;
            Comm_Info.Shoot_Mode = Shoot_OFF;
            Comm_Info.Control_Mode = Remote_Control;
          }
          break;
          }
          break;
        default:
        {
          Comm_Info.Chassis_Mode = Chassis_OFF;
          Comm_Info.Gimbal_Mode = Gimbal_OFF;
          Comm_Info.Shoot_Mode = Shoot_OFF;
          Comm_Info.Control_Mode = Remote_Control;
        }
        break;
        }
      }
      else if (Comm_Info.Control_Mode == KeyBoard_Control)
      { 
				
				if(remote_ctrl.rc.s[0]!=1||remote_ctrl.rc.s[1]!=1)
					Comm_Info.Control_Mode=Remote_Control;
        Shoot_Info.Flag_Last = Shoot_Info.Fire_Flag;
        if (KeyBoard_Info.mouse.press_l == 1)
          Shoot_Info.Fire_Flag = true;
        else
          Shoot_Info.Fire_Flag = false;
        if (Vision_Info.Distance > 0)
          Vision_Info.IF_Aiming_Enable = true;
        else
          Vision_Info.IF_Aiming_Enable = false;

        
        Comm_Info.Shoot_Mode = Shoot_Repeat;
        if (KeyBoard_Info.mouse.press_r == 1)
        {
          Comm_Info.Gimbal_Mode = Gimbal_AutoAim;
        }
        else
        {
          Comm_Info.Gimbal_Mode = Gimbal_ON;
        }
      

        if (Comm_Info.Chassis_Lost == 1)
          Comm_Info.Chassis_Mode = Chassis_Follow;

        static bool SHIFT_Flag = 0;
        if (KeyBoard_Info.key.set.SHIFT == 1 && SHIFT_Flag == 0)
        {
          if (Comm_Info.Chassis_Mode == Chassis_Follow)
          {
            Comm_Info.Chassis_Mode = Chassis_Spin;
          }
          else if (Comm_Info.Chassis_Mode != Chassis_Follow)
          {
            Comm_Info.Chassis_Mode = Chassis_Follow;
          }
          SHIFT_Flag = 1;
        }
        else if (KeyBoard_Info.key.set.SHIFT == 0)
          SHIFT_Flag = 0;

				
        static bool E_Flag = 0;
        if (KeyBoard_Info.key.set.E == 1 && E_Flag == 0)
        {
          Gimbal_Info.Target.Yaw += 180.f;
          E_Flag = 1;
        }
        else if (KeyBoard_Info.key.set.E == 0)
          E_Flag = 0;
				
        static int16_t V_Flag = 0;
        if (KeyBoard_Info.key.set.V == 1 && V_Flag <= 1000)
        {
          V_Flag++;
          HAL_NVIC_SystemReset();
        }
        else
          V_Flag = 0;
      }
      }
    else
    {
      Comm_Info.Chassis_Mode = Chassis_OFF;
      Comm_Info.Gimbal_Mode = Gimbal_OFF;
      Comm_Info.Shoot_Mode = Shoot_OFF;
    }


  }
  else if (Comm_Info.Remote_State == VT13_Ctrl)
  {
if(Gimbal_Motor[Pitch].Data.State==0)
{
DM_Motor_Command(&CAN1_TxFrame, &Gimbal_Motor[Pitch], Motor_Enable);
osDelay(30);
}
if(Gimbal_Motor[Yaw].Data.State==0)
{
DM_Motor_Command(&CAN1_TxFrame, &Gimbal_Motor[Yaw], Motor_Enable);
osDelay(30);
}


    Comm_Info.Last_Gimbal_Mode = Comm_Info.Gimbal_Mode;
    if (VT13_Info.RC.Switch == 1)
      Comm_Info.Control_Mode = Remote_Control;
    else if (VT13_Info.RC.Switch == 2)
      Comm_Info.Control_Mode = KeyBoard_Control;
    else
    {
      Comm_Info.Chassis_Mode = Chassis_OFF;
      Comm_Info.Gimbal_Mode = Gimbal_OFF;
      Comm_Info.Shoot_Mode = Shoot_OFF;
      if (VT13_Info.RC.Stop == 1)
        Comm_Info.Remote_State = DT7_Ctrl;
    }

    if (Comm_Info.Control_Mode == Remote_Control)
    {
      Shoot_Info.Flag_Last = Shoot_Info.Fire_Flag;
      if (abs(VT13_Info.RC.Wheel) >= 10)
        Shoot_Info.Fire_Flag = true;
      else
        Shoot_Info.Fire_Flag = false;
      // 开火
      static bool Right_Flag = 0;
      if (VT13_Info.RC.Right == 1 && Right_Flag == 0)
      {
        if (Comm_Info.Gimbal_Mode == Gimbal_ON)
        {
          Comm_Info.Gimbal_Mode = Gimbal_AutoAim;
          Vision_Info.Mode = Vision_Info.Color;
        }
        else if (Comm_Info.Gimbal_Mode != Gimbal_ON)
        {
          Comm_Info.Gimbal_Mode = Gimbal_ON;
        }
        Right_Flag = 1;
      }
      else if (VT13_Info.RC.Right == 0)
        Right_Flag = 0;
      // 触发切换云台模式
      if (Vision_Info.Distance > 0)
        Vision_Info.IF_Aiming_Enable = true;
      else
        Vision_Info.IF_Aiming_Enable = false;

      static bool Left_Flag = 0;
      if (VT13_Info.RC.Left == 1 && VT13_Info.RC.Stop == 1)
        Comm_Info.Shoot_Mode = Shoot_OFF;
      if (VT13_Info.RC.Left == 1 && Left_Flag == 0)
      {
        if (Comm_Info.Shoot_Mode == Shoot_Repeat)
        {
          Comm_Info.Shoot_Mode = Shoot_Single;
        }
        else if (Comm_Info.Shoot_Mode != Shoot_Repeat)
        {
          Comm_Info.Shoot_Mode = Shoot_Repeat;
        }
        Left_Flag = 1;
      }
      else if (VT13_Info.RC.Left == 0)
        Left_Flag = 0;
      // 触发切换射击模式

      static bool Trigger_Flag;
      if (VT13_Info.RC.Stop == 1 && VT13_Info.RC.Trigger == 1)
        Comm_Info.Chassis_Mode = Chassis_OFF;
      if (VT13_Info.RC.Trigger == 1 && Trigger_Flag == 0)
      {
        if (Comm_Info.Chassis_Mode == Chassis_Follow)
        {
          Comm_Info.Chassis_Mode = Chassis_Spin;
        }
        else if (Comm_Info.Chassis_Mode != Chassis_Follow)
        {
          Comm_Info.Chassis_Mode = Chassis_Follow;
        }
        Trigger_Flag = 1;
      }
      else if (VT13_Info.RC.Trigger == 0)
        Trigger_Flag = 0;
      // 单次触发切换底盘模式
    }

    else if (Comm_Info.Control_Mode == KeyBoard_Control)
    {

      Shoot_Info.Flag_Last = Shoot_Info.Fire_Flag;
      if (KeyBoard_Info.mouse.press_l == 1)
        Shoot_Info.Fire_Flag = true;
      else
        Shoot_Info.Fire_Flag = false;
      if (Vision_Info.Distance > 0)
        Vision_Info.IF_Aiming_Enable = true;
      else
        Vision_Info.IF_Aiming_Enable = false;

      // 长摁触发开火

      if (KeyBoard_Info.mouse.press_r == 1)
      {
        Comm_Info.Gimbal_Mode = Gimbal_AutoAim;
       
          if (Vision_Info.Color == RED)
            Vision_Info.Mode = RED;
          else if (Vision_Info.Color == BULE)
            Vision_Info.Mode = BULE;
        
      }
      else
      {
        Comm_Info.Gimbal_Mode = Gimbal_ON;
        Comm_Info.Shoot_Mode = Shoot_Repeat;
      }
      // 长摁触发自瞄或打符模式

      if (Comm_Info.Chassis_Lost == 1)
        Comm_Info.Chassis_Mode = Chassis_Follow;
      if (KeyBoard_Info.key.set.C == 1)
        Comm_Info.Chassis_Mode = Chassis_OFF;

      static bool SHIFT_Flag = 0;
      if (KeyBoard_Info.key.set.SHIFT == 1 && SHIFT_Flag == 0)
      {
        if (Comm_Info.Chassis_Mode == Chassis_Follow)
        {
          Comm_Info.Chassis_Mode = Chassis_Spin;
        }
        else if (Comm_Info.Chassis_Mode != Chassis_Follow)
        {
          Comm_Info.Chassis_Mode = Chassis_Follow;
        }
        SHIFT_Flag = 1;
      }
      else if (KeyBoard_Info.key.set.SHIFT == 0)
        SHIFT_Flag = 0;

      static bool E_Flag = 0;
      if (KeyBoard_Info.key.set.E == 1 && E_Flag == 0)
      {
        Gimbal_Info.Target.Yaw += 180.f;
        E_Flag = 1;
      }
      else if (KeyBoard_Info.key.set.E == 0)
        E_Flag = 0;
      static int16_t V_Flag = 0;
      if (KeyBoard_Info.key.set.V == 1 && V_Flag <= 1000)
      {
        V_Flag++;
        HAL_NVIC_SystemReset();
      }
      else
        V_Flag = 0;
    }

    else
    {
      Comm_Info.Chassis_Mode = Chassis_OFF;
      Comm_Info.Gimbal_Mode = Gimbal_OFF;
      Comm_Info.Shoot_Mode = Shoot_OFF;
    }
  }
}
