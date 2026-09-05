
#include "Keyboard.h"
/* Private variables ---------------------------------------------------------*/
/**
 * @brief structure that contains the information of keyboard
 */
KeyBoard_Info_Typedef KeyBoard_Info;
void Remote_Control_Lost_Check(void){

 Remote_Message_Moniter(&remote_ctrl);
 Image_Transmission_Message_Moniter(&Image_Transmission_Info);
 VT13_Message_Moniter(&VT13_Info);	
	
}


void Keyboard_Info_Update(KeyBoard_Info_Typedef *Keyboard_Info){

    Remote_Control_Lost_Check();
	
	  if(remote_ctrl.rc_lost != 1){
		 
		 Keyboard_Info->mouse.x = remote_ctrl.mouse.x;
		 Keyboard_Info->mouse.y = remote_ctrl.mouse.y;
		 Keyboard_Info->mouse.z = remote_ctrl.mouse.z;
		 Keyboard_Info->mouse.press_l = remote_ctrl.mouse.press_l;
		 Keyboard_Info->mouse.press_r = remote_ctrl.mouse.press_r;
		 Keyboard_Info->key.v = remote_ctrl.key.v;

		}else if(remote_ctrl.rc_lost == 1){
//			if(Comm_Info.Control_Mode==KeyBoard_Control)
//			Comm_Info.Remote_State = VT13_Ctrl;
//
			if (Image_Transmission_Info.remote_control.lost != 1)
			{
				Keyboard_Info->mouse.x = Image_Transmission_Info.remote_control.mouse_x;
				Keyboard_Info->mouse.y = Image_Transmission_Info.remote_control.mouse_y;
				Keyboard_Info->mouse.z = Image_Transmission_Info.remote_control.mouse_z;
				Keyboard_Info->mouse.press_l = Image_Transmission_Info.remote_control.left_button_down;
				Keyboard_Info->mouse.press_r = Image_Transmission_Info.remote_control.right_button_down;
				Keyboard_Info->key.v = Image_Transmission_Info.remote_control.Key.keyboard_value;
			}
			else if (Image_Transmission_Info.remote_control.lost == 1)
			{

				if (VT13_Info.lost != 1)
				{

					Keyboard_Info->mouse.x = VT13_Info.Mouse.X;
					Keyboard_Info->mouse.y = VT13_Info.Mouse.Y;
					Keyboard_Info->mouse.z = VT13_Info.Mouse.Z;
					Keyboard_Info->mouse.press_l = VT13_Info.Mouse.Press_L;
					Keyboard_Info->mouse.press_r = VT13_Info.Mouse.Press_R;
					Keyboard_Info->key.v = VT13_Info.Key.V;
				}
				else if (VT13_Info.lost == 1)
				{

					Keyboard_Info->mouse.x = 0;
					Keyboard_Info->mouse.y = 0;
					Keyboard_Info->mouse.z = 0;
					Keyboard_Info->mouse.press_l = 0;
					Keyboard_Info->mouse.press_r = 0;
					Keyboard_Info->key.v = 0;
				}
			}
		}




}