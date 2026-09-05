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
#include "arm_math.h"
#include "fast_math.h"
#include "rls.h"

#define Matrix arm_matrix_instance_f32
#define Matrix_64 arm_matrix_instance_f64
#define Matrix_Init arm_mat_init_f32
#define Matrix_Add arm_mat_add_f32
#define Matrix_Subtract arm_mat_sub_f32
#define Matrix_Multiply arm_mat_mult_f32
#define Matrix_Transpose arm_mat_trans_f32
#define Matrix_Inverse arm_mat_inverse_f32
#define Matrix_Inverse_64 arm_mat_inverse_f64

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

typedef struct
{
    struct
    {
        float Yaw;
        float Pitch;
    } Target;
    struct
    {
        float Yaw;
        float Pitch;
    } Row;
    struct
    {
        float Yaw;
        float Pitch;
    } Output;
    float Err;
    float Pitch_Max;
    float Pitch_Min;
    struct
    {
        Matrix V;
        Matrix R;
        Matrix G_B;
        Matrix W_B;
        Matrix W_G;
        Matrix W;
    } Mat;
    uint8_t sizeof_float;
    arm_status MatStatus;
    struct
    {
        float *V;
        float *R;
        float *G_B;
        float *W_B;
        float *W_G;
        float *W;
    } Data;

    struct
    {
        float Pitch_V;
        float Yaw_V;
        float LastYaw;
        float LastPitch;
        float K_Yaw;
        float K_Pitch;
    } Forward;

} Gimbal_Typedef_Info;

typedef enum
{
    SWEEP_STATE_IDLE = 0,
    SWEEP_STATE_RUNNING,
    SWEEP_STATE_COMPLETE
} SweepState_t;

typedef struct
{
    float Frequency;     // ��ǰƵ�� (Hz)
    float Phase;         // �ۻ���λ (rad)
    float U_output;      // ����ź�
    float Amplitude;     // �źŷ�ֵ
    uint32_t LastTick;   // �ϴ�ʱ���
    uint8_t IsSweeping;  // ɨƵ״̬
    uint32_t CycleCount; // ��ǰƵ���µ����ڼ���
    float LastZeroCross; // �ϴι�������λ
} SweepFrequency_Info_Typedef;

extern Gimbal_Typedef_Info Gimbal_Info;

// void IK_Solver_Init(INS_Info_Typedef *INS_Info, Gimbal_Typedef_Info *Gimbal_Info)
//{
//     Gimbal_Info->sizeof_float = sizeof(float);
//     Gimbal_Info->Data.V = (float *)malloc(Gimbal_Info->sizeof_float * 3);
//     memset(Gimbal_Info->Data.V, 0, Gimbal_Info->sizeof_float * 3);
//     Matrix_Init(&Gimbal_Info->Mat.V, 3, 1, (float *)Gimbal_Info->Data.V);

//    Gimbal_Info->Data.R = (float *)malloc(Gimbal_Info->sizeof_float * 9);
//    memset(Gimbal_Info->Data.R, 0, Gimbal_Info->sizeof_float * 9);
//    Matrix_Init(&Gimbal_Info->Mat.R, 3, 3, (float *)Gimbal_Info->Data.R);

//    Gimbal_Info->Data.W = (float *)malloc(Gimbal_Info->sizeof_float * 3);
//    memset(Gimbal_Info->Data.W, 0, Gimbal_Info->sizeof_float * 3);
//    Matrix_Init(&Gimbal_Info->Mat.W, 3, 1, (float *)Gimbal_Info->Data.W);

//    Gimbal_Info->Data.G_B = (float *)malloc(Gimbal_Info->sizeof_float * 9);
//    memset(Gimbal_Info->Data.G_B, 0, Gimbal_Info->sizeof_float * 9);
//    Matrix_Init(&Gimbal_Info->Mat.G_B, 3, 3, (float *)Gimbal_Info->Data.G_B);

//    Gimbal_Info->Data.W_G = (float *)malloc(Gimbal_Info->sizeof_float * 3);
//    memset(Gimbal_Info->Data.W_G, 0, Gimbal_Info->sizeof_float * 3);
//    Matrix_Init(&Gimbal_Info->Mat.W_G, 1, 3, (float *)Gimbal_Info->Data.W_G);

//    Gimbal_Info->Data.W_B = (float *)malloc(Gimbal_Info->sizeof_float * 3);
//    memset(Gimbal_Info->Data.W_B, 0, Gimbal_Info->sizeof_float * 3);
//    Matrix_Init(&Gimbal_Info->Mat.W_B, 1, 3, (float *)Gimbal_Info->Data.W_B);
//}
// void IK_Solver(INS_Info_Typedef *INS_Info, Gimbal_Typedef_Info *Gimbal_Info)
//{
//    Gimbal_Info->Data.G_B[0] = fm_cos_fast(Gimbal_Motor[Yaw].Data.Angle * DegreesToRadians) * fm_cos_fast(Gimbal_Motor[Pitch].Data.Angle * DegreesToRadians);
//    Gimbal_Info->Data.G_B[1] = fm_sin_fast(Gimbal_Motor[Yaw].Data.Angle * DegreesToRadians) * fm_cos_fast(Gimbal_Motor[Pitch].Data.Angle * DegreesToRadians);
//    Gimbal_Info->Data.G_B[2] =-fm_sin_fast(Gimbal_Motor[Pitch].Data.Angle * DegreesToRadians);
//    Gimbal_Info->Data.G_B[3] =-fm_sin_fast(Gimbal_Motor[Yaw].Data.Angle * DegreesToRadians);
//    Gimbal_Info->Data.G_B[4] = fm_cos_fast(Gimbal_Motor[Yaw].Data.Angle * DegreesToRadians);
//    Gimbal_Info->Data.G_B[5] = 0;
//    Gimbal_Info->Data.G_B[6] = fm_cos_fast(Gimbal_Motor[Yaw].Data.Angle * DegreesToRadians) * fm_sin_fast(Gimbal_Motor[Pitch].Data.Angle * DegreesToRadians);
//    Gimbal_Info->Data.G_B[7] = fm_sin_fast(Gimbal_Motor[Yaw].Data.Angle * DegreesToRadians) * fm_sin_fast(Gimbal_Motor[Pitch].Data.Angle * DegreesToRadians);
//    Gimbal_Info->Data.G_B[8] = fm_cos_fast(Gimbal_Motor[Pitch].Data.Angle * DegreesToRadians);

