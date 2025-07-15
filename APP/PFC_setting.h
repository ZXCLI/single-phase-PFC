#ifndef PFC_SETTING_H
#define PFC_SETTING_H

#include "main.h"

#define PFC_SPLL_COEFF_B0 ((float32_t)166.974338481427f)
#define PFC_SPLL_COEFF_B1 ((float32_t) - 166.26611653716f)
#define PFC_SPLL_COEFF_K1 ((float32_t)0.00188141f)
#define PFC_SPLL_COEFF_K2 ((float32_t) - 0.99623717f)

// ADC的缩放系数
#define PFC_ADC_PU_SCALE_FACTOR ((float32_t)(1.0f / (1 << 12))) // 12bit

#define PFC_LAB 5

#endif
