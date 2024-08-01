# 项目简介

本项目基于RT-thread操作系统，采用先进的**七合一土壤传感器**收集土壤数据，并通过**RS485总线**将数据传输至**星火一号主控板**。该主控板通过**WIFI或4G网络**，将数据实时上传至**阿里云平台**，实现用户对土壤状况的实时监控，促进农田或土地的科学管理。

# 开源代码与组件使用情况说明

## 系统硬件
- **主控板**：采用睿赛德公司提供的星火一号开发板。
- **传感器**：集成温湿度、pH值、电导率、氮磷钾等数据的七合一土壤数据传感器。
- **通讯模块**：板载RW007 WiFi模块。

## 系统软件
- **操作系统**：底层硬件上搭载RT-thread操作系统的4.1.1版本。
- **软件包**：运用RT官方提供的RW007软件包、ali-iotkit软件包以及cJSON软件包。
- **数据传输**：通过UART外设进行。

## 云平台
- **平台选择**：采用阿里云平台。
- **平台优势**：提供设备物模型、数据可视化等组件，为项目提供直观的数据展示平台。

# 项目安装说明

1. 将七合一土壤传感器的**VCC**和**GND**连接至开发板的5V与GND，为传感器供电。
2. 将传感器的**RS485A**和**RS485B**连接至开发板的RS485接口，用于数据传输。
3. 在终端启动WIFI连接程序和阿里云连接程序。
4. 将传感器埋入距地面20至30公分的土壤中。
5. 打开阿里云平台，即可观察到传感器上报的数据。

# 设计思路

1. **数据采集分析**：确定采集的土壤数据类型，由七合一土壤传感器负责采集并通过RS485总线传输。
2. **数据传输设计**：主控自检设备后开始采集数据，经传感器采集后传输至主控处理，再通过WiFi/4G模块发送至云平台。
3. **程序设计**：利用RT-thread实时操作系统创建多个线程，分别负责数据采集、网络连接和数据传输等任务，实现线程间通信。
4. **硬件升级与网络通信**：预留其他传感器接口，具备网络通信功能，支持远程OTA系统升级。

# 设计重点难点

1. **环境适应性**：鉴于野外工作环境，设备需具备高防水防尘性能（IP68等级），并设计休眠唤醒功能以节省能源。
2. **数据报文解析**：传感器使用RS485协议和工业modbus总线报文，需设计算法提取有效数据。
3. **网络协议实现**：通过MQTT协议连接云平台，需填写设备三元组信息，并利用订阅、属性上报等命令更新数据。

