//
// Created by RM UI Designer
// Static Edition
//

#include <string.h>

#include "ui_interface.h"
#include "ui_g.h"

ui_5_frame_t ui_g_Super_chassis_buffer_0;

ui_interface_arc_t *ui_g_Super_chassis_buffer_Super_cup_box = (ui_interface_arc_t*)&(ui_g_Super_chassis_buffer_0.data[0]);
ui_interface_round_t *ui_g_Super_chassis_buffer_Super_cup_status_round = (ui_interface_round_t*)&(ui_g_Super_chassis_buffer_0.data[1]);
ui_interface_number_t *ui_g_Super_chassis_buffer_Super_cap_number = (ui_interface_number_t*)&(ui_g_Super_chassis_buffer_0.data[2]);
ui_interface_arc_t *ui_g_Super_chassis_buffer_Chassis_status = (ui_interface_arc_t*)&(ui_g_Super_chassis_buffer_0.data[3]);

void _ui_init_g_Super_chassis_buffer_0() {
    for (int i = 0; i < 4; i++) {
        ui_g_Super_chassis_buffer_0.data[i].figure_name[0] = 0;
        ui_g_Super_chassis_buffer_0.data[i].figure_name[1] = 0;
        ui_g_Super_chassis_buffer_0.data[i].figure_name[2] = i + 0;
        ui_g_Super_chassis_buffer_0.data[i].operate_tpyel = 1;
    }
    for (int i = 4; i < 5; i++) {
        ui_g_Super_chassis_buffer_0.data[i].operate_tpyel = 0;
    }

    ui_g_Super_chassis_buffer_Super_cup_box->figure_tpye = 4;
    ui_g_Super_chassis_buffer_Super_cup_box->operate_tpyel = 1;
    ui_g_Super_chassis_buffer_Super_cup_box->layer = 0;
    ui_g_Super_chassis_buffer_Super_cup_box->color = 0;
    ui_g_Super_chassis_buffer_Super_cup_box->start_x = 958;
    ui_g_Super_chassis_buffer_Super_cup_box->start_y = 535;
    ui_g_Super_chassis_buffer_Super_cup_box->width = 8;
    ui_g_Super_chassis_buffer_Super_cup_box->start_angle = 228;
    ui_g_Super_chassis_buffer_Super_cup_box->end_angle = 312;
    ui_g_Super_chassis_buffer_Super_cup_box->rx = 390;
    ui_g_Super_chassis_buffer_Super_cup_box->ry = 405;

    ui_g_Super_chassis_buffer_Super_cup_status_round->figure_tpye = 2;
    ui_g_Super_chassis_buffer_Super_cup_status_round->operate_tpyel = 1;
    ui_g_Super_chassis_buffer_Super_cup_status_round->layer = 0;
    ui_g_Super_chassis_buffer_Super_cup_status_round->color = 0;
    ui_g_Super_chassis_buffer_Super_cup_status_round->start_x = 686;
    ui_g_Super_chassis_buffer_Super_cup_status_round->start_y = 234;
    ui_g_Super_chassis_buffer_Super_cup_status_round->width = 15;
    ui_g_Super_chassis_buffer_Super_cup_status_round->r = 12;

    ui_g_Super_chassis_buffer_Super_cap_number->figure_tpye = 5;
    ui_g_Super_chassis_buffer_Super_cap_number->operate_tpyel = 1;
    ui_g_Super_chassis_buffer_Super_cap_number->layer = 0;
    ui_g_Super_chassis_buffer_Super_cap_number->color = 0;
    ui_g_Super_chassis_buffer_Super_cap_number->start_x = 597;
    ui_g_Super_chassis_buffer_Super_cap_number->start_y = 578;
    ui_g_Super_chassis_buffer_Super_cap_number->width = 2;
    ui_g_Super_chassis_buffer_Super_cap_number->font_size = 20;
    ui_g_Super_chassis_buffer_Super_cap_number->number = 666000;

    ui_g_Super_chassis_buffer_Chassis_status->figure_tpye = 4;
    ui_g_Super_chassis_buffer_Chassis_status->operate_tpyel = 1;
    ui_g_Super_chassis_buffer_Chassis_status->layer = 0;
    ui_g_Super_chassis_buffer_Chassis_status->color = 1;
    ui_g_Super_chassis_buffer_Chassis_status->start_x = 960;
    ui_g_Super_chassis_buffer_Chassis_status->start_y = 542;
    ui_g_Super_chassis_buffer_Chassis_status->width = 7;
    ui_g_Super_chassis_buffer_Chassis_status->start_angle = 0;
    ui_g_Super_chassis_buffer_Chassis_status->end_angle = 76;
    ui_g_Super_chassis_buffer_Chassis_status->rx = 82;
    ui_g_Super_chassis_buffer_Chassis_status->ry = 82;


    ui_proc_5_frame(&ui_g_Super_chassis_buffer_0);
    SEND_MESSAGE((uint8_t *) &ui_g_Super_chassis_buffer_0, sizeof(ui_g_Super_chassis_buffer_0));
}

