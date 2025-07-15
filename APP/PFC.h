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
#define PFC_NOTCH DCL_DF22
#define PFC_GI_ControllerCoeff computeDF22_PRcontrollerCoeff
#define PFC_vDC_NotchFltrCoeff computeDF22_NotchFltrCoeff

#define PFC_EMAVG_MACRO(in, out, multiplier) out = ((out - in) * multiplier) + in;

extern SPLL_1PH_SOGI PFC_PLL;

// 电网参数
extern float PFC_Vac_frequency;

// 归一化测量值
extern float PFC_iAC_sensed_pu;
extern float PFC_vAC_sensed_pu;
extern float PFC_vDC_sensed_pu;
extern float PFC_iDC_sensed_pu;

extern float PFC_iAC_offset_pu;
extern float PFC_vAC_offset_pu;
extern float PFC_vDC_offset_pu;
extern float PFC_iDC_offset_pu;

// 实际值
extern __IO float PFC_iAC_sensed;
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
extern float PFC_GV_vDC_out;
extern float PFC_GI_iAC_out;

extern volatile uint16_t PFC_updateDutyflag;
extern volatile uint16_t PFC_startupflag;


extern PFC_NOTCH VDC_NOTCH_FILTER;
extern float PFC_vAC_sensed_pu_Filtered; // 交流侧电压滤波值
extern float PFC_vDC_sensed_pu_Filtered; // 直流侧电压滤波值
extern float PFC_vDC_sensed_pu_NOTCH;
extern float PFC_iDC_sensed_pu_Filtered;
extern uint16_t PFC_vAC_POS;            // 交流侧电压极性

void PFC_initGlobalVariables(void);

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

static inline void PFC_readCurrentAndVoltageSignals(void)
{
    // 读取注入组的ADC值
    PFC_vAC_sensed_pu = (PFC_VAC_READ * PFC_ADC_PU_SCALE_FACTOR - PFC_vAC_offset_pu) * 2.0f;
    PFC_iAC_sensed_pu = (PFC_VAC_READ * PFC_ADC_PU_SCALE_FACTOR - PFC_iAC_offset_pu) * 2.0f;
    PFC_vDC_sensed_pu = (PFC_VDC_READ * PFC_ADC_PU_SCALE_FACTOR - PFC_iAC_offset_pu);
    PFC_iDC_sensed_pu = (PFC_iDC_sensed_pu * PFC_ADC_PU_SCALE_FACTOR - PFC_iAC_offset_pu) * 2.0f;

    // 适当的滤波
    // 低通滤波
    PFC_vAC_sensed_pu_Filtered += PFC_AC_FILTER_CONSTANT *
                                  (PFC_vAC_sensed_pu -
                                   PFC_vAC_sensed_pu_Filtered);
    PFC_vDC_sensed_pu_Filtered += PFC_DC_FILTER_CONSTANT *
                                  (PFC_vDC_sensed_pu -
                                   PFC_vDC_sensed_pu_Filtered);

    // 陷波器
    PFC_vDC_sensed_pu_NOTCH = PFC_NOTCH_RUN(&VDC_NOTCH_FILTER,
                                            PFC_vDC_sensed_pu_Filtered);
}

static inline void PFC_checkOverFlow(void)
{
    PFC_EMAVG_MACRO(PFC_iDC_sensed_pu, PFC_iDC_sensed_pu_Filtered, 0.80f)

    PFC_vDC_sensed = PFC_vDC_sensed_pu_NOTCH * PFC_VDC_MAX;
    PFC_iDC_sensed = PFC_iDC_sensed_pu_Filtered * PFC_IDC_MAX;
    
    if(PFC_vDC_sensed > PFC_VDC_OVERVOLT_LIMIT)
    {
        
    }
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

static inline void isr_lab4(void)
{
    SPLL_1PH_SOGI_run(&PFC_PLL, PFC_vAC_sensed_pu);

    // 电压上升检测
    PFC_Vac = PFC_vAC_sensed_pu_Filtered;
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
    PFC_Vac = PFC_vAC_sensed_pu_Filtered;
    if (PFC_Vac - PFC_Vac_prev > 0.04f) {
        PFC_EDGE_POS = 1;
    } else {
        PFC_EDGE_POS = 0;
    }
    PFC_Vac = PFC_Vac_prev;
    
    // 准备启动，检测到过零开始发波
    if(PFC_startupflag == 1) {  

        // 等待过零点
        if((PFC_vAC_sensed_pu_Filtered > -0.01f) &&
           (PFC_vAC_sensed_pu_Filtered < 0.01f) &&
           (PFC_EDGE_POS == 1)) // 仅在第一次进入时生效
        {
            PFC_updateDutyflag = 1; // 启动
            PFC_vAC_POS = 1;        // 正半周期
            PFC_enable_gate = 1;    // 开半桥
        }
    }

    if (PFC_updateDutyflag == 1) {

        // 电压环
        PFC_vDC_loop_err = PFC_vDC_Ref_pu - PFC_vDC_sensed_pu_NOTCH;
        PFC_GV_vDC_out = PFC_GV_RUN(&pfc_gv, PFC_vDC_loop_err,0);
        PFC_iAC_Ref_pu = PFC_GV_vDC_out * PFC_PLL.sine;

        // 电流环
        PFC_iAC_loop_err = PFC_iAC_Ref_pu - PFC_iAC_sensed_pu;
        PFC_GI_iAC_out = PFC_GI_RUN(&pfc_gi, PFC_iAC_loop_err);

        // 前馈
        PFC_Duty_pu = PFC_GI_iAC_out;   // 此处暂未加入前馈

        PFC_PWM_UpdateDuty();
    }
    
    PFC_checkOverFlow();    // 检测溢出或掉电
}

static inline void PFC_isr(void)
{
    PFC_readCurrentAndVoltageSignals();

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
