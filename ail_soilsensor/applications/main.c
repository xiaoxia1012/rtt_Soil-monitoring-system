/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-5-10      ShiHao       first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG
#include <rtdbg.h>

char read_TemHumECPh[8] = {0x01,0x03,0x00,0x06,0x00,0x04,0xA4,0x08};//读取温湿度ec值和ph的命令
char read_NPK[8] = {0x01,0x03,0x00,0x1E,0x00,0x03,0x65,0xCD};//读取氮磷钾含量的命令

float sensor_data[7] = {0};//装载了七种传感器数据的数组

rt_device_t u3_dev;
struct rt_semaphore u3_rx_sem;
struct serial_configure u3_configs = RT_SERIAL_CONFIG_DEFAULT;
rt_thread_t u3_rx_th;
rt_thread_t u3_tx_th;

rt_err_t rx_callback(rt_device_t dev,rt_size_t size)
{
    rt_sem_release(&u3_rx_sem); //清除信号量
    return RT_EOK;
}

void serial_rx_thread_entry(void *parameter)
{
    int i = 0;
    char buffer;
    char rx_data[13] = {0};
    while(1)
    {
        while(rt_device_read(u3_dev, 0, &buffer, 1) != 1){
            rt_sem_take(&u3_rx_sem, RT_WAITING_FOREVER);    //等待信号量
        }
        rx_data[i] = buffer;    //接收的数据装进数组里
        i++;
        if (i == 11) {
            i = 0;
            if(rx_data[2] == 0x08){
                sensor_data[0] = (float)(rx_data[3] << 8 | rx_data[4]); //将温度值装进数组
                sensor_data[1] = (float)(rx_data[5] << 8 | rx_data[6]); //将湿度值装进数组
                sensor_data[2] = (float)(rx_data[7] << 8 | rx_data[8]); //将EC值装进数组
                sensor_data[3] = (float)(rx_data[9] << 8 | rx_data[10]); //将ph值装进数组
            }
            else if (rx_data[2] == 0x06) {                               //将氮磷钾含量值装进数组
                sensor_data[4] = (float)(rx_data[3] << 8 | rx_data[4]);
                sensor_data[5] = (float)(rx_data[5] << 8 | rx_data[6]);
                sensor_data[6] = (float)(rx_data[7] << 8 | rx_data[8]);
            }
        }

    }
}

void serial_tx_thread_entry(void *parameter)
{
    while(1)
    {
        rt_device_write(u3_dev, 0, read_TemHumECPh , 8);//发送读取温湿度ec值和ph的命令
        rt_thread_mdelay(1000);
        rt_device_write(u3_dev, 0, read_NPK , 8);//发送读取氮磷钾值的命令
        rt_thread_mdelay(1000);
    }


}
int main(void)
{
    rt_err_t ret = 0;
    u3_dev = rt_device_find("uart3");   //寻找串口3设备
    if(u3_dev == RT_NULL){
        LOG_E("rt_device_find[uart3] failed...\n");
        return -1;
    }
    ret = rt_device_open(u3_dev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX); //以读写和中断接收模式打开串口3
    if(ret < 0){
        LOG_E("rt_device_open[uart3] failed...\n");
        return ret;
    }
    rt_device_control(u3_dev, RT_DEVICE_CTRL_CONFIG, (void*)&u3_configs);   //配置串口3的参数

    rt_device_set_rx_indicate(u3_dev, rx_callback); //设置接收回调函数

    rt_sem_init(&u3_rx_sem, "u3_rx_sem", 0, RT_IPC_FLAG_FIFO);  //初始化接收信号量

    u3_rx_th = rt_thread_create("u3_recv", serial_rx_thread_entry, NULL, 1024, 10, 5);  //创建串口接收线程
    rt_thread_startup(u3_rx_th);        //开启线程

    u3_tx_th = rt_thread_create("u3_tx", serial_tx_thread_entry, NULL, 1024, 29, 5);    //创建串口发送线程
    rt_thread_startup(u3_tx_th);        //开启线程

    return RT_EOK;

}



