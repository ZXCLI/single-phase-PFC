#ifndef PFC_SETTING_H
#define PFC_SETTING_H

#include "main.h"

#define PFC_SPLL_COEFF_B0 ((float32_t)166.974338481427f)
#define PFC_SPLL_COEFF_B1 ((float32_t) - 166.26611653716f)
#define PFC_SPLL_COEFF_K1 ((float32_t)0.00188141f)
#define PFC_SPLL_COEFF_K2 ((float32_t) - 0.99623717f)

#define PFC_VAC_MAX ((float32_t)40.0f)
#define PFC_IAC_MAX ((float32_t)10.0f)
#define PFC_VDC_MAX ((float32_t)40.0f)
#define PFC_IDC_MAX ((float32_t)10.0f)

#define PFC_VDC_OVERVOLT_LIMIT ((float32_t)45.0f)
#define PFC_IDC_OVERVOLT_LIMIT ((float32_t)12.0f)
#define PFC_VAC_RMS_OVERVOLT_LIMIT ((float32_t)45.0f)
#define PFC_IAC_RMS_OVERVOLT_LIMIT ((float32_t)12.0f)
#define PFC_VAC_RMS_UNDERVOLT_LIMIT ((float32_t)20.0f)

// ADC的缩放系数
#define PFC_ADC_PU_SCALE_FACTOR ((float32_t)(1.0f / (1 << 12))) // 12bit

// 低通滤波
#define PFC_AC_FILTER_CONSTANT ((float32_t)0.1534f) // Ts x 2 x pi x fcut Cut off frequency of 12 kHz
#define PFC_DC_FILTER_CONSTANT ((float32_t)0.1534f)

#define PFC_LAB 5

#endif
