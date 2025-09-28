/**
  ******************************************************************************
  * @file    bsp_basic_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ������ʱ����ʱ����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "bsp_basic_tim.h"

/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"

static		uint16_t t1s;
static 		uint8_t  ucTime_1s_flag = 0;
static 		__IO uint32_t TimingDelay;


 /**
  * @brief  ������ʱ�� TIMx,x[6,7]�ж����ȼ�����
  * @param  ��
  * @retval ��
  */
//static void TIMx_NVIC_Configuration(void)
//{
//    NVIC_InitTypeDef NVIC_InitStructure; 
//    // �����ж���Ϊ0
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);		
//		// �����ж���Դ
//    NVIC_InitStructure.NVIC_IRQChannel = BASIC_TIM_IRQn; 	
//		// ������ռ���ȼ�
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;	 
//	  // ���������ȼ�
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//}

/*
 * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
 * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
 * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         ����
 * TIM_CounterMode			 TIMx,x[6,7]û�У��������У�������ʱ����
 * TIM_Period            ����
 * TIM_ClockDivision     TIMx,x[6,7]û�У���������(������ʱ��)
 * TIM_RepetitionCounter TIMx,x[1,8]����(�߼���ʱ��)
 *-----------------------------------------------------------------------------
 */


void TIM3_Int_Init(u16 arr,u16 psc)//arr10-1,psc840-1
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///ʹ��TIM3ʱ��
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���¼���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//��ʼ��TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��3
	TIM_Cmd(TIM2,ENABLE); //ʹ�ܶ�ʱ��2
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=4; //��ռ���ȼ�4
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0; //�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}




void TIM3_IRQHandler(void)
{
	uint32_t ulReturn;
	ulReturn=taskENTER_CRITICAL_FROM_ISR();//�����ٽ��
	if ( TIM_GetITStatus(TIM3,TIM_IT_Update)==SET ) 
	{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
		static uint16_t usTimeCnt_1ms = 0;
		static uint8_t  ucTimeCnt_100us = 0;
	
		if (TimingDelay != 0x00) 
     { 
       TimingDelay--;
     }
	
		t1s++;
		if(t1s > 10000)
			t1s=0;
         	
		ucTimeCnt_100us++;
		if(10 == ucTimeCnt_100us)
		{
			ucTimeCnt_100us = 0;
			usTimeCnt_1ms++;
			if(1000 == usTimeCnt_1ms)
			{
				usTimeCnt_1ms = 0;
				ucTime_1s_flag = 1;
			
				on_t1s_event();
			}
			on_t1ms_event();
		}
		on_t100us_event();
		
	}
		taskEXIT_CRITICAL_FROM_ISR(ulReturn);//�˳��ٽ��
}	


/*****************************************************************
  * �������ƣ�   ��ȡ�����־
  * ��    ����   ��
  * �� �� ֵ��   0 : δ���룻1���ѹ��� 
  *	ע    �⣺	 
******************************************************************/
uint8_t get_sec_tick_flag(void)
{
	if(ucTime_1s_flag)
	{
		ucTime_1s_flag = 0;
		return 1;
	}
	return 0;
}



/*****************************************************************
  * �������ƣ�   ��ȡ�����ʱ��tick����
  * ��    ����   ��
  * �� �� ֵ��   �����ʱ��tick����
  *	ע    �⣺	
******************************************************************/
uint32_t get_t1s_tc_value(void)
{
	return t1s;
}


/*****************************************************************
  * �������ƣ�   100us���ڴ�����
  * ��    ����   ��
  * �� �� ֵ��   ��
  *	ע    �⣺	1�������ʱ���жϵ��ã������庯��
******************************************************************/
__weak void on_t100us_event(void)
{
	return;
}


/*****************************************************************
  * �������ƣ�   1ms���ڴ�����
  * ��    ����   ��
  * �� �� ֵ��   ��
  *	ע    �⣺	1�������ʱ���жϵ��ã������庯�����ⲿ���¶���
******************************************************************/
__weak void on_t1ms_event(void)
{
	return;
}
/*********************************************END OF FILE**********************/
