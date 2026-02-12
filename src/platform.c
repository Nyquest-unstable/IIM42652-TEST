#include "platform.h"
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

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