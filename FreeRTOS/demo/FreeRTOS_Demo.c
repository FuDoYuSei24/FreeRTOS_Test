#include "FreeRTOS_Demo.h"
#include "main.h"
//freertos必须的头文件
#include "FreeRTOS.h"
#include "task.h"
//需要用到的其他头文件
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include"LED.h"
#include "KEY.h"
#include "event_groups.h"


/***************启动任务的配置**********************/
#define START_STK_SIZE 512//任务栈的大小
#define START_TASK_PRIO 1//任务的优先级
TaskHandle_t StartTask_Handler;//任务句柄
void start_task(void *pvParameters);

/***************1-舵机任务的配置**********************/
#define SG90_STK_SIZE 512//任务栈的大小
#define SG90_TASK_PRIO 4//任务的优先级
TaskHandle_t SG90Task_Handler;//任务句柄
void SG90_task(void *pvParameters);

/***************2-LCD任务的配置**********************/
#define LCD_STK_SIZE 512//任务栈的大小
#define LCD_TASK_PRIO 3//任务的优先级
TaskHandle_t  LCDTask_Handler;//任务句柄
void LCD_task(void *pvParameters);

/***************3-识别卡任务的配置**********************/
#define RFID_STK_SIZE 512//任务栈的大小
#define RFID_TASK_PRIO 3//任务的优先级
TaskHandle_t RFIDTask_Handler;//任务句柄
void RFID_task(void *pvParameters);

/***************4-指纹识别任务的配置**********************/
#define AS608_STK_SIZE 512//任务栈的大小
#define AS608_TASK_PRIO 3//任务的优先级
TaskHandle_t AS608Task_Handler;//任务句柄
void AS608_task(void *pvParameters);

/***************5-ESP8266任务的配置**********************/
#define ESP8266_STK_SIZE 512//任务栈的大小
#define ESP8266_TASK_PRIO 3//任务的优先级
TaskHandle_t ESP8266Task_Handler;//任务句柄
void ESP8266_task(void *pvParameters);

/***************事件标志组的配置**********************/
EventGroupHandle_t EventGroupHandler;//事件标志组句柄
#define EVENTBIT_0	(1<<0)				//事件位
#define EVENTBIT_1	(1<<1)
#define EVENTBIT_2	(1<<2)
#define EVENTBIT_ALL	(EVENTBIT_0|EVENTBIT_1|EVENTBIT_2)

const  u8* kbd_menu[15]={"coded"," : ","lock","1","2","3","4","5","6","7","8","9","DEL","0","Enter",};//按键表
u8 sg90flag;
u8 rfidflag;
u8 key;
u8 err=0;

/*
*@brief  创建一个启动任务，并且启动调度器
*@param  无
*/

void freertos_start(void){

    //创建事件之前创建好事件标志组
    //无参数
    //返回值为事件标志组的句柄
    EventGroupHandler = xEventGroupCreate();
    if(EventGroupHandler != NULL){
        printf("事件标志组创建成功\r\n");
    }

    //1.创建一个启动任务
    xTaskCreate((TaskFunction_t)start_task,                     //1.1函数（任务）的地址
                (char *)"start_task",                           //1.2函数（任务）的名字
                (configSTACK_DEPTH_TYPE)START_STK_SIZE,         //1.3函数（任务）的栈大小
                (void *)NULL,                                   //1.4传递给函数（任务）的参数列表
                (UBaseType_t)START_TASK_PRIO,                   //1.5函数（任务）的优先级
                (TaskHandle_t *)&StartTask_Handler              //1.6函数（任务）的句柄
               );
    //2.启动调度器,会自动创建空闲任务
    vTaskStartScheduler();
}



void start_task(void *pvParameters){

    BaseType_t xReturn;
    //进入临界区：保护临界区里的代码不会被打断
    taskENTER_CRITICAL();//进入临界区

    //1-创建SG90任务
    xReturn = xTaskCreate((TaskFunction_t)SG90_task,                    
                (char *)"SG90_task",                           
                (configSTACK_DEPTH_TYPE)SG90_STK_SIZE, 
                (void *)NULL,                                   
                (UBaseType_t)SG90_TASK_PRIO,               
                (TaskHandle_t *)&SG90Task_Handler             
               );
    if(xReturn==pdPass){
        printf("SG90_TASK任务创建成功\r\n");
    }
    //2-创建LCD任务
    xReturn = xTaskCreate((TaskFunction_t)LCD_task,                    
                (char *)"LCD_task",                           
                (configSTACK_DEPTH_TYPE)LCD_STK_SIZE, 
                (void *)NULL,                                   
                (UBaseType_t)LCD_TASK_PRIO,               
                (TaskHandle_t *)&LCDTask_Handler             
               );
    if(xReturn==pdPass){
        printf("LCD_TASK任务创建成功\r\n");
    }
    //3-创建RFID识别卡任务
    xReturn = xTaskCreate((TaskFunction_t)RFID_task,                    
                (char *)"RFID_task",                           
                (configSTACK_DEPTH_TYPE)RFID_STK_SIZE, 
                (void *)NULL,                                   
                (UBaseType_t)RFID_TASK_PRIO,               
                (TaskHandle_t *)&RFIDTask_Handler             
               );
    if(xReturn==pdPass){
        printf("RFID_TASK任务创建成功\r\n");
    }
    //4-创建AS608指纹识别任务
    xReturn = xTaskCreate((TaskFunction_t)AS608_task,                    
                (char *)"AS608_task",                           
                (configSTACK_DEPTH_TYPE)AS608_STK_SIZE, 
                (void *)NULL,                                   
                (UBaseType_t)AS608_TASK_PRIO,               
                (TaskHandle_t *)&AS608Task_Handler             
               );
    if(xReturn==pdPass){
        printf("AS608_TASK任务创建成功\r\n");
    }
    //5-创建ESP8266任务
    xReturn = xTaskCreate((TaskFunction_t)ESP8266_task,                    
                (char *)"ESP8266_task",                           
                (configSTACK_DEPTH_TYPE)ESP8266_STK_SIZE, 
                (void *)NULL,                                   
                (UBaseType_t)ESP8266_TASK_PRIO,               
                (TaskHandle_t *)&ESP8266Task_Handler             
               );
    if(xReturn==pdPass){
        printf("ESP8266_TASK任务创建成功\r\n");
    }
    //删除启动任务,因为启动任务的任务已经完成了,所以删除启动任务
    vTaskDelete(NULL);

    //退出临界段
    taskEXIT_CRITICAL();//退出临界区
}

//舵机任务的具体实现
void SG90_task(void *pvParameters){
    volatile EventBits_t EventValue;
    while(1){
        EventValue = xEventGroupWaitBits(
            EventGroupHandler,//事件标志组句柄
            EVENTBIT_ALL,//等待哪些标志位，这里是全部
            pdTRUE,      //退出时进行清除
            pdFALSE,     //不需要都满足
            portMAX_DELAY
        );
        printf("接收事件成功\r\n");
        //舵机转动模拟开关门
        set_Angle(180);
        delay_xms(1000);
        delay_xms(1000);
        set_Angle(0);
        LCD_ShowString(80,150,260,16,16,"                  ");
        vTaskDelay(100);//延时10ms，也就是10个时钟节拍
    }
}

void LCD_task(void *pvParameters){
    while(1){

    }
}

void RFID_task(void *pvParameters){
    while(1){
        
    }
}

void AS608_task(void *pvParameters){
    while(1){

    }
}

void ESP8266_task(void *pvParameters){
    while(1){

    }
}