void _ui_update_g_Super_chassis_buffer_0() {
    for (int i = 0; i < 4; i++) {
        ui_g_Super_chassis_buffer_0.data[i].operate_tpyel = 2;
    }

    ui_proc_5_frame(&ui_g_Super_chassis_buffer_0);
    SEND_MESSAGE((uint8_t *) &ui_g_Super_chassis_buffer_0, sizeof(ui_g_Super_chassis_buffer_0));
}

void _ui_remove_g_Super_chassis_buffer_0() {
    for (int i = 0; i < 4; i++) {
        ui_g_Super_chassis_buffer_0.data[i].operate_tpyel = 3;
    }

    ui_proc_5_frame(&ui_g_Super_chassis_buffer_0);
    SEND_MESSAGE((uint8_t *) &ui_g_Super_chassis_buffer_0, sizeof(ui_g_Super_chassis_buffer_0));
}


void ui_init_g_Super_chassis_buffer() {
    _ui_init_g_Super_chassis_buffer_0();
}

void ui_update_g_Super_chassis_buffer() {
    _ui_update_g_Super_chassis_buffer_0();
}

void ui_remove_g_Super_chassis_buffer() {
    _ui_remove_g_Super_chassis_buffer_0();
}

ui_5_frame_t ui_g_Text_Status_0;

ui_interface_round_t *ui_g_Text_Status_Vision_status_round = (ui_interface_round_t*)&(ui_g_Text_Status_0.data[0]);
ui_interface_round_t *ui_g_Text_Status_Rfid_status_round = (ui_interface_round_t*)&(ui_g_Text_Status_0.data[1]);
ui_interface_number_t *ui_g_Text_Status_Shooter_42_barrel_heat_number = (ui_interface_number_t*)&(ui_g_Text_Status_0.data[2]);

void _ui_init_g_Text_Status_0() {
    for (int i = 0; i < 3; i++) {
        ui_g_Text_Status_0.data[i].figure_name[0] = 0;
        ui_g_Text_Status_0.data[i].figure_name[1] = 1;
        ui_g_Text_Status_0.data[i].figure_name[2] = i + 0;
        ui_g_Text_Status_0.data[i].operate_tpyel = 1;
    }
    for (int i = 3; i < 5; i++) {
        ui_g_Text_Status_0.data[i].operate_tpyel = 0;
    }

    ui_g_Text_Status_Vision_status_round->figure_tpye = 2;
    ui_g_Text_Status_Vision_status_round->operate_tpyel = 1;
    ui_g_Text_Status_Vision_status_round->layer = 0;
    ui_g_Text_Status_Vision_status_round->color = 0;
    ui_g_Text_Status_Vision_status_round->start_x = 461;
    ui_g_Text_Status_Vision_status_round->start_y = 693;
    ui_g_Text_Status_Vision_status_round->width = 15;
    ui_g_Text_Status_Vision_status_round->r = 12;

    ui_g_Text_Status_Rfid_status_round->figure_tpye = 2;
    ui_g_Text_Status_Rfid_status_round->operate_tpyel = 1;
    ui_g_Text_Status_Rfid_status_round->layer = 0;
    ui_g_Text_Status_Rfid_status_round->color = 0;
    ui_g_Text_Status_Rfid_status_round->start_x = 461;
    ui_g_Text_Status_Rfid_status_round->start_y = 623;
    ui_g_Text_Status_Rfid_status_round->width = 15;
    ui_g_Text_Status_Rfid_status_round->r = 12;

    ui_g_Text_Status_Shooter_42_barrel_heat_number->figure_tpye = 5;
    ui_g_Text_Status_Shooter_42_barrel_heat_number->operate_tpyel = 1;
    ui_g_Text_Status_Shooter_42_barrel_heat_number->layer = 0;
    ui_g_Text_Status_Shooter_42_barrel_heat_number->color = 0;
    ui_g_Text_Status_Shooter_42_barrel_heat_number->start_x = 1280;
    ui_g_Text_Status_Shooter_42_barrel_heat_number->start_y = 578;
    ui_g_Text_Status_Shooter_42_barrel_heat_number->width = 2;
    ui_g_Text_Status_Shooter_42_barrel_heat_number->font_size = 20;
    ui_g_Text_Status_Shooter_42_barrel_heat_number->number = 666000;


    ui_proc_5_frame(&ui_g_Text_Status_0);
    SEND_MESSAGE((uint8_t *) &ui_g_Text_Status_0, sizeof(ui_g_Text_Status_0));
}

