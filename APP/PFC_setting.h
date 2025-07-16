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

#define PFC_VOLTS_PER_SECOND_SLEW ((float32_t)15.0f)

// ADC的缩放系数
#define PFC_ADC_PU_SCALE_FACTOR ((float32_t)(1.0f / (1 << 12))) // 12bit

// 低通滤波
#define PFC_AC_FILTER_CONSTANT ((float32_t)0.1534f) // Ts x 2 x pi x fcut Cut off frequency of 12 kHz
#define PFC_DC_FILTER_CONSTANT ((float32_t)0.1534f)


// lab说明：
// lab1：开环发波，用于测试驱动是否正常，下管给的占空比很低。不建议在交流或者直流端口施加电压，如果要加电，建议先测试直流，自行计算负载大小
// lab2：半桥关闭，交流侧接入交流电，直流侧接入合适的负载。此时是被动整流模式，用于测试采样和锁相环是否正常。
// lab3：直流侧接入电源，交流测接入适当负载，开环发SPWM波。用于测试PWM以及采样极性。
// lab4：PFC模式，但是只运行电流环，请在直流侧接入合适的负载。用于调试电流环。
// lab5：电压电流双闭环，正常运行。
#define PFC_LAB 2

#endif
