#ifndef __BASIC_TIM_H
#define	__BASIC_TIM_H

#include "stm32f4xx.h"

#define BASIC_TIM           		TIM6
#define BASIC_TIM_CLK       		RCC_APB1Periph_TIM6

#define BASIC_TIM_IRQn					TIM3_IRQn
#define BASIC_TIM_IRQHandler    TIM6_DAC_IRQHandler

uint8_t get_sec_tick_flag(void);             //��ȡ�����־
uint32_t get_t1s_tc_value(void);						//��ȡ�����ʱ��tick����
__weak void on_t100us_event(void);            //100us���ڴ�����
__weak void on_t1ms_event(void);             //1ms���ڴ�����
__weak void on_t1s_event(void);              //1s���ڴ�����

void TIM3_Int_Init(uint16_t arr,uint16_t psc);//arr10-1,psc840-1

#endif /* __BASIC_TIM_H */