void _ui_update_g_Text_Status_0() {
    for (int i = 0; i < 3; i++) {
        ui_g_Text_Status_0.data[i].operate_tpyel = 2;
    }

    ui_proc_5_frame(&ui_g_Text_Status_0);
    SEND_MESSAGE((uint8_t *) &ui_g_Text_Status_0, sizeof(ui_g_Text_Status_0));
}

void _ui_remove_g_Text_Status_0() {
    for (int i = 0; i < 3; i++) {
        ui_g_Text_Status_0.data[i].operate_tpyel = 3;
    }

    ui_proc_5_frame(&ui_g_Text_Status_0);
    SEND_MESSAGE((uint8_t *) &ui_g_Text_Status_0, sizeof(ui_g_Text_Status_0));
}

ui_string_frame_t ui_g_Text_Status_1;
ui_interface_string_t* ui_g_Text_Status_Vision_status = &(ui_g_Text_Status_1.option);

void _ui_init_g_Text_Status_1() {
    ui_g_Text_Status_1.option.figure_name[0] = 0;
    ui_g_Text_Status_1.option.figure_name[1] = 1;
    ui_g_Text_Status_1.option.figure_name[2] = 3;
    ui_g_Text_Status_1.option.operate_tpyel = 1;

    ui_g_Text_Status_Vision_status->figure_tpye = 7;
    ui_g_Text_Status_Vision_status->operate_tpyel = 1;
    ui_g_Text_Status_Vision_status->layer = 0;
    ui_g_Text_Status_Vision_status->color = 0;
    ui_g_Text_Status_Vision_status->start_x = 215;
    ui_g_Text_Status_Vision_status->start_y = 713;
    ui_g_Text_Status_Vision_status->width = 3;
    ui_g_Text_Status_Vision_status->font_size = 25;
    ui_g_Text_Status_Vision_status->str_length = 8;
    strcpy(ui_g_Text_Status_Vision_status->string, "AUTO_AIM");


    ui_proc_string_frame(&ui_g_Text_Status_1);
    SEND_MESSAGE((uint8_t *) &ui_g_Text_Status_1, sizeof(ui_g_Text_Status_1));
}

void _ui_update_g_Text_Status_1() {
    ui_g_Text_Status_1.option.operate_tpyel = 2;

    ui_proc_string_frame(&ui_g_Text_Status_1);
    SEND_MESSAGE((uint8_t *) &ui_g_Text_Status_1, sizeof(ui_g_Text_Status_1));
}

void _ui_remove_g_Text_Status_1() {
    ui_g_Text_Status_1.option.operate_tpyel = 3;

    ui_proc_string_frame(&ui_g_Text_Status_1);
    SEND_MESSAGE((uint8_t *) &ui_g_Text_Status_1, sizeof(ui_g_Text_Status_1));
}
ui_string_frame_t ui_g_Text_Status_2;
ui_interface_string_t* ui_g_Text_Status_Rfid_status = &(ui_g_Text_Status_2.option);

