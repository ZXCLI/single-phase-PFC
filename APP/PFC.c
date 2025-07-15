#include "PFC.h"
#include "PFC_parameter.h"
#include "PFC_setting.h"
#include "spll_1ph_sogi.h"
#include "DCLF32.h"
#include "rampgen.h"


SPLL_1PH_SOGI PFC_PLL;

// 电网参数
float PFC_ac_frequency;

// 归一化测量值
float PFC_iAC_sensed_pu;
float PFC_vAC_sensed_pu;
float PFC_vDC_sensed_pu;
float PFC_iDC_sensed_pu;

float PFC_iAC_offset_pu;
float PFC_vAC_offset_pu;
float PFC_vDC_offset_pu;
float PFC_iDC_offset_pu;
// 实际值
__IO float PFC_iAC_sensed;
float PFC_vAC_sensed;
float PFC_vDC_sensed;
float PFC_iDC_sensed;

float PFC_vDC_Ref_pu;   //直流侧电压参考值
float PFC_iAC_Ref_pu;   //交流侧电流参考值，流入为正

float PFC_Duty_pu;      //占空比

// 系统状态以及保护
volatile int32_t PFC_closeGVloop;
volatile int32_t PFC_closeGIloop;
volatile int32_t PFC_enable_gate;

// ZCD检测
volatile float PFC_Vac;
volatile float PFC_Vac_prev;
volatile uint16_t PFC_EDGE_POS; // 电压处于上升阶段

// 控制器参数
PFC_GI pfc_gi;
PFC_GV pfc_gv;
float PFC_vDC_loop_err;
float PFC_iAC_loop_err;
float PFC_GV_vDC_out;
float PFC_GI_iAC_out;

volatile uint16_t PFC_updateDutyflag;   // 开始发波
volatile uint16_t PFC_startupflag;      // 开机

// 陷波器
PFC_NOTCH VDC_NOTCH_FILTER;

float PFC_vAC_sensed_pu_Filtered;   // 交流侧电压滤波值
float PFC_vDC_sensed_pu_Filtered;   // 直流侧电压滤波值
float PFC_vDC_sensed_pu_NOTCH;
float PFC_iDC_sensed_pu_Filtered;   // 直流侧电流滤波值
uint16_t PFC_vAC_POS;            // 交流侧电压极性

void PFC_initGlobalVariables(void)
{
    // 先关闭所有控制
    PFC_closeGIloop = 0;
    PFC_closeGVloop = 0;
    PFC_enable_gate = 0;
    PFC_startupflag = 0;

    PFC_ac_frequency = AC_FREQUENCY;
    // 初始化锁相环参数
    SPLL_1PH_SOGI_reset(&PFC_PLL);
    SPLL_1PH_SOGI_config(&PFC_PLL,
                          AC_FREQUENCY,
                          ISR_FREQUENCY,
                          PFC_SPLL_COEFF_B0,
                          PFC_SPLL_COEFF_B1
                        );
    SPLL_1PH_SOGI_coeff_calc(&PFC_PLL);

    // 初始化环路参数
    PFC_iAC_sensed_pu = 0.0f;
    PFC_vAC_sensed_pu = 0.0f;
    PFC_vDC_sensed_pu = 0.0f;
    PFC_iDC_sensed_pu = 0.0f;

    PFC_iAC_sensed  = 0.0f;
    PFC_vAC_sensed  = 0.0f;
    PFC_vDC_sensed  = 0.0f;
    PFC_iDC_sensed  = 0.0f;

    PFC_vDC_Ref_pu = 0.0f;
    PFC_iAC_Ref_pu = 0.0f;
    
    DCL_resetDF22(&pfc_gi);
    DCL_resetPI(&pfc_gv);
    PFC_Duty_pu = 0.0f;

    // 初始化电压控制器参数
    pfc_gv.Kp = 0.0f;
    pfc_gv.Ki = 0.0f;
    pfc_gv.Umax = 0.95f;
    pfc_gv.Umin = 0.05f;

    // 初始化电流控制器参数
    PFC_GI_ControllerCoeff(&pfc_gi,
                           0.0f,
                           0.0f,
                           AC_FREQUENCY * 2.0f * CONST_PI_32,
                           ISR_FREQUENCY,
                           0.15f);

    // 初始化陷波器
    PFC_vDC_NotchFltrCoeff(&VDC_NOTCH_FILTER,
                           ISR_FREQUENCY,
                           2.0f * AC_FREQUENCY,
                           0.25f,
                           0.00001f);
}