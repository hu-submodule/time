/**
 * @file      hs_time.c
 * @brief     时间模块源文件
 * @author    huenrong (sgyhy1028@outlook.com)
 * @date      2026-02-01 14:29:36
 *
 * @copyright Copyright (c) 2026 huenrong
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <linux/limits.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>

#include "hs_time.h"

/**
 * @brief 将 date_time_t 类型的时间转换为 struct tm 类型的时间
 *
 * @param[out] system_time: 转换后的 struct tm 类型的时间
 * @param[in]  date_time  : 待转换的 date_time_t 类型的时间
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
static int hs_time_date_time_to_tm(struct tm *system_time, const hs_date_time_t *date_time)
{
    if (system_time == NULL)
    {
        return -1;
    }

    if (date_time == NULL)
    {
        return -2;
    }

    system_time->tm_year = date_time->tm_year - 1900;
    system_time->tm_mon = date_time->tm_mon - 1;
    system_time->tm_mday = date_time->tm_mday;
    system_time->tm_hour = date_time->tm_hour;
    system_time->tm_min = date_time->tm_min;
    system_time->tm_sec = date_time->tm_sec;
    system_time->tm_yday = date_time->tm_yday - 1;
    system_time->tm_wday = date_time->tm_wday == 7 ? 0 : date_time->tm_wday;

    return 0;
}

/**
 * @brief 将 struct tm 类型的时间转换为 date_time_t 类型的时间
 *
 * @param[out] date_time  : 转换后的 date_time_t 类型的时间
 * @param[in]  system_time: 待转换的 struct tm 类型的时间
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
static int hs_time_tm_to_date_time(hs_date_time_t *date_time, const struct tm *system_time)
{
    if (date_time == NULL)
    {
        return -1;
    }

    if (system_time == NULL)
    {
        return -2;
    }

    date_time->tm_year = system_time->tm_year + 1900;
    date_time->tm_mon = system_time->tm_mon + 1;
    date_time->tm_mday = system_time->tm_mday;
    date_time->tm_hour = system_time->tm_hour;
    date_time->tm_min = system_time->tm_min;
    date_time->tm_sec = system_time->tm_sec;
    date_time->tm_yday = system_time->tm_yday + 1;
    date_time->tm_wday = system_time->tm_wday == 0 ? 7 : system_time->tm_wday;

    return 0;
}

/**
 * @brief 将 date_time_t 类型的时间转换为 struct rtc_time 类型的时间
 *
 * @param[out] rtc_tm   : 转换后的 struct rtc_time 类型的时间
 * @param[in]  date_time: 待转换的 date_time_t 类型的时间
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
static int hs_time_date_time_to_rtc_time(struct rtc_time *rtc_tm, const hs_date_time_t *date_time)
{
    if (rtc_tm == NULL)
    {
        return -1;
    }

    if (date_time == NULL)
    {
        return -2;
    }

    rtc_tm->tm_year = date_time->tm_year - 1900;
    rtc_tm->tm_mon = date_time->tm_mon - 1;
    rtc_tm->tm_mday = date_time->tm_mday;
    rtc_tm->tm_hour = date_time->tm_hour;
    rtc_tm->tm_min = date_time->tm_min;
    rtc_tm->tm_sec = date_time->tm_sec;
    rtc_tm->tm_yday = date_time->tm_yday - 1;
    rtc_tm->tm_wday = date_time->tm_wday == 7 ? 0 : date_time->tm_wday;

    return 0;
}

/**
 * @brief 将 struct rtc_time 类型的时间转换为 date_time_t 类型的时间
 *
 * @param[out] date_time: 转换后的 date_time_t 类型的时间
 * @param[in]  rtc_tm   : 待转换的 struct rtc_time 类型的时间
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
static int hs_time_rtc_time_to_date_time(hs_date_time_t *date_time, const struct rtc_time *rtc_tm)
{
    if (date_time == NULL)
    {
        return -1;
    }

    if (rtc_tm == NULL)
    {
        return -2;
    }

    date_time->tm_year = rtc_tm->tm_year + 1900;
    date_time->tm_mon = rtc_tm->tm_mon + 1;
    date_time->tm_mday = rtc_tm->tm_mday;
    date_time->tm_hour = rtc_tm->tm_hour;
    date_time->tm_min = rtc_tm->tm_min;
    date_time->tm_sec = rtc_tm->tm_sec;
    date_time->tm_yday = rtc_tm->tm_yday + 1;
    date_time->tm_wday = rtc_tm->tm_wday == 0 ? 7 : rtc_tm->tm_wday;

    return 0;
}

int hs_time_get_compile_date_time(char *date_time, const size_t len)
{
    if (date_time == NULL)
    {
        return -1;
    }

    if (len <= 0)
    {
        return -2;
    }

    const char month[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    char compile_date[20] = {0};
    char compile_time[20] = {0};
    char month_str[4] = {0};
    int year = 0;
    int mon = 0;
    int day = 0;
    int hour = 0;
    int min = 0;
    int sec = 0;

    snprintf(compile_date, sizeof(compile_date), "%s", __DATE__);
    snprintf(compile_time, sizeof(compile_time), "%s", __TIME__);

    sscanf(compile_date, "%s %02d %04d", month_str, &day, &year);
    sscanf(compile_time, "%02d:%02d:%02d", &hour, &min, &sec);

    for (uint8_t i = 0; i < 12; ++i)
    {
        if (strncmp(month_str, month[i], 3) == 0)
        {
            mon = i + 1;

            break;
        }
    }

    snprintf(date_time, len, "%04d-%02d-%02d %02d:%02d:%02d", year, mon, day, hour, min, sec);

    return 0;
}

int hs_time_get_system_timezone(char *timezone, const size_t len)
{
    if (timezone == NULL)
    {
        return -1;
    }

    if (len <= 0)
    {
        return -2;
    }

    char link[PATH_MAX];
    ssize_t ret = readlink("/etc/localtime", link, sizeof(link) - 1);
    if (ret < 0)
    {
        return -3;
    }
    link[ret] = '\0';

    // 获取时区字段起始位置
    const char *timezone_start = strstr(link, "zoneinfo/");
    if (timezone_start == NULL)
    {
        return -4;
    }
    timezone_start += strlen("zoneinfo/");

    size_t timezone_len = strlen(timezone_start);
    if (timezone_len >= len)
    {
        return -5;
    }

    memset(timezone, 0, len);
    strncpy(timezone, timezone_start, timezone_len);
    timezone[timezone_len] = '\0';

    return 0;
}

int hs_time_set_system_timezone(const char *timezone)
{
    if (timezone == NULL)
    {
        return -1;
    }

    char zoneinfo_path[PATH_MAX] = "";
    snprintf(zoneinfo_path, sizeof(zoneinfo_path), "/usr/share/zoneinfo/%s", timezone);
    // 没有这个时区文件
    if (access(zoneinfo_path, F_OK) != 0)
    {
        return -2;
    }

    // 删除软链接, 如果不存在, 则忽略错误
    int ret = unlink("/etc/localtime");
    if ((ret != 0) && (errno != ENOENT))
    {
        return -3;
    }

    // 创建软链接
    ret = symlink(zoneinfo_path, "/etc/localtime");
    if (ret != 0)
    {
        return -4;
    }

    // 更新环境变量
    tzset();

    return 0;
}

int hs_time_get_date_time_str(char *date_time, const size_t len, const uint8_t gap_flag)
{
    if (date_time == NULL)
    {
        return -1;
    }

    if (len <= 0)
    {
        return -2;
    }

    struct timespec time_spec = {0};
    if (clock_gettime(CLOCK_REALTIME, &time_spec) == -1)
    {
        return -3;
    }

    struct tm local_time = {0};
    if (localtime_r(&time_spec.tv_sec, &local_time) == NULL)
    {
        return -4;
    }

    switch (gap_flag)
    {
    case 0:
    {
        snprintf(date_time, len, "%4d%02d%02d%02d%02d%02d%06ld", (local_time.tm_year + 1900), (local_time.tm_mon + 1),
                 local_time.tm_mday, local_time.tm_hour, local_time.tm_min, local_time.tm_sec,
                 (time_spec.tv_nsec / 1000));

        break;
    }

    case 1:
    {
        snprintf(date_time, len, "%4d-%02d-%02d %02d:%02d:%02d.%06ld", (local_time.tm_year + 1900),
                 (local_time.tm_mon + 1), local_time.tm_mday, local_time.tm_hour, local_time.tm_min, local_time.tm_sec,
                 (time_spec.tv_nsec / 1000));

        break;
    }

    case 2:
    {
        snprintf(date_time, len, "%4d-%02d-%02d_%02d-%02d-%02d-%06ld", (local_time.tm_year + 1900),
                 (local_time.tm_mon + 1), local_time.tm_mday, local_time.tm_hour, local_time.tm_min, local_time.tm_sec,
                 (time_spec.tv_nsec / 1000));

        break;
    }

    default:
    {
        return -5;
    }
    }

    return 0;
}

int hs_time_get_local_time(hs_date_time_t *local_time)
{
    if (local_time == NULL)
    {
        return -1;
    }

    struct timespec time_spec = {0};
    if (clock_gettime(CLOCK_REALTIME, &time_spec) == -1)
    {
        return -2;
    }

    struct tm local_time_system = {0};
    if (localtime_r(&time_spec.tv_sec, &local_time_system) == NULL)
    {
        return -3;
    }

    if (hs_time_tm_to_date_time(local_time, &local_time_system) != 0)
    {
        return -4;
    }

    return 0;
}

int hs_time_set_local_time(const hs_date_time_t *local_time)
{
    if (local_time == NULL)
    {
        return -1;
    }

    struct tm new_time = {0};
    if (hs_time_date_time_to_tm(&new_time, local_time) != 0)
    {
        return -2;
    }
    new_time.tm_isdst = -1; // 让系统自动判断是否为夏令时

    time_t mk_ret = mktime(&new_time);
    if (mk_ret == -1)
    {
        return -3;
    }

    struct timespec time_spec = {0};
    time_spec.tv_sec = mk_ret;

    int ret = clock_settime(CLOCK_REALTIME, &time_spec);
    if (ret == -1)
    {
        return -4;
    }

    return 0;
}

int hs_time_get_current_timestamp_ms(uint64_t *timestamp_ms)
{
    if (timestamp_ms == NULL)
    {
        return -1;
    }

    struct timespec time_spec = {0};
    if (clock_gettime(CLOCK_REALTIME, &time_spec) == -1)
    {
        return -2;
    }

    *timestamp_ms = time_spec.tv_sec * 1000 + time_spec.tv_nsec / 1000000;

    return 0;
}

int hs_time_get_uptime_ms(uint64_t *uptime_ms)
{
    if (uptime_ms == NULL)
    {
        return -1;
    }

    struct timespec time_spec = {0};
    if (clock_gettime(CLOCK_MONOTONIC, &time_spec) == -1)
    {
        return -2;
    }

    *uptime_ms = time_spec.tv_sec * 1000 + time_spec.tv_nsec / 1000000;

    return 0;
}

int hs_time_convert_unix_to_local(hs_date_time_t *local_time, const uint64_t unix_timestamp_s)
{
    if (local_time == NULL)
    {
        return -1;
    }

    struct tm local_time_system = {0};
    if (localtime_r((const time_t *)&unix_timestamp_s, &local_time_system) == NULL)
    {
        return -2;
    }

    if (hs_time_tm_to_date_time(local_time, &local_time_system) != 0)
    {
        return -3;
    }

    return 0;
}

int hs_time_convert_local_to_unix(uint64_t *unix_timestamp_s, const hs_date_time_t *local_time)
{
    if (unix_timestamp_s == NULL)
    {
        return -1;
    }

    if (local_time == NULL)
    {
        return -2;
    }

    struct tm local_time_system = {0};
    if (hs_time_date_time_to_tm(&local_time_system, local_time) != 0)
    {
        return -3;
    }

    time_t mk_ret = mktime(&local_time_system);
    if (mk_ret == -1)
    {
        return -4;
    }
    *unix_timestamp_s = (uint64_t)mk_ret;

    return 0;
}

int hs_time_set_hardware_clock_from_system_time(const char *device)
{
    if (device == NULL)
    {
        return -1;
    }

    hs_date_time_t local_time = {0};
    if (hs_time_get_local_time(&local_time) != 0)
    {
        return -2;
    }

    struct rtc_time rtc_tm = {0};
    if (hs_time_date_time_to_rtc_time(&rtc_tm, &local_time) != 0)
    {
        return -3;
    }

    int fd = open(device, O_RDWR);
    if (fd < 0)
    {
        return -4;
    }

    if (ioctl(fd, RTC_SET_TIME, &rtc_tm) == -1)
    {
        close(fd);

        return -5;
    }

    close(fd);

    return 0;
}

int hs_time_set_system_time_from_hardware_clock(const char *device)
{
    if (device == NULL)
    {
        return -1;
    }

    int fd = open(device, O_RDWR);
    if (fd < 0)
    {
        return -2;
    }

    struct rtc_time rtc_tm = {0};
    if (ioctl(fd, RTC_RD_TIME, &rtc_tm) == -1)
    {
        close(fd);

        return -3;
    }
    close(fd);

    hs_date_time_t local_time = {0};
    if (hs_time_rtc_time_to_date_time(&local_time, &rtc_tm) != 0)
    {
        return -4;
    }

    if (hs_time_set_local_time(&local_time) != 0)
    {
        return -5;
    }

    return 0;
}
