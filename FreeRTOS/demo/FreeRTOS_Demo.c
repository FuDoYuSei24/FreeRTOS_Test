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
/***************舵机任务的配置**********************/
#define SG90_STK_SIZE 512//任务栈的大小
#define SG90_TASK_PRIO 4//任务的优先级
TaskHandle_t SG90Task_Handler;//任务句柄
void SG90_task(void *pvParameters);
/***************LCD任务的配置**********************/
#define LCD_STK_SIZE 512//任务栈的大小
#define LCD_TASK_PRIO 3//任务的优先级
TaskHandle_t  LCDTask_Handler;//任务句柄
void LCD_task(void *pvParameters);
/***************识别卡任务的配置**********************/
#define RFID_STK_SIZE 512//任务栈的大小
#define RFID_TASK_PRIO 3//任务的优先级
TaskHandle_t RFIDTask_Handler;//任务句柄
void RFID_task(void *pvParameters);
/***************指纹识别任务的配置**********************/
#define AS608_STK_SIZE 512//任务栈的大小
#define AS608_TASK_PRIO 3//任务的优先级
TaskHandle_t AS608Task_Handler;//任务句柄
void AS608_task(void *pvParameters);
/***************ESP8266任务的配置**********************/
#define ESP8266_STK_SIZE 512//任务栈的大小
#define ESP8266_TASK_PRIO 3//任务的优先级
TaskHandle_t ESP8266Task_Handler;//任务句柄
void ESP8266_task(void *pvParameters);
/***************任务的配置**********************/
#define TASK1_STACK 512//任务栈的大小
#define TASK1_PRIORITY 2//任务的优先级
TaskHandle_t task1_handler;//任务句柄
void task1(void *pvParameters);


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

    //进入临界区：保护临界区里的代码不会被打断
    taskENTER_CRITICAL();//进入临界区

    //创建任务1
    xTaskCreate((TaskFunction_t)Task1,                    
                (char *)"Task1",                           
                (configSTACK_DEPTH_TYPE)TASK1_STACK_DEPTH, 
                (void *)NULL,                                   
                (UBaseType_t)TASK1_PRIORITY,               
                (TaskHandle_t *)&task1_handler             
               );

    //创建任务2
    xTaskCreate((TaskFunction_t)Task2,                    
                (char *)"Task2",                           
                (configSTACK_DEPTH_TYPE)TASK2_STACK_DEPTH, 
                (void *)NULL,                                   
                (UBaseType_t)TASK2_PRIORITY,               
                (TaskHandle_t *)&task2_handler             
               );
    


    //删除启动任务,因为启动任务的任务已经完成了,所以删除启动任务
    vTaskDelete(NULL);

    //退出临界段
    taskEXIT_CRITICAL();//退出临界区
}

//任务1的具体实现
void task1(void *pvParameters){

    uint8_t key = 0;
    while(1){
        key = Key_Delect();
        if(key==KEY1_PRESS){
            //key1按下，给bit0置1
            //参数：事件标志组句柄，要设置的值
            //返回值是更新后的结果
            xEventGroupSetBits(event_group_handle,EVENTBIT_0);
            printf("key1按下,bit0置1\r\n");
        }
        else if(key==KEY2_PRESS){
            //key2按下，给bit1置1
            xEventGroupSetBits(event_group_handle,EVENTBIT_1);
            printf("key1按下,bit1置1\r\n");
        }

        vTaskDelay(500);
    }
}

//任务2的具体实现
void task2(void *pvParameters){
    EventBits_t event_bits = 0;
     while(1){
        //等待事件标志组
        //参数：事件标志组句柄，等待哪一些比特位，
        // 退出时是否清除，是否等待所有标志位成立，
        // 是否进行阻塞地等待
        //返回时就是更新后的事件标志组
        event_bits = xEventGroupWaitBits(
            event_group_handle,
            EVENTBIT_0 | EVENTBIT_1,//用|连接
            pdTRUE,//表示退出时要进行清除
            pdTRUE,//表示都得满足
            portMAX_DELAY
        ); 
        printf("Task2接收到的事件标志组= %#x.....\r\n",event_bits);
    }
}

