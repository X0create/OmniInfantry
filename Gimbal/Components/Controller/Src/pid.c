/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : PID.c
 * @brief          : PID functions
 * @author         : GrassFan Wang
 * @date           : 2024/12/29
 * @version        : v1.1
 ******************************************************************************
 * @attention      : To be perfected
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "pid.h"

/* Includes ------------------------------------------------------------------*/

/**
 * @brief ???PID??.
 * @Param PID: ??PID_Info_TypeDef?????,??PID??????.
 * @Param Param: ??PID????????,??PID????.
 * @retval PID????.
 */
static PID_Status_e PID_Param_Init(PID_Info_TypeDef *PID, float Param[PID_PARAMETER_NUM])
{
  //??PID????????? ??? ??PID_FAILED_INIT(?????)
  if (PID->Type == PID_Type_None || Param == NULL)
  {
    return PID_FAILED_INIT;
  }

  PID->Param.KP = Param[0];
  PID->Param.KI = Param[1];
  PID->Param.KD = Param[2];
  PID->Param.Alpha = Param[3];
  if (PID->Param.Alpha > 0.f && PID->Param.Alpha < 1.f)
    LowPassFilter1p_Init(&PID->Dout_LPF, PID->Param.Alpha);

  PID->Param.Deadband = Param[4];
  PID->Param.LimitIntegral = Param[5];
  PID->Param.LimitOutput = Param[6];

  PID->ERRORHandler.ErrorCount = 0;

  return PID_ERROR_NONE;
}
//------------------------------------------------------------------------------

/**
 * @brief ??PID???,??????0.
 * @Param PID: ??PID_Info_TypeDef?????,??PID??????.
 * @retval ?.
 */
static void PID_Calc_Clear(PID_Info_TypeDef *PID)
{
  //??????0
  memset(PID->Err, 0, sizeof(PID->Err));
  PID->Integral = 0;

  PID->Pout = 0;
  PID->Iout = 0;
  PID->Dout = 0;
  PID->Output = 0;
}
//------------------------------------------------------------------------------

/**
 * @brief ???PID???.
 * @Param PID: ??PID_Info_TypeDef?????,??PID??????.
 * @Param Type: PID?????.
 * @Param Param:??PID????????,??PID????.
 * @retval PID????.
 */
void PID_Init(PID_Info_TypeDef *Pid, PID_Type_e Type, float Param[PID_PARAMETER_NUM])
{

  Pid->Type = Type;

  Pid->PID_Calc_Clear = PID_Calc_Clear;
  Pid->PID_Param_Init = PID_Param_Init;
  Pid->PID_Calc_Clear(Pid);
  Pid->ERRORHandler.Status = Pid->PID_Param_Init(Pid, Param);
}
//------------------------------------------------------------------------------

/**
 * @brief ??PID????
 * @Param PID: ??PID_Info_TypeDef?????,??PID??????.
 * @retval ?.
 */
static void PID_ErrorHandle(PID_Info_TypeDef *PID)
{
  /* Judge NAN/INF */
  if (isnan(PID->Output) == true || isinf(PID->Output) == true)
  {
    PID->ERRORHandler.Status = PID_CALC_NANINF;
  }
}


/**
 * @brief  PID???????
 * @Param  *PID pointer to a PID_TypeDef_t structure that contains
 *              the configuration information for the specified PID.
 * @Param  Target  Target for the PID controller
 * @Param  Measure Measure for the PID controller
 * @retval the PID Output
 */
float PID_Calculate(PID_Info_TypeDef *Pid, float Target, float Measure)
{
  /* update the Pid error status */
  PID_ErrorHandle(Pid);
  if (Pid->ERRORHandler.Status != PID_ERROR_NONE)
  {
    Pid->PID_Calc_Clear(Pid);
    return 0;
  }

  /* update the target/measure */
  Pid->Target = Target;
  Pid->Measure = Measure;

  /* update the error */
  Pid->Err[2] = Pid->Err[1];
  Pid->Err[1] = Pid->Err[0];
  Pid->Err[0] = Pid->Target - Pid->Measure;

  if (fabsf(Pid->Err[0]) >= Pid->Param.Deadband)
  {
    /* update the Pid controller output */
    if (Pid->Type == PID_POSITION)
    {
      /* Update the Pid Integral */
      if (Pid->Param.KI != 0)
        Pid->Integral += Pid->Err[0];
      else
        Pid->Integral = 0;

      VAL_LIMIT(Pid->Integral, -Pid->Param.LimitIntegral, Pid->Param.LimitIntegral);

      /* Update the Proportional Output,Integral Output,Derivative Output */
      Pid->Pout = Pid->Param.KP * Pid->Err[0];
      Pid->Iout = Pid->Param.KI * Pid->Integral;
      Pid->Dout = Pid->Param.KD * (Pid->Err[0] - Pid->Err[1]);
      if (Pid->Param.Alpha > 0.f && Pid->Param.Alpha < 1.f)
      {

        Pid->Dout_LPF.Alpha = Pid->Param.Alpha;
        Pid->Dout = LowPassFilter1p_Update(&Pid->Dout_LPF, Pid->Dout);
      }
      /* update the Pid output */
      Pid->Output = Pid->Pout + Pid->Iout + Pid->Dout;
      VAL_LIMIT(Pid->Output, -Pid->Param.LimitOutput, Pid->Param.LimitOutput);
    }
    else if (Pid->Type == PID_VELOCITY)
    {
      /* Update the Proportional Output,Integral Output,Derivative Output */
      Pid->Pout = Pid->Param.KP * (Pid->Err[0] - Pid->Err[1]);
      Pid->Iout = Pid->Param.KI * (Pid->Err[0]);
      Pid->Dout = Pid->Param.KD * (Pid->Err[0] - 2.f * Pid->Err[1] + Pid->Err[2]);
      if (Pid->Param.Alpha > 0.f && Pid->Param.Alpha < 1.f)
      {

        Pid->Dout_LPF.Alpha = Pid->Param.Alpha;
        Pid->Dout = LowPassFilter1p_Update(&Pid->Dout_LPF, Pid->Dout);
      }
      /* update the Pid output */
      Pid->Output += Pid->Pout + Pid->Iout + Pid->Dout;
      VAL_LIMIT(Pid->Output, -Pid->Param.LimitOutput, Pid->Param.LimitOutput);
    }
  }
  return Pid->Output;
}