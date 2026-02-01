/**
 * @file      hs_time.h
 * @brief     时间模块头文件
 * @author    huenrong (sgyhy1028@outlook.com)
 * @date      2026-02-01 14:29:42
 *
 * @copyright Copyright (c) 2026 huenrong
 *
 */

#ifndef __HS_TIME_H
#define __HS_TIME_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

// 日期时间结构体
typedef struct hs_date_time
{
    uint16_t tm_year; // 4位年份值(示例: 1995)
    uint8_t tm_mon;   // 取值范围: [1, 12]
    uint8_t tm_mday;  // 取值范围: [1, 31]
    uint8_t tm_hour;  // 取值范围: [0, 23]
    uint8_t tm_min;   // 取值范围: [0, 59]
    uint8_t tm_sec;   // 取值范围: [0, 59]
    uint16_t tm_yday; // 一年中的第几天(取值范围: [1, 366])
    uint8_t tm_wday;  // 星期几(取值范围: [1, 7]. 1: 周一, 7: 周日)
} hs_date_time_t;

/**
 * @brief 获取编译日期时间
 *
 * @param[out] date_time: 获取到的编译日期时间(示例: 2021-05-12 15:53:41)
 * @param[in]  len      : 缓冲区大小
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
int hs_time_get_compile_date_time(char *date_time, const size_t len);

/**
 * @brief 获取系统时区
 *
 * @param[out] timezone: 获取到的时区(示例: Asia/Shanghai)
 * @param[in]  len     : 缓冲区大小
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
int hs_time_get_system_timezone(char *timezone, const size_t len);

/**
 * @brief 设置系统时区
 *
 * @param[in] timezone: 待设置的时区(示例: Asia/Shanghai)
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
int hs_time_set_system_timezone(const char *timezone);

/**
 * @brief 获取系统当前日期时间字符串
 *
 * @param[out] date_time: 获取到的日期时间串
 * @param[in]  len      : 缓冲区大小
 * @param[in]  gap_flag : 获取到的时间是否需要间隔
 *                            0: 无间隔: 20180806163000616678
 *                            1: 有间隔: 2018-08-06 16:30:00.616678
 *                            2: 有间隔: 2018-08-06_16-30-00-616678
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
int hs_time_get_date_time_str(char *date_time, const size_t len, const uint8_t gap_flag);

/**
 * @brief 获取系统时间
 *
 * @param[out] local_time: 获取到的本地时区时间
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
int hs_time_get_local_time(hs_date_time_t *local_time);

/**
 * @brief 设置系统时间
 *
 * @param[in] local_time: 待设置的本地时区时间
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
int hs_time_set_local_time(const hs_date_time_t *local_time);

/**
 * @brief 获取当前 UTC 时间戳(毫秒)
 *
 * @param[out] timestamp_ms: 自 1970-01-01 00:00:00 UTC 起的毫秒数
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
int hs_time_get_current_timestamp_ms(uint64_t *timestamp_ms);

/**
 * @brief 获取系统开机以来的毫秒数(单调时钟)
 *
 * @param[out] uptime_ms: 系统开机到现在的毫秒数
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
int hs_time_get_uptime_ms(uint64_t *uptime_ms);

/**
 * @brief 将 Unix 时间戳(秒)转换为本地时间
 *
 * @param[out] local_time      : 转换后的本地时间
 * @param[in]  unix_timestamp_s: 自 1970-01-01 00:00:00 UTC 起的秒数
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
int hs_time_convert_unix_to_local(hs_date_time_t *local_time, const uint64_t unix_timestamp_s);

/**
 * @brief 将本地时间转换为 Unix 时间戳(秒)
 *
 * @param[out] unix_timestamp_s: 自 1970-01-01 00:00:00 UTC 起的秒数
 * @param[in]  local_time      : 本地时间
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
int hs_time_convert_local_to_unix(uint64_t *unix_timestamp_s, const hs_date_time_t *local_time);

/**
 * @brief 从系统时间设置硬件时钟
 *
 * @param[in] device: RTC 设备(示例: /dev/rtc0)
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
int hs_time_set_hardware_clock_from_system_time(const char *device);

/**
 * @brief 从硬件时钟设置系统时间
 *
 * @param[in] device: RTC 设备(示例: /dev/rtc0)
 *
 * @return 0 : 成功
 * @return <0: 失败
 */
int hs_time_set_system_time_from_hardware_clock(const char *device);

#ifdef __cplusplus
}
#endif

#endif // __HS_TIME_H
