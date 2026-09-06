#include "Comm_Task.h"
#include "can_protocol.h"


Comm_Info_Typedef Comm_Info;

static void Mode_Com();
static void Comm();
static void UI_Init();
static void UI_Update();

TickType_t CAN_Task_SysTick = 0;
TickType_t Init_Systick = 0;
TickType_t Update_Systick = 0;

void Comm_Task(void const *argument)
{
  /* USER CODE BEGIN Comm_Task */

  /* Infinite loop */
  for (;;)
  {

    CAN_Task_SysTick = HAL_GetTick();
    Comm();//发送数据
		Mode_Com();
		UI_Init();
		UI_Update();
    osDelay(1);
  }
  /* USER CODE END Comm_Task */
}


void Mode_Com()
{

  static bool Ctrl_Flag = 0;
  if (remote_ctrl.key.set.CTRL == 1 && Ctrl_Flag == 0)
  {
    if (Chassis_Info.SuperCap.SuperCap_Flag == false)
    {
      Chassis_Info.SuperCap.SuperCap_Flag = true;
    }
    else if (Chassis_Info.SuperCap.SuperCap_Flag != false)
    {
			
      Chassis_Info.SuperCap.SuperCap_Flag = false;
    }
    Ctrl_Flag = 1;
  }
  else if (remote_ctrl.key.set.CTRL == 0)
    Ctrl_Flag = 0;
	if(Chassis_Info.SuperCap.SuperCap_Flag == true)
		Chassis_Info.SuperCap.EnableCap = true;
	else{
	if(Chassis_Info.SuperCap.Chassis_Power<Referee_System_Info.robot_status.chassis_power_limit)
		Chassis_Info.SuperCap.EnableCap = false;
	}
	
}
void Comm() 
{
	CAN1_TxFrame.header.StdId=DJI_TxFrame_Low;
	CAN1_TxFrame.Data[0] = (uint8_t)( Chassis_PowerCtrl.Output[0]>>8);
	CAN1_TxFrame.Data[1] = (uint8_t)Chassis_PowerCtrl.Output[0];
	CAN1_TxFrame.Data[2] = (uint8_t)(Chassis_PowerCtrl.Output[1]>>8);
	CAN1_TxFrame.Data[3] = (uint8_t)Chassis_PowerCtrl.Output[1];
	CAN1_TxFrame.Data[4] = (uint8_t)(Chassis_PowerCtrl.Output[2]>>8);
	CAN1_TxFrame.Data[5] = (uint8_t)Chassis_PowerCtrl.Output[2];
	CAN1_TxFrame.Data[6] = (uint8_t)(Chassis_PowerCtrl.Output[3]>>8);
	CAN1_TxFrame.Data[7] = (uint8_t)Chassis_PowerCtrl.Output[3];
  USER_CAN_TxMessage(&CAN1_TxFrame);
	CAN1_TxFrame.header.StdId=DJI_TxFrame_High;
	CAN1_TxFrame.Data[0] = (uint8_t)(Shoot_Info.Output.BulletFeed>>8);
	CAN1_TxFrame.Data[1] = (uint8_t)(Shoot_Info.Output.BulletFeed);
  USER_CAN_TxMessage(&CAN1_TxFrame);
	CAN2_TxFrame.header.StdId=CAN_ID_CHASSIS_TO_GIMBAL;
	CAN2_TxFrame.Data[0] = (uint8_t)(Referee_System_Info.robot_status.chassis_power_limit>>8);
    CAN2_TxFrame.Data[1] = (uint8_t)Referee_System_Info.robot_status.chassis_power_limit;
	CAN2_TxFrame.Data[2] = (uint8_t)(Referee_System_Info.power_heat_data.buffer_energy);
    CAN2_TxFrame.Data[3] = 
	CAN2_TxFrame.Data[4] = Chassis_Info.SuperCap.EnableCap;
	CAN2_TxFrame.Data[5] = (uint8_t)(Comm_Info.Chassis_Lost << 1) | (uint8_t)(Comm_Info.Gimbal_Lost);
	CAN2_TxFrame.Data[6] = (uint8_t)(Referee_System_Info.shoot_data.initial_speed*10.f);                      
  USER_CAN_TxMessage(&CAN2_TxFrame);
	
}


