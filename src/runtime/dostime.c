/**
 * DOS Time API Wrapper Implementation
 */

#include "dostime.h"
#include <dos.h>
#include <string.h>

/* Days in each month (non-leap year) */
static const uint8_t days_in_month[12] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

/* Check if year is leap year */
static int is_leap_year(uint16_t year) {
    if (year % 400 == 0) return 1;
    if (year % 100 == 0) return 0;
    if (year % 4 == 0) return 1;
    return 0;
}

/* Get days in month for given year/month */
static uint8_t get_days_in_month(uint16_t year, uint8_t month) {
    if (month == 2 && is_leap_year(year)) {
        return 29;
    }
    return days_in_month[month - 1];
}

/**
 * Get current date and time from DOS
 */
int dos_get_datetime(DOSDateTime* dt) {
    union REGS regs;
    
    if (dt == NULL) {
        return -1;
    }
    
    /* Get date: INT 21h, AH=2Ah */
    regs.h.ah = 0x2A;
    int86(0x21, &regs, &regs);
    
    dt->year = regs.x.cx;
    dt->month = regs.h.dh;
    dt->day = regs.h.dl;
    
    /* Get time: INT 21h, AH=2Ch */
    regs.h.ah = 0x2C;
    int86(0x21, &regs, &regs);
    
    dt->hour = regs.h.ch;
    dt->minute = regs.h.cl;
    dt->second = regs.h.dh;
    dt->hundredths = regs.h.dl;
    
    return 0;
}

/**
 * Convert DOS datetime to Unix timestamp
 * Unix epoch: 1970-01-01 00:00:00
 * DOS epoch: 1980-01-01 00:00:00
 */
uint32_t dos_datetime_to_timestamp(const DOSDateTime* dt) {
    uint32_t days;
    uint32_t timestamp;
    uint16_t year;
    uint8_t month;
    
    if (dt == NULL || dt->year < 1980) {
        return 0;
    }
    
    /* Calculate days from 1970-01-01 to given date */
    days = 0;
    
    /* Add days for complete years from 1970 to (year-1) */
    for (year = 1970; year < dt->year; year++) {
        if (is_leap_year(year)) {
            days += 366;
        } else {
            days += 365;
        }
    }
    
    /* Add days for complete months in current year */
    for (month = 1; month < dt->month; month++) {
        days += get_days_in_month(dt->year, month);
    }
    
    /* Add days in current month */
    days += dt->day - 1;
    
    /* Convert to seconds */
    timestamp = days * 86400UL;  /* 24 * 60 * 60 */
    timestamp += (uint32_t)dt->hour * 3600UL;
    timestamp += (uint32_t)dt->minute * 60UL;
    timestamp += (uint32_t)dt->second;
    
    return timestamp;
}

/**
 * Convert Unix timestamp to DOS datetime
 */
int dos_timestamp_to_datetime(uint32_t timestamp, DOSDateTime* dt) {
    uint32_t days;
    uint32_t seconds_in_day;
    uint16_t year;
    uint8_t month;
    uint16_t days_in_year;
    
    if (dt == NULL) {
        return -1;
    }
    
    /* Clear structure */
    memset(dt, 0, sizeof(DOSDateTime));
    
    /* Calculate days and remaining seconds */
    days = timestamp / 86400UL;
    seconds_in_day = timestamp % 86400UL;
    
    /* Calculate time components */
    dt->hour = (uint8_t)(seconds_in_day / 3600UL);
    seconds_in_day %= 3600UL;
    dt->minute = (uint8_t)(seconds_in_day / 60UL);
    dt->second = (uint8_t)(seconds_in_day % 60UL);
    dt->hundredths = 0;
    
    /* Calculate date starting from 1970 */
    year = 1970;
    while (1) {
        days_in_year = is_leap_year(year) ? 366 : 365;
        if (days < days_in_year) {
            break;
        }
        days -= days_in_year;
        year++;
    }
    dt->year = year;
    
    /* Calculate month and day */
    month = 1;
    while (month <= 12) {
        uint8_t days_in_this_month = get_days_in_month(year, month);
        if (days < days_in_this_month) {
            break;
        }
        days -= days_in_this_month;
        month++;
    }
    dt->month = month;
    dt->day = (uint8_t)(days + 1);
    
    return 0;
}

/**
 * Get current time as Unix timestamp
 */
uint32_t dos_get_timestamp(void) {
    DOSDateTime dt;
    
    if (dos_get_datetime(&dt) < 0) {
        return 0;
    }
    
    return dos_datetime_to_timestamp(&dt);
}