//    Gimbal_Info->Data.W_G[0] = INS_Info->Yaw_Angle;
//    Gimbal_Info->Data.W_G[1] = INS_Info->Pitch_Angle;
//    Gimbal_Info->Data.W_G[2] = INS_Info->Roll_Angle;

//    Gimbal_Info->MatStatus = Matrix_Multiply(&Gimbal_Info->Mat.W_G, &Gimbal_Info->Mat.G_B, &Gimbal_Info->Mat.W_B);

//    Gimbal_Info->Data.R[0] = fm_cos_fast(Comm_Info.Chassis.Yaw.Angle * DegreesToRadians) * fm_cos_fast(Comm_Info.Chassis.Pitch.Angle * DegreesToRadians);

//    Gimbal_Info->Data.R[1] = fm_sin_fast(Comm_Info.Chassis.Yaw.Angle * DegreesToRadians) * fm_cos_fast(Comm_Info.Chassis.Pitch.Angle * DegreesToRadians);

//    Gimbal_Info->Data.R[2] = -fm_sin_fast(Comm_Info.Chassis.Pitch.Angle * DegreesToRadians);

//    Gimbal_Info->Data.R[3] = fm_cos_fast(Comm_Info.Chassis.Yaw.Angle * DegreesToRadians) * fm_sin_fast(Comm_Info.Chassis.Pitch.Angle * DegreesToRadians) * fm_sin_fast(Comm_Info.Chassis.Roll.Angle * DegreesToRadians) - fm_sin_fast(Comm_Info.Chassis.Yaw.Angle * DegreesToRadians) * fm_cos_fast(Comm_Info.Chassis.Roll.Angle * DegreesToRadians);

//    Gimbal_Info->Data.R[4] = fm_sin_fast(Comm_Info.Chassis.Yaw.Angle * DegreesToRadians) * fm_sin_fast(Comm_Info.Chassis.Pitch.Angle * DegreesToRadians) * fm_sin_fast(Comm_Info.Chassis.Roll.Angle * DegreesToRadians) + fm_cos_fast(Comm_Info.Chassis.Yaw.Angle * DegreesToRadians) * fm_cos_fast(Comm_Info.Chassis.Roll.Angle * DegreesToRadians);

//    Gimbal_Info->Data.R[5] = fm_cos_fast(Comm_Info.Chassis.Pitch.Angle * DegreesToRadians) * fm_sin_fast(Comm_Info.Chassis.Roll.Angle * DegreesToRadians);

//    Gimbal_Info->Data.R[6] = fm_cos_fast(Comm_Info.Chassis.Yaw.Angle * DegreesToRadians) * fm_sin_fast(Comm_Info.Chassis.Pitch.Angle * DegreesToRadians) * fm_cos_fast(Comm_Info.Chassis.Roll.Angle * DegreesToRadians) + fm_sin_fast(Comm_Info.Chassis.Yaw.Angle * DegreesToRadians) * fm_sin_fast(Comm_Info.Chassis.Roll.Angle * DegreesToRadians);

//    Gimbal_Info->Data.R[7] = fm_sin_fast(Comm_Info.Chassis.Yaw.Angle * DegreesToRadians) * fm_sin_fast(Comm_Info.Chassis.Pitch.Angle * DegreesToRadians) * fm_cos_fast(Comm_Info.Chassis.Roll.Angle * DegreesToRadians) - fm_cos_fast(Comm_Info.Chassis.Yaw.Angle * DegreesToRadians) * fm_sin_fast(Comm_Info.Chassis.Roll.Angle * DegreesToRadians);

//    Gimbal_Info->Data.R[8] = fm_cos_fast(Comm_Info.Chassis.Pitch.Angle * DegreesToRadians) * fm_cos_fast(Comm_Info.Chassis.Roll.Angle * DegreesToRadians);

//    Gimbal_Info->Data.V[0] = fm_cos_fast(Gimbal_Info->Target.Pitch * DegreesToRadians) * fm_cos_fast(Gimbal_Info->Target.Yaw * DegreesToRadians);
//    Gimbal_Info->Data.V[1] = fm_cos_fast(Gimbal_Info->Target.Pitch * DegreesToRadians) * fm_sin_fast(Gimbal_Info->Target.Yaw * DegreesToRadians);
//    Gimbal_Info->Data.V[2] = -fm_sin_fast(Gimbal_Info->Target.Pitch * DegreesToRadians);

//    Gimbal_Info->MatStatus = Matrix_Multiply(&Gimbal_Info->Mat.R, &Gimbal_Info->Mat.V, &Gimbal_Info->Mat.W);

//    x = Gimbal_Info->Data.W[0];
//    y = Gimbal_Info->Data.W[1];
//    z = Gimbal_Info->Data.W[2];
//    Gimbal_Info->Row.Yaw = atan2(y,x)*RadiansToDegrees;
//    Gimbal_Info->Row.Pitch  = atan2(-z, fm_sqrt_fast(x*x + y*y))*RadiansToDegrees;
//}
#endif