void UI_Init()
{
  ui_self_id = Referee_System_Info.robot_status.robot_id;
  if (remote_ctrl.key.set.B == 1)
  {
    if(CAN_Task_SysTick % 500 == 0)
    {
      ui_init_g_Super_chassis_buffer();
      Init_Systick = CAN_Task_SysTick;
    }
    else if (CAN_Task_SysTick % 500 == Init_Systick + 50)
    {
      ui_init_g_Text_Status();
    }
    else if (CAN_Task_SysTick % 500 == Init_Systick + 100)
    {
      ui_init_g_Text_Status_Vision();
    }
    else if (CAN_Task_SysTick % 500 == Init_Systick + 150)
    {
      ui_init_g_Text_Status_Rfid();
    }
    else if (CAN_Task_SysTick % 500 == Init_Systick + 200)
    {
      ui_init_g_Xhair();
    }

  }
  else
  {
    Init_Systick = 0;
  }
}
static void UI_Update()
{

  if (remote_ctrl.key.set.B != 1)
  {

    if (CAN_Task_SysTick % 100 == 0 && Update_Systick == 0)
    {
			
    Update_Systick = CAN_Task_SysTick;
		ui_g_Super_chassis_buffer_Super_cup_box->end_angle = ui_g_Super_chassis_buffer_Super_cup_box->start_angle + Chassis_Info.SuperCap.Surplus_Energy * 0.84 + 1;
		ui_g_Super_chassis_buffer_Super_cap_number->number = Chassis_Info.SuperCap.Surplus_Energy * 1000;
		
		if(Chassis_Info.SuperCap.Surplus_Energy >40)
		{
		ui_g_Super_chassis_buffer_Super_cup_box->color = 3;
		}			
		else if(Chassis_Info.SuperCap.Surplus_Energy <= 40 && Chassis_Info.SuperCap.Surplus_Energy>=15)
		{
		ui_g_Super_chassis_buffer_Super_cup_box->color = 4;
		}
		else if(Chassis_Info.SuperCap.Surplus_Energy < 15)
		{
		ui_g_Super_chassis_buffer_Super_cup_box->color = 7;
		}
		
		if(Chassis_Info.SuperCap.EnableCap == 0)
		{

		ui_g_Super_chassis_buffer_Super_cup_status_round->color = 0;
		}
    else if(Chassis_Info.SuperCap.EnableCap == 1)
		{
		ui_g_Super_chassis_buffer_Super_cup_status_round->color = 2;
		}
		ui_update_g_Super_chassis_buffer();
		
    }
		
    else if (CAN_Task_SysTick == Update_Systick + 30)
    {
			float Gimbal_direction;
			 if(Gimbal_Motor.Data.Angle > 0)
			 {
				 Gimbal_direction = Gimbal_Motor.Data.Angle  + 260;
			 }
			 if(Gimbal_Motor.Data.Angle  < 0)
			 {
				 Gimbal_direction = Gimbal_Motor.Data.Angle + 365 - 90;
			 }
			 ui_g_Super_chassis_buffer_Chassis_status->start_angle = Gimbal_direction-15;
			 ui_g_Super_chassis_buffer_Chassis_status->end_angle = Gimbal_direction+15;
			 
			 ui_update_g_Super_chassis_buffer();
		
    }
    else if (CAN_Task_SysTick == Update_Systick + 60)
    {
		 	if(Shoot_Info.IF_Fire_Accept == 0)
		 {
		 ui_g_Text_Status_Vision_status_round->color = 0;
		 }
		 else if(Shoot_Info.IF_Fire_Accept == 1)
		 {
		 ui_g_Text_Status_Vision_status_round->color = 2;
		 
		 }		
		 ui_update_g_Text_Status();		
			
		}
    else if (CAN_Task_SysTick == Update_Systick + 90)
    {
		 
		 if(Referee_System_Info.rfid_status.rfid_status != 0)
		 {
     ui_g_Text_Status_Rfid_status_round->color = 2;
			 
		 }
		 else
		 if(Referee_System_Info.rfid_status.rfid_status == 0)
		 {
		 ui_g_Text_Status_Rfid_status_round->color = 0;
		 
		 }

		 ui_update_g_Text_Status();				
		 
    }
		
		
    else if (CAN_Task_SysTick == Update_Systick + 120)
    {
	     Update_Systick = 0;
			
    }
	
  }
  else
  {
    Update_Systick = 0;
  }
}



