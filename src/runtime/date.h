#ifndef DATE_H
#define DATE_H

#include "../types.h"
#include "object.h"
#include "dostime.h"

/**
 * Date structure - C representation of java.util.Date
 * 
 * Represents a specific instant in time with millisecond precision
 * Internally stores time as Unix timestamp (seconds since 1970-01-01)
 */
typedef struct Date {
    Object base;             /* Base Object structure */
    uint32_t time_sec;       /* Time in seconds since Unix epoch */
    uint8_t cache_valid;     /* 1 if cached values are valid */
    uint16_t cached_year;    /* Cached year */
    uint8_t cached_month;    /* Cached month (0-11) */
    uint8_t cached_day;      /* Cached day (1-31) */
    uint8_t cached_hour;     /* Cached hour (0-23) */
    uint8_t cached_minute;   /* Cached minute (0-59) */
    uint8_t cached_second;   /* Cached second (0-59) */
} Date;

/**
 * Create a new Date with current time
 * @return Pointer to new Date, or NULL on error
 */
Date* date_new(void);

/**
 * Create a new Date with specified time
 * @param time_sec Time in seconds since Unix epoch
 * @return Pointer to new Date, or NULL on error
 */
Date* date_new_with_time(uint32_t time_sec);

/**
 * Delete a Date instance
 * @param date Date to delete
 */
void date_delete(Date* date);

/**
 * Get time value in seconds
 * @param date Date object
 * @return Time in seconds since Unix epoch
 */
uint32_t date_get_time(Date* date);

/**
 * Set time value in seconds
 * @param date Date object
 * @param time_sec Time in seconds since Unix epoch
 */
void date_set_time(Date* date, uint32_t time_sec);

/**
 * Get full year (e.g., 2026)
 * @param date Date object
 * @return Year value
 */
uint16_t date_get_full_year(Date* date);

/**
 * Get month (0-11, where 0=January)
 * @param date Date object
 * @return Month value (0-11)
 */
uint8_t date_get_month(Date* date);

/**
 * Get day of month (1-31)
 * @param date Date object
 * @return Day value (1-31)
 */
uint8_t date_get_date(Date* date);

/**
 * Get hours (0-23)
 * @param date Date object
 * @return Hour value (0-23)
 */
uint8_t date_get_hours(Date* date);

/**
 * Get minutes (0-59)
 * @param date Date object
 * @return Minute value (0-59)
 */
uint8_t date_get_minutes(Date* date);

/**
 * Get seconds (0-59)
 * @param date Date object
 * @return Second value (0-59)
 */
uint8_t date_get_seconds(Date* date);

/**
 * Get string representation of date
 * Format: "YYYY-MM-DD HH:MM:SS"
 * @param date Date object
 * @return String representation (caller must free)
 */
char* date_to_string(Date* date);

#endif /* DATE_H */
