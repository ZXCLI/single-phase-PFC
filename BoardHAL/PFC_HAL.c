#include "PFC.h"
#include "PFC_HAL.h"
#include "SEGGER_RTT.h"

#warning "PFC_HAL.c need to be implemented according to the specific hardware platform"
// 下面关于PWM和ADC的函数需要根据具体的硬件平台进行实现，请自行修改

void PFC_PWM_UpdateDuty(void)
{
    
}

void PFC_HAL_setupADC(void)
{

    /*
     * ADC1-3 直流电压 / ADC2-4 直流电流
     * ADC2-5 交流电流 / ADC2-12 交流电压
     */

    // 校准ADC
    LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
    while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0) {
    }
    LL_ADC_StartCalibration(ADC2, LL_ADC_SINGLE_ENDED);
    while (LL_ADC_IsCalibrationOnGoing(ADC2) != 0) {
    }

    // 开启ADC
    LL_ADC_Enable(ADC1);
    while (LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0) {
    }
    LL_ADC_Enable(ADC2);
    while (LL_ADC_IsActiveFlag_ADRDY(ADC2) == 0) {
    }

    // 启动注入组
    LL_ADC_INJ_StartConversion(ADC1);
    LL_ADC_INJ_StartConversion(ADC2);

    // 启动注入组中断
    LL_ADC_EnableIT_JEOS(ADC2);
}

void PFC_ADC_Read(void)
{
    // 读取注入组的ADC值
    PFC_vAC_sensed_pu = (PFC_VAC_READ * PFC_ADC_PU_SCALE_FACTOR
                         - PFC_vAC_offset_pu) * 2.0f;
    PFC_iAC_sensed_pu = (PFC_VAC_READ * PFC_ADC_PU_SCALE_FACTOR
                         - PFC_iAC_offset_pu) * 2.0f;
    PFC_vDC_sensed_pu = (PFC_VDC_READ * PFC_ADC_PU_SCALE_FACTOR
                         - PFC_iAC_offset_pu);
    PFC_iDC_sensed_pu = (PFC_iDC_sensed_pu * PFC_ADC_PU_SCALE_FACTOR
                         - PFC_iAC_offset_pu) * 2.0f;

    // 适当的滤波
    
}

void PFC_HAL_setupPWM(void)
{
    // 设置COMP1
    // LL_HRTIM_TIM_SetCompare1(HRTIM1, PFC_PHASE_A_TIMER, 1000);  
    // LL_HRTIM_TIM_SetCompare1(HRTIM1, PFC_PHASE_B_TIMER, 1000);
    
    // 设置死区时间
    LL_HRTIM_DT_SetFallingValue(HRTIM1, PFC_PHASE_A_TIMER, 60); 
    LL_HRTIM_DT_SetRisingValue(HRTIM1, PFC_PHASE_A_TIMER, 60);

    LL_HRTIM_DT_SetFallingValue(HRTIM1, PFC_PHASE_B_TIMER, 60);
    LL_HRTIM_DT_SetRisingValue(HRTIM1, PFC_PHASE_B_TIMER, 60);

    // 输出使能
    // LL_HRTIM_EnableOutput(HRTIM1,    LL_HRTIM_OUTPUT_TB1
    //                                 |LL_HRTIM_OUTPUT_TB2
    //                                 |LL_HRTIM_OUTPUT_TC1
    //                                 |LL_HRTIM_OUTPUT_TC2);

    // 启动计数器
    LL_HRTIM_TIM_CounterEnable(HRTIM1,   PFC_PHASE_A_TIMER
                                        |PFC_PHASE_B_TIMER);
}