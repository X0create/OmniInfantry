//
// Created by RM UI Designer
// Static Edition
//

#ifndef UI_g_H
#define UI_g_H

#include "ui_interface.h"

extern ui_interface_arc_t *ui_g_Super_chassis_buffer_Super_cup_box;
extern ui_interface_round_t *ui_g_Super_chassis_buffer_Super_cup_status_round;
extern ui_interface_number_t *ui_g_Super_chassis_buffer_Super_cap_number;
extern ui_interface_arc_t *ui_g_Super_chassis_buffer_Chassis_status;

void ui_init_g_Super_chassis_buffer();
void ui_update_g_Super_chassis_buffer();
void ui_remove_g_Super_chassis_buffer();

extern ui_interface_round_t *ui_g_Text_Status_Vision_status_round;
extern ui_interface_round_t *ui_g_Text_Status_Rfid_status_round;
extern ui_interface_number_t *ui_g_Text_Status_Shooter_42_barrel_heat_number;
extern ui_interface_string_t *ui_g_Text_Status_Vision_status;
extern ui_interface_string_t *ui_g_Text_Status_Rfid_status;

void ui_init_g_Text_Status();
void ui_update_g_Text_Status();
void ui_remove_g_Text_Status();

extern ui_interface_rect_t *ui_g_Xhair_Vision_rang;
extern ui_interface_line_t *ui_g_Xhair_Z_xhair;
extern ui_interface_line_t *ui_g_Xhair_X_1_xhair;
extern ui_interface_line_t *ui_g_Xhair_X_2_xhair;
extern ui_interface_line_t *ui_g_Xhair_X_3_xhair;
extern void ui_init_g_Text_Status_Vision();
extern void ui_init_g_Text_Status_Rfid();
void ui_init_g_Xhair();
void ui_update_g_Xhair();
void ui_remove_g_Xhair();


#endif // UI_g_H