void _ui_init_g_Text_Status_2() {
    ui_g_Text_Status_2.option.figure_name[0] = 0;
    ui_g_Text_Status_2.option.figure_name[1] = 1;
    ui_g_Text_Status_2.option.figure_name[2] = 4;
    ui_g_Text_Status_2.option.operate_tpyel = 1;

    ui_g_Text_Status_Rfid_status->figure_tpye = 7;
    ui_g_Text_Status_Rfid_status->operate_tpyel = 1;
    ui_g_Text_Status_Rfid_status->layer = 0;
    ui_g_Text_Status_Rfid_status->color = 0;
    ui_g_Text_Status_Rfid_status->start_x = 320;
    ui_g_Text_Status_Rfid_status->start_y = 644;
    ui_g_Text_Status_Rfid_status->width = 3;
    ui_g_Text_Status_Rfid_status->font_size = 25;
    ui_g_Text_Status_Rfid_status->str_length = 4;
    strcpy(ui_g_Text_Status_Rfid_status->string, "RFID");


    ui_proc_string_frame(&ui_g_Text_Status_2);
    SEND_MESSAGE((uint8_t *) &ui_g_Text_Status_2, sizeof(ui_g_Text_Status_2));
}

void _ui_update_g_Text_Status_2() {
    ui_g_Text_Status_2.option.operate_tpyel = 2;

    ui_proc_string_frame(&ui_g_Text_Status_2);
    SEND_MESSAGE((uint8_t *) &ui_g_Text_Status_2, sizeof(ui_g_Text_Status_2));
}

void _ui_remove_g_Text_Status_2() {
    ui_g_Text_Status_2.option.operate_tpyel = 3;

    ui_proc_string_frame(&ui_g_Text_Status_2);
    SEND_MESSAGE((uint8_t *) &ui_g_Text_Status_2, sizeof(ui_g_Text_Status_2));
}

void ui_init_g_Text_Status() {
    _ui_init_g_Text_Status_0();
}

void ui_init_g_Text_Status_Vision(){
   _ui_init_g_Text_Status_1();
}

void ui_init_g_Text_Status_Rfid(){
    _ui_init_g_Text_Status_2();
}

void ui_update_g_Text_Status() {
    _ui_update_g_Text_Status_0();
    _ui_update_g_Text_Status_1();
    _ui_update_g_Text_Status_2();
}

void ui_remove_g_Text_Status() {
    _ui_remove_g_Text_Status_0();
    _ui_remove_g_Text_Status_1();
    _ui_remove_g_Text_Status_2();
}

ui_5_frame_t ui_g_Xhair_0;

ui_interface_rect_t *ui_g_Xhair_Vision_rang = (ui_interface_rect_t*)&(ui_g_Xhair_0.data[0]);
ui_interface_line_t *ui_g_Xhair_Z_xhair = (ui_interface_line_t*)&(ui_g_Xhair_0.data[1]);
ui_interface_line_t *ui_g_Xhair_X_1_xhair = (ui_interface_line_t*)&(ui_g_Xhair_0.data[2]);
ui_interface_line_t *ui_g_Xhair_X_2_xhair = (ui_interface_line_t*)&(ui_g_Xhair_0.data[3]);
ui_interface_line_t *ui_g_Xhair_X_3_xhair = (ui_interface_line_t*)&(ui_g_Xhair_0.data[4]);

