#include "PFC.h"
#include "PFC_parameter.h"
#include "PFC_setting.h"
#include "spll_1ph_sogi.h"
#include "DCLF32.h"
#include "rampgen.h"


SPLL_1PH_SOGI PFC_PLL;

// 归一化测量值
float PFC_iAC_sensed_pu;
float PFC_vAC_sensed_pu;
float PFC_vDC_sensed_pu;
float PFC_iDC_sensed_pu;
// 实际值
float PFC_iAC_sensed;
float PFC_vAC_sensed;
float PFC_vDC_sensed;
float PFC_iDC_sensed;

float PFC_vDC_Ref_pu;  //直流侧电压参考值
float PFC_iAC_Ref_pu;  //交流侧电流参考值，流入为正

void PFC_GlobalVariablesInit(void)
{

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

}