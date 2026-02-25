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
    
    uint8_t tx_buf[256];
    // 临时缓冲区：1字节地址回读 + len字节数据
    uint8_t rx_buf[256];
    if (len + 1 > sizeof(rx_buf)) {
        close(fd);
        return INV_ERROR_SIZE;  // 需要定义这个错误码
    }
    tx_buf[0] = reg | 0x80;
    memset(tx_buf + 1, 0x00, len);
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx_buf,
        .rx_buf = (unsigned long)rx_buf,
        .len = len + 1, // 地址1字节 + 数据len字节
        .speed_hz = 500000, // SPI 速率（根据传感器手册调整，如 1MHz）
        .bits_per_word = 8,
        // .cs_change = 1,                     // 关键：发送地址后释放CS（避免时序错误）
        .delay_usecs = 0,
    };

    int rc = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    
    // 调试打印
    printf("SPI Read - Reg: 0x%02X, Len: %u\n", reg, len);
    // printf("TX: %02X\n", tx_buf[0]);
    
    // 或者明确标注：
    printf("RX raw[0]=0x%02X (discard), data: ", rx_buf[0]);
    for (uint32_t i = 1; i < len + 1; i++) {
        printf("%02X ", rx_buf[i]);
    }
    printf("\n");

    close(fd);

    if (rc < 0)
        return INV_ERROR_IO;

    // 关键：跳过第1字节（地址回读），复制有效数据到buf
    memcpy(buf, rx_buf + 1, len);

    return INV_ERROR_SUCCESS;
}

extern imu_spi_cfg_t imu_spi_cfg_X6;
#define s_imu_spi_cfg imu_spi_cfg_X6

void spi_write_read_data(int fd, void *tx_data, void *rx_data, int len)
{
    int ret;
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx_data,
        .rx_buf = (unsigned long)rx_data,
        .len = len,
        .delay_usecs = s_imu_spi_cfg.delay,
        .speed_hz = s_imu_spi_cfg.speed,
        .bits_per_word = s_imu_spi_cfg.bits,
    };
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret == 1)
        printf("can't send spi message");
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
        .speed_hz = 500000,
        .bits_per_word = 8,
    };

    int rc = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    close(fd);

    return (rc < 0) ? INV_ERROR_IO : INV_ERROR_SUCCESS;
}

void pabort(const char *s)
{
    perror(s);
    // abort();
}

int spi_open(imu_spi_cfg_t *pstSpiCfg)
{
    int fd, ret;
    fd = open(pstSpiCfg->device, O_RDWR);
    if (fd < 0)
    {
        pabort("can't open device");
        return -1;
    }
    pstSpiCfg->fd = fd;
    printf("set spi mode: %d\n", pstSpiCfg->mode);
    printf("set bits per word: %d\n", pstSpiCfg->bits);
    printf("set speed: %d Hz (%d KHz)\n", pstSpiCfg->speed, pstSpiCfg->speed / 1000);
    /*
     * spi mode
     */
    ret = ioctl(pstSpiCfg->fd, SPI_IOC_WR_MODE, &pstSpiCfg->mode);
    if (ret == -1)
    {
        pabort("can't set spi mode");
        return -1;
    }
    ret = ioctl(pstSpiCfg->fd, SPI_IOC_RD_MODE, &pstSpiCfg->mode);
    if (ret == -1)
        pabort("can't get spi mode");
    /*
     * bits per word
     */
    ret = ioctl(pstSpiCfg->fd, SPI_IOC_WR_BITS_PER_WORD, &pstSpiCfg->bits);
    if (ret == -1)
    {
        pabort("can't set bits per word");
        return -1;
    }

    ret = ioctl(pstSpiCfg->fd, SPI_IOC_RD_BITS_PER_WORD, &pstSpiCfg->bits);
    if (ret == -1)
        pabort("can't get bits per word");
    /*
     * max speed hz
     */
    ret = ioctl(pstSpiCfg->fd, SPI_IOC_WR_MAX_SPEED_HZ, &pstSpiCfg->speed);
    if (ret == -1)
    {
        pabort("can't set max speed hz");
        return -1;
    }
    ret = ioctl(pstSpiCfg->fd, SPI_IOC_RD_MAX_SPEED_HZ, &pstSpiCfg->speed);
    if (ret == -1)
        pabort("can't get max speed hz");

    printf("get spi mode: %d\n", pstSpiCfg->mode);
    printf("get bits per word: %d\n", pstSpiCfg->bits);
    printf("get max speed: %d Hz (%d KHz)\n", pstSpiCfg->speed, pstSpiCfg->speed / 1000);
    return fd;
}
