#include "platform.h"
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

void inv_ixm42xxx_sleep_us(unsigned int us)
{
#ifdef _WIN32
    Sleep(us / 1000);  // Sleep takes milliseconds on Windows
#else
    usleep(us);  // usleep takes microseconds on Linux/Unix
#endif
}

uint64_t inv_ixm42xxx_get_time_us(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec;
}

void inv_helper_disable_irq(void)
{
    // Platform-specific interrupt disabling
    // For a simple application on desktop OS, we can ignore this
}

void inv_helper_enable_irq(void)
{
    // Platform-specific interrupt enabling
    // For a simple application on desktop OS, we can ignore this
}


// SPI 底层读函数（匹配 inv_ixm42xxx_serif 的函数签名）
int platform_spi_read(struct inv_ixm42xxx_serif *serif, uint8_t reg, uint8_t *buf, uint32_t len) {
    if (!serif || !buf || len == 0) return INV_ERROR_INVALID_PARAMETER;

    int fd = open((char*)serif->context, O_RDWR);
    if (fd < 0) return INV_ERROR_IO;

    // IIM42652 SPI 读规则：寄存器地址需置位最高位（0x80）
    uint8_t tx_buf[1] = {reg | 0x80};
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx_buf,
        .rx_buf = (unsigned long)buf,
        .len = len + 1, // 地址1字节 + 数据len字节
        .speed_hz = 1000000, // SPI 速率（根据传感器手册调整，如 1MHz）
        .bits_per_word = 8,
        .cs_change = 0,
    };

    int rc = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    close(fd);

    return (rc < 0) ? INV_ERROR_IO : INV_ERROR_SUCCESS;
}

// SPI 底层写函数
int platform_spi_write(struct inv_ixm42xxx_serif *serif, uint8_t reg, const uint8_t *buf, uint32_t len) {
    if (!serif || !buf || len == 0) return INV_ERROR_INVALID_PARAMETER;

    int fd = open((char*)serif->context, O_RDWR);
    if (fd < 0) return INV_ERROR_IO;

    // 拼接「寄存器地址 + 数据」
    uint8_t tx_buf[len + 1];
    tx_buf[0] = reg & 0x7F; // 写地址：最高位清0
    memcpy(&tx_buf[1], buf, len);

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx_buf,
        .rx_buf = (unsigned long)NULL,
        .len = len + 1,
        .speed_hz = 1000000,
        .bits_per_word = 8,
        .cs_change = 0,
    };

    int rc = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    close(fd);

    return (rc < 0) ? INV_ERROR_IO : INV_ERROR_SUCCESS;
}
