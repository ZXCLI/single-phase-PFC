#ifndef PFC_HAL_H
#define PFC_HAL_H

#include "DCLF32.h"
#include "math.h"

void PFC_PWM_Init(void);
void PFC_PWM_UpdateDuty(void);
void PFC_ADC_Read(void);

#endif
