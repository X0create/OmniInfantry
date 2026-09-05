#ifndef AGV_GIMBAL
#define AGV_GIMBAL

#include "pid.h"
#include "remote_control.h"
#include "INS_Task.h"
#include "cmsis_os.h"
#include "motor.h"
#include "Comm_Task.h"
#include "bsp_uart.h"
#include "math.h"
#include "arm_math.h"
#include "Vision_Task.h"
#include "Keyboard.h"

typedef struct 
{
struct
{
    float Yaw;
    float Pitch;
} Target;
struct
{
    int16_t Yaw;
    int16_t Pitch;
} Output;
float Err;
float Pitch_Max;
float Pitch_Min;
uint8_t Last_Gimbal_Mode;
struct
{
float Pitch;
float Yaw;
float LastYaw;
float LastPitch;
float K_Yaw;
float K_Pitch;
}Forward;
}Gimbal_Typedef_Info;

typedef enum
{
YAW_A,
YAW_V,
PITCH_A,
PITCH_V,
AOTOYAW_A,
AOTOYAW_V,
AOTOPITCH_A,
AOTOPITCH_V,
DAFUPITCH_A,
DAFUPITCH_V,
} Pid_Gimbal_Enum;

extern Gimbal_Typedef_Info Gimbal_Info;
#endif