void _ui_init_g_Xhair_0() {
    for (int i = 0; i < 5; i++) {
        ui_g_Xhair_0.data[i].figure_name[0] = 0;
        ui_g_Xhair_0.data[i].figure_name[1] = 2;
        ui_g_Xhair_0.data[i].figure_name[2] = i + 0;
        ui_g_Xhair_0.data[i].operate_tpyel = 1;
    }
    for (int i = 5; i < 5; i++) {
        ui_g_Xhair_0.data[i].operate_tpyel = 0;
    }

    ui_g_Xhair_Vision_rang->figure_tpye = 1;
    ui_g_Xhair_Vision_rang->operate_tpyel = 1;
    ui_g_Xhair_Vision_rang->layer = 0;
    ui_g_Xhair_Vision_rang->color = 0;
    ui_g_Xhair_Vision_rang->start_x = 914;
    ui_g_Xhair_Vision_rang->start_y = 493;
    ui_g_Xhair_Vision_rang->width = 1;
    ui_g_Xhair_Vision_rang->end_x = 1005;
    ui_g_Xhair_Vision_rang->end_y = 587;

    ui_g_Xhair_Z_xhair->figure_tpye = 0;
    ui_g_Xhair_Z_xhair->operate_tpyel = 1;
    ui_g_Xhair_Z_xhair->layer = 0;
    ui_g_Xhair_Z_xhair->color = 8;
    ui_g_Xhair_Z_xhair->start_x = 960;
    ui_g_Xhair_Z_xhair->start_y = 299;
    ui_g_Xhair_Z_xhair->width = 1;
    ui_g_Xhair_Z_xhair->end_x = 960;
    ui_g_Xhair_Z_xhair->end_y = 585;

    ui_g_Xhair_X_1_xhair->figure_tpye = 0;
    ui_g_Xhair_X_1_xhair->operate_tpyel = 1;
    ui_g_Xhair_X_1_xhair->layer = 0;
    ui_g_Xhair_X_1_xhair->color = 8;
    ui_g_Xhair_X_1_xhair->start_x = 780;
    ui_g_Xhair_X_1_xhair->start_y = 539;
    ui_g_Xhair_X_1_xhair->width = 1;
    ui_g_Xhair_X_1_xhair->end_x = 1138;
    ui_g_Xhair_X_1_xhair->end_y = 539;

    ui_g_Xhair_X_2_xhair->figure_tpye = 0;
    ui_g_Xhair_X_2_xhair->operate_tpyel = 1;
    ui_g_Xhair_X_2_xhair->layer = 0;
    ui_g_Xhair_X_2_xhair->color = 8;
    ui_g_Xhair_X_2_xhair->start_x = 840;
    ui_g_Xhair_X_2_xhair->start_y = 456;
    ui_g_Xhair_X_2_xhair->width = 1;
    ui_g_Xhair_X_2_xhair->end_x = 1082;
    ui_g_Xhair_X_2_xhair->end_y = 456;

    ui_g_Xhair_X_3_xhair->figure_tpye = 0;
    ui_g_Xhair_X_3_xhair->operate_tpyel = 1;
    ui_g_Xhair_X_3_xhair->layer = 0;
    ui_g_Xhair_X_3_xhair->color = 8;
    ui_g_Xhair_X_3_xhair->start_x = 860;
    ui_g_Xhair_X_3_xhair->start_y = 373;
    ui_g_Xhair_X_3_xhair->width = 1;
    ui_g_Xhair_X_3_xhair->end_x = 1060;
    ui_g_Xhair_X_3_xhair->end_y = 373;


    ui_proc_5_frame(&ui_g_Xhair_0);
    SEND_MESSAGE((uint8_t *) &ui_g_Xhair_0, sizeof(ui_g_Xhair_0));
}

void _ui_update_g_Xhair_0() {
    for (int i = 0; i < 5; i++) {
        ui_g_Xhair_0.data[i].operate_tpyel = 2;
    }

    ui_proc_5_frame(&ui_g_Xhair_0);
    SEND_MESSAGE((uint8_t *) &ui_g_Xhair_0, sizeof(ui_g_Xhair_0));
}

void _ui_remove_g_Xhair_0() {
    for (int i = 0; i < 5; i++) {
        ui_g_Xhair_0.data[i].operate_tpyel = 3;
    }

    ui_proc_5_frame(&ui_g_Xhair_0);
    SEND_MESSAGE((uint8_t *) &ui_g_Xhair_0, sizeof(ui_g_Xhair_0));
}


void ui_init_g_Xhair() {
    _ui_init_g_Xhair_0();
}

void ui_update_g_Xhair() {
    _ui_update_g_Xhair_0();
}

void ui_remove_g_Xhair() {
    _ui_remove_g_Xhair_0();
}

