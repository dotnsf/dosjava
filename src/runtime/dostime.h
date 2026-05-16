/**
 * DOS Time API Wrapper
 * Provides access to DOS system time functions
 */

#ifndef DOSTIME_H
#define DOSTIME_H

#include <stdint.h>

/**
 * DOS DateTime structure
 * Represents date and time components
 */
typedef struct {
    uint16_t year;      /* 1980-2099 */
    uint8_t month;      /* 1-12 */
    uint8_t day;        /* 1-31 */
    uint8_t hour;       /* 0-23 */
    uint8_t minute;     /* 0-59 */
    uint8_t second;     /* 0-59 */
    uint8_t hundredths; /* 0-99 (1/100 second) */
} DOSDateTime;

/**
 * Get current date and time from DOS
 * Returns: 0 on success, -1 on error
 */
int dos_get_datetime(DOSDateTime* dt);

/**
 * Convert DOS datetime to Unix timestamp (seconds since 1970-01-01)
 * Note: DOS time starts from 1980-01-01, so dates before that return 0
 * Returns: timestamp in seconds, or 0 on error
 */
uint32_t dos_datetime_to_timestamp(const DOSDateTime* dt);

/**
 * Convert Unix timestamp to DOS datetime
 * Returns: 0 on success, -1 on error
 */
int dos_timestamp_to_datetime(uint32_t timestamp, DOSDateTime* dt);

/**
 * Get current time as Unix timestamp
 * Returns: current timestamp in seconds
 */
uint32_t dos_get_timestamp(void);

#endif /* DOSTIME_H */
