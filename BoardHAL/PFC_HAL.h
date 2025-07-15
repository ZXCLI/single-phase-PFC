#ifndef PFC_HAL_H
#define PFC_HAL_H

#include "DCLF32.h"
#include "math.h"

#define PFC_PHASE_A_TIMER LL_HRTIM_TIMER_B
#define PFC_PHASE_B_TIMER LL_HRTIM_TIMER_C

#define PFC_VAC_READ (float)LL_ADC_INJ_ReadConversionData12(ADC2, LL_ADC_INJ_RANK_3)
#define PFC_IAC_READ (float)LL_ADC_INJ_ReadConversionData12(ADC2, LL_ADC_INJ_RANK_2)
#define PFC_VDC_READ (float)LL_ADC_INJ_ReadConversionData12(ADC1, LL_ADC_INJ_RANK_1)

void PFC_PWM_UpdateDuty(void);
void PFC_HAL_setupADC(void);
void PFC_HAL_setupPWM(void);

#endif
