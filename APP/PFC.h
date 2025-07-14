#ifndef PFC_H
#define PFC_H

#include "PFC_setting.h"
#include "PFC_parameter.h"
#include "PFC_HAL.h"
#include "DCLF32.h"
#include "SPLL_1PH_SOGI.h"
#include "PFC_parameter.h"

#define PFC_GV DCL_PI
#define PFC_GI DCL_DF22
#define PFC_GI_ControllerCoeff computeDF22_PRcontrollerCoeff
#define PFC_vDC_NotchFltrCoeff computeDF22_NotchFltrCoeff

extern SPLL_1PH_SOGI PFC_PLL;

// 电网参数
extern float PFC_Vac_frequency;

// 归一化测量值
extern float PFC_iAC_sensed_pu;
extern float PFC_vAC_sensed_pu;
extern float PFC_vDC_sensed_pu;
extern float PFC_iDC_sensed_pu;

// 实际值
extern float PFC_iAC_sensed;
extern float PFC_vAC_sensed;
extern float PFC_vDC_sensed;
extern float PFC_iDC_sensed;

extern float PFC_vDC_Ref_pu; // 直流侧电压参考值
extern float PFC_iAC_Ref_pu; // 交流侧电流参考值，流入为正

extern float PFC_Duty_pu; // 占空比

// 系统状态以及保护
extern volatile int32_t PFC_closeGVloop;
extern volatile int32_t PFC_closeGIloop;
extern volatile int32_t PFC_enable_gate;

// ZCD检测
extern volatile float PFC_Vac;
extern volatile float PFC_Vac_prev;
extern volatile uint16_t PFC_EDGE_POS;   // 电压处于上升阶段

// 控制器参数
extern PFC_GI pfc_gi;
extern PFC_GV pfc_gv;
extern float PFC_vDC_loop_err;
extern float PFC_iAC_loop_err;

extern volatile uint16_t PFC_updateDutyflag;
extern volatile uint16_t PFC_startupflag;

extern float PFC_vAC_sensed_Filtered;   // 交流侧电压滤波值
extern float PFC_vDC_sensed_pu_NOTCH;
extern uint16_t PFC_vAC_POS;            // 交流侧电压极性

void PFC_initGlobalVariables(void);
void PFC_Init(void);

////////////////Notch filter and PR controllers//////////

static inline void computeDF22_PRcontrollerCoeff(DCL_DF22 *v, float32_t kp, float32_t ki,
                                   float32_t wo, float32_t fs, float32_t wrc)
{
    float32_t temp1, temp2, wo_adjusted;
    wo_adjusted = 2.0f * fs * tanf(wo / (2.0f * fs));

    temp1 = 4.0f * fs * fs + wo_adjusted * wo_adjusted + 4.0f * fs * wrc;
    temp2 = 4.0f * ki * wrc * fs / temp1;
    v->b0 = temp2;
    v->b1 = 0;
    v->b2 = -temp2;
    v->a1 = ((-8.0f * fs * fs + 2 * wo_adjusted * wo_adjusted) / temp1);
    v->a2 = ((temp1 - 8.0f * fs * wrc) / temp1);
    v->x1 = 0;
    v->x2 = 0;

    if (kp != 0) {
        v->b0 += kp;
        v->b1 += kp * v->a1;
        v->b2 += kp * v->a2;
    }

    v->a1 = (v->a1);
    v->a2 = (v->a2);
}

static inline void computeDF22_NotchFltrCoeff(DCL_DF22 *v, float32_t Fs, float32_t notch_freq,
                                             float32_t c1, float32_t c2)
{
    float32_t temp1;
    float32_t temp2;
    float32_t wn2;
    float32_t Ts;
    Ts = 1.0f / Fs;

    // pre warp the notch frequency
    wn2 = 2.0f * Fs * tanf(notch_freq * CONST_PI_32 * Ts); /////////// Here there was a PI, not able to localized in the project and used directly CONST_PI_32!!!!!!/////

    temp1 = 4.0f * Fs * Fs + 4.0f * wn2 * c2 * Fs + wn2 * wn2;
    temp2 = 1.0f / (4.0f * Fs * Fs + 4.0f * wn2 * c1 * Fs + wn2 * wn2);

    v->b0 = temp1 * temp2;
    v->b1 = (-8.0f * Fs * Fs + 2.0f * wn2 * wn2) * temp2;
    v->b2 = (4.0f * Fs * Fs - 4 * wn2 * c2 * Fs + wn2 * wn2) * temp2;
    v->a1 = (-8.0f * Fs * Fs + 2.0f * wn2 * wn2) * temp2;
    v->a2 = (4.0f * Fs * Fs - 4.0f * wn2 * c1 * Fs + wn2 * wn2) * temp2;
}



inline void isr_lab1(void)
{
    // 正向过零检测
    PFC_Vac = PFC_vAC_sensed_pu;
    if (PFC_Vac - PFC_Vac_prev > 0.04f) {
        PFC_EDGE_POS = 1;
    } else {
        PFC_EDGE_POS = 0;
    }
    PFC_Vac = PFC_Vac_prev;
}

inline void isr_lab2(void)
{
    
}

inline void isr_lab3(void)
{
    
}

inline void isr_lab4(void)
{
    SPLL_1PH_SOGI_run(&PFC_PLL, PFC_vAC_sensed_pu);

    // 电压上升检测
    PFC_Vac = PFC_vAC_sensed_Filtered;
    if (PFC_Vac - PFC_Vac_prev > 0.04f) {
        PFC_EDGE_POS = 1;
    } else {
        PFC_EDGE_POS = 0;
    }
    PFC_Vac = PFC_Vac_prev;
}

static inline void isr_lab5(void) 
{

    SPLL_1PH_SOGI_run(&PFC_PLL, PFC_vAC_sensed_pu);

    // 电压上升检测
    PFC_Vac = PFC_vAC_sensed_Filtered;
    if (PFC_Vac - PFC_Vac_prev > 0.04f) {
        PFC_EDGE_POS = 1;
    } else {
        PFC_EDGE_POS = 0;
    }
    PFC_Vac = PFC_Vac_prev;

    if(PFC_startupflag == 1) {  // 准备启动阶段

        // 等待过零点
        if((PFC_vAC_sensed_Filtered > -0.01f) &&
           (PFC_vAC_sensed_Filtered < 0.01f) &&
           (PFC_EDGE_POS == 1)) // 仅在第一次进入时生效
        {
            PFC_updateDutyflag = 1; // 启动
            PFC_vAC_POS = 1;        // 正半周期
        }
    }

    if (PFC_updateDutyflag == 1) {

        // 电压环
        PFC_vDC_loop_err = PFC_vDC_Ref_pu - PFC_vDC_sensed_pu_NOTCH;
        PFC_iAC_Ref_pu = PFC_GV_RUN(&pfc_gv, PFC_vDC_loop_err,0);

        // 电流环
        PFC_iAC_loop_err = PFC_iAC_Ref_pu - PFC_iAC_sensed_pu;
        PFC_Duty_pu = PFC_GI_RUN(&pfc_gi, PFC_iAC_loop_err);

        PFC_PWM_UpdateDuty();
    }
}

static inline void PFC_isr(void)
{
    PFC_ADC_Read();

#if PFC_LAB == 1
    isr_lab1();
#elif PFC_LAB == 2
    isr_lab2();
#elif PFC_LAB == 3
    isr_lab3();
#elif PFC_LAB == 4
    isr_lab4();
#elif PFC_LAB == 5
    isr_lab5();
#endif
}



#endif
