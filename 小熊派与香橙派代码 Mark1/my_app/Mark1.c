/*
 * Copyright (c) 2020 Nanjing Xiaoxiongpai Intelligent Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifiiot_errno.h"
#include "wifiiot_gpio.h"
#include "wifiiot_gpio_ex.h"
#include "wifiiot_i2c.h"
#include "wifiiot_i2c_ex.h"

int a = 0;
int b = 0;

unsigned int DACwrite(unsigned char *senddata, unsigned int datalen)
{
    unsigned int result;
    WifiIotI2cData DACdata = {0};//结构体，详见头文件wifiiot_i2c_ex.h
    DACdata.sendBuf=senddata;
    DACdata.sendLen=datalen;
    result=I2cWrite(WIFI_IOT_I2C_IDX_0, 0xc0, &DACdata);//0xc0为mcp4725的设备地址。注意设备码为7bit，地址为设备码+1位R/W比特，0xC0代表“写”操作。
    printf("Data write complete!\r\n");
    return result;
}
static void F1_Pressed(char *arg)
{
    (void)arg;
    printf("F1 Pressed!");
}
static void F2_Pressed(char *arg)
{
    (void)arg;
    printf("F2 Pressed!");
}
static void GPIO9RISE(char *arg)
{
    (void)arg;
    GpioSetOutputVal(WIFI_IOT_IO_NAME_GPIO_2, 1);
    a++;
    if(a == 60000)
    {
        b++;
        a=0;
    }
    printf("%d,%d\r\n",b,a);
    GpioSetIsrMask(WIFI_IOT_IO_NAME_GPIO_9,1);
    for(int j=0;j<1000;j++)
    {
        usleep(1);
    }
    GpioSetOutputVal(WIFI_IOT_IO_NAME_GPIO_2, 0);
    GpioSetIsrMask(WIFI_IOT_IO_NAME_GPIO_9,0);
}
/*static void GPIO10RISE(char *arg)
{
    (void)arg;
    a=0;
    b=0;
    printf("%d,%d\r\n",b,a);
    GpioSetIsrMask(WIFI_IOT_IO_NAME_GPIO_10,1);
    for(int j=0;j<1000;j++)
    {
        usleep(1);
    }
    GpioSetOutputVal(WIFI_IOT_IO_NAME_GPIO_2, 0);
    GpioSetIsrMask(WIFI_IOT_IO_NAME_GPIO_10,0);
}*/
static void ButtonExampleEntry(void)
{
    printf("Starts!");
    GpioInit();


    //初始化LED灯，在屏幕不点亮时，可以观察板子上LED闪动得知粒子入射
    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_2, WIFI_IOT_IO_FUNC_GPIO_2_GPIO);

    GpioSetDir(WIFI_IOT_IO_NAME_GPIO_2, WIFI_IOT_GPIO_DIR_OUT);

    //初始化F1按键，设置为下降沿触发中断
    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_11, WIFI_IOT_IO_FUNC_GPIO_11_GPIO);

    GpioSetDir(WIFI_IOT_IO_NAME_GPIO_11, WIFI_IOT_GPIO_DIR_IN);
    IoSetPull(WIFI_IOT_IO_NAME_GPIO_11, WIFI_IOT_IO_PULL_UP);
    GpioRegisterIsrFunc(WIFI_IOT_IO_NAME_GPIO_11, WIFI_IOT_INT_TYPE_EDGE, WIFI_IOT_GPIO_EDGE_FALL_LEVEL_LOW, F1_Pressed, NULL);

    //初始化F2按键，设置为下降沿触发中断
    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_12, WIFI_IOT_IO_FUNC_GPIO_12_GPIO);

    GpioSetDir(WIFI_IOT_IO_NAME_GPIO_12, WIFI_IOT_GPIO_DIR_IN);
    IoSetPull(WIFI_IOT_IO_NAME_GPIO_12, WIFI_IOT_IO_PULL_UP);
    GpioRegisterIsrFunc(WIFI_IOT_IO_NAME_GPIO_12, WIFI_IOT_INT_TYPE_EDGE, WIFI_IOT_GPIO_EDGE_FALL_LEVEL_LOW, F2_Pressed, NULL);

    //外接触发GPIO9，用于接收来自模拟电路的信号
    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_9, WIFI_IOT_IO_FUNC_GPIO_9_GPIO);

    GpioSetDir(WIFI_IOT_IO_NAME_GPIO_9, WIFI_IOT_GPIO_DIR_IN);
    GpioRegisterIsrFunc(WIFI_IOT_IO_NAME_GPIO_9, WIFI_IOT_INT_TYPE_EDGE, WIFI_IOT_GPIO_EDGE_RISE_LEVEL_HIGH, GPIO9RISE, NULL);
    
    //外接触发GPIO10, 用于接收来自香橙派的重置信号，此方案因地线接触不良暂时搁置
    //IoSetFunc(WIFI_IOT_IO_NAME_GPIO_10, WIFI_IOT_IO_FUNC_GPIO_10_GPIO);

    //GpioSetDir(WIFI_IOT_IO_NAME_GPIO_10, WIFI_IOT_GPIO_DIR_IN);
    //GpioRegisterIsrFunc(WIFI_IOT_IO_NAME_GPIO_10, WIFI_IOT_INT_TYPE_EDGE, WIFI_IOT_GPIO_EDGE_RISE_LEVEL_HIGH, GPIO10RISE, NULL);
    
    //设置GPIO13，GPIO14分别为I2C0的SDA，SCL接口。
    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_13, WIFI_IOT_IO_FUNC_GPIO_13_I2C0_SDA);
    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_14, WIFI_IOT_IO_FUNC_GPIO_14_I2C0_SCL);
    
    //串口初始化
    I2cInit(WIFI_IOT_I2C_IDX_0, 400000);
    I2cSetBaudrate(WIFI_IOT_I2C_IDX_0, 400000);//这行命令可以在程序运行时实时调整通讯速率。
    //设置比较电平
    printf("Init succesffuly!");
    unsigned char data_h=0x01;//高8位：00xxyyyy，两个x代表power模式，4个y构成12bit的前4位。
    unsigned char data_l=0x00;//低8位：yyyyyyyy，8个y构成12bit的后4位。
    
    //参考电平计算方式：Vout=(Vref*Data)/4096 此处的Vref对于BearPi来说是3.3V。
    unsigned char output[2]={data_h,data_l};//DACwrite函数的输入为指针。
    DACwrite(output,sizeof(output));
}

APP_FEATURE_INIT(ButtonExampleEntry);