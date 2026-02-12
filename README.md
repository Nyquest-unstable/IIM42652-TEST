# IIM42652 六轴运动传感器测试项目

本项目是一个基于InvenSense IIM42652六轴运动传感器（3轴加速度计+3轴陀螺仪）的测试应用，用于验证传感器的基本功能。

## 简介

IIM42652是一款高性能、低功耗的6轴MEMS运动传感器，集成了3轴加速度计和3轴陀螺仪。该传感器广泛应用于消费电子、物联网设备和其他需要运动检测的应用中。

本项目集成了InvenSense官方提供的驱动库，实现了传感器的基本初始化、配置和数据读取功能。

## 功能特性

- 传感器初始化和WHOAMI验证
- 加速度计和陀螺仪配置（量程和输出频率设置）
- 低噪声模式启用
- FIFO数据读取和处理
- 温度数据读取

## 项目结构

```
IIM42652/
├── public.mcu.iim42652/          # 官方IIM42652驱动库
│   ├── Ixm42xxx/                 # 核心驱动代码
│   └── examples/                 # 示例代码
├── src/                          # 项目源码
│   ├── main.cpp                  # 主程序入口
│   ├── platform.c                # 平台相关实现
│   └── platform.h                # 平台相关头文件
├── xmake.lua                     # XMake构建配置
└── README.md                     # 项目说明文档
```

## 依赖项

- [XMake](https://xmake.io) - 跨平台构建工具
- InvenSense IIM42652官方驱动库 (已包含在项目中)

## 构建方法

### 1. 环境要求

- XMake 构建系统
- GCC 或 Clang 编译器

### 2. 构建步骤

```bash
# 进入项目目录
cd IIM42652

# 执行构建
xmake

# 构建完成后，可执行文件位于
./build/linux/x86_64/release/IIM42652
```

## 使用说明

### 桌面测试

在桌面环境中运行（仅用于验证代码框架，无法访问实际硬件）：

```bash
xmake run
```

或直接执行生成的可执行文件：

```bash
./build/linux/x86_64/release/IIM42652
```

注意：在桌面环境中，程序会因为无法访问实际硬件而初始化失败，这是正常现象。

### 嵌入式平台部署

要在实际的嵌入式平台上运行，需要完成以下步骤：

1. 实现SPI或I2C传输接口函数：

   在[main.cpp](file:///home/zc/xmake/IIM42652/src/main.cpp)中，需要实现`serif.read_reg`和`serif.write_reg`函数指针，
   以完成对传感器的实际读写操作。

2. 连接硬件电路：

   将MCU的SPI/I2C接口与IIM42652传感器正确连接。

3. 适配平台函数：

   确保[platform.c](file:///home/zc/xmake/IIM42652/src/platform.c)中的延时和时间戳函数适用于目标平台。

## 配置选项

- 加速度计量程：±16g
- 陀螺仪量程：±2000 dps
- 加速度计输出频率：50Hz
- 陀螺仪输出频率：50Hz
- 启用低噪声模式

这些配置可以在[main.cpp](file:///home/zc/xmake/IIM42652/src/main.cpp)中的`ConfigureInvDevice`部分进行修改。

## API说明

### 传感器初始化

```c
struct inv_ixm42xxx_serif serif;
// 需要实现实际的读写函数
serif.read_reg = your_spi_read_func;
serif.write_reg = your_spi_write_func;

int rc = inv_ixm42xxx_init(&sensor_driver, &serif, handle_fifo_data);
```

### 传感器配置

- `inv_ixm42xxx_set_accel_fsr()` - 设置加速度计量程
- `inv_ixm42xxx_set_gyro_fsr()` - 设置陀螺仪量程
- `inv_ixm42xxx_set_accel_frequency()` - 设置加速度计输出频率
- `inv_ixm42xxx_set_gyro_frequency()` - 设置陀螺仪输出频率
- `inv_ixm42xxx_enable_accel_low_noise_mode()` - 启用加速度计低噪声模式
- `inv_ixm42xxx_enable_gyro_low_noise_mode()` - 启用陀螺仪低噪声模式

## 注意事项

1. 本项目在桌面环境下主要用于验证代码框架，在实际硬件上才能完成完整功能测试。
2. 传感器与MCU之间的通信接口（SPI/I2C）需要根据硬件设计进行相应配置。
3. 电源管理配置需要参考IIM42652数据手册进行优化。

## 参考资料

- [IIM42652产品页面](https://www.invensense.com/products/motion-tracking/6-axis/iim42652/)
- [官方驱动库](https://github.com/InvenSenseInc/public.mcu.iim42652)
- [IIM42652数据手册](https://3cfeqx1hf82y3xcoull08ihx-wpengine.netdna-ssl.com/wp-content/uploads/2021/06/DS-000185-ICM-42652-v1.0.pdf)# IIM42652-TEST
