#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "date.h"
#include "system.h"

/* Class ID for Date objects */
#define CLASS_ID_DATE 4

/**
 * Ensure cached date/time values are valid
 * Recalculates from time_ms if cache is invalid
 */
static void ensure_cache(Date* date) {
    DOSDateTime dt;
    uint32_t time_sec;
    
    if (!date) {
        return;
    }
    
    if (date->cache_valid) {
        return;
    }
    
    /* IMPORTANT: time_ms_low stores SECONDS (not milliseconds) for 32-bit compatibility
     * We use it directly as seconds for date/time calculation */
    time_sec = date->time_ms_low;
    
    /* Convert timestamp to date/time components */
    if (dos_timestamp_to_datetime(time_sec, &dt) == 0) {
        date->cached_year = dt.year;
        date->cached_month = dt.month - 1;  /* Convert to 0-based */
        date->cached_day = dt.day;
        date->cached_hour = dt.hour;
        date->cached_minute = dt.minute;
        date->cached_second = dt.second;
        date->cache_valid = 1;
    }
}

/**
 * Create a new Date with current time
 */
Date* date_new(void) {
    uint32_t current_time_sec;
    uint32_t time_ms_high, time_ms_low;
    
    /* Get current timestamp in seconds */
    current_time_sec = dos_get_timestamp();
    
    /* IMPORTANT: Due to 32-bit limitations, we store SECONDS (not milliseconds)
     * This allows dates up to year 2106 without overflow
     * Modern dates like 2026 = 1779468303 seconds would overflow if multiplied by 1000 */
    time_ms_high = 0;
    time_ms_low = current_time_sec;  /* Store seconds directly */
    
    return date_new_with_time(time_ms_high, time_ms_low);
}
/**
 * Create a new Date with specified time
 */
Date* date_new_with_time(uint32_t time_ms_high, uint32_t time_ms_low) {
    Date* date;
    
    /* Allocate Date structure */
    date = (Date*)malloc(sizeof(Date));
    if (!date) {
        return NULL;
    }
    
    /* Initialize base Object */
    date->base.class_id = CLASS_ID_DATE;
    date->base.ref_count = 1;
    
    /* Initialize Date fields
     * IMPORTANT: Due to 32-bit limitations, we treat time_ms_low as SECONDS
     * This allows dates up to year 2106 */
    date->time_ms_high = 0;
    date->time_ms_low = time_ms_low;  /* Store as-is (seconds) */
    date->cache_valid = 0;
    
    return date;
}

/**
 * Delete a Date instance
 */
void date_delete(Date* date) {
    if (!date) {
        return;
    }
    
    free(date);
}

/**
 * Get time value in milliseconds (high 32 bits)
 */
uint32_t date_get_time_high(Date* date) {
    if (!date) {
        return 0;
    }
    
    return date->time_ms_high;
}

/**
 * Get time value in milliseconds (low 32 bits)
 */
uint32_t date_get_time_low(Date* date) {
    if (!date) {
        return 0;
    }
    
    /* IMPORTANT: Due to 32-bit limitations, we return SECONDS (not milliseconds)
     * Multiplying by 1000 would overflow for modern dates (2026 = 1779468192 seconds)
     * 1779468192 * 1000 = 1779468192000 > 4294967295 (32-bit max)
     * So we return seconds directly */
    return date->time_ms_low;
}

/**
 * Set time value in milliseconds
 */
void date_set_time(Date* date, uint32_t time_ms_high, uint32_t time_ms_low) {
    if (!date) {
        return;
    }
    
    /* IMPORTANT: Due to 32-bit limitations, we treat time_ms_low as SECONDS
     * not milliseconds. This allows dates up to year 2106 */
    date->time_ms_high = 0;
    date->time_ms_low = time_ms_low;  /* Store as-is (seconds) */
    date->cache_valid = 0;  /* Invalidate cache */
}

/**
 * Get full year (e.g., 2026)
 */
uint16_t date_get_full_year(Date* date) {
    if (!date) {
        return 0;
    }
    
    ensure_cache(date);
    return date->cached_year;
}

/**
 * Get month (0-11, where 0=January)
 */
uint8_t date_get_month(Date* date) {
    if (!date) {
        return 0;
    }
    
    ensure_cache(date);
    return date->cached_month;
}

/**
 * Get day of month (1-31)
 */
uint8_t date_get_date(Date* date) {
    if (!date) {
        return 0;
    }
    
    ensure_cache(date);
    return date->cached_day;
}

/**
 * Get hours (0-23)
 */
uint8_t date_get_hours(Date* date) {
    if (!date) {
        return 0;
    }
    
    ensure_cache(date);
    return date->cached_hour;
}

/**
 * Get minutes (0-59)
 */
uint8_t date_get_minutes(Date* date) {
    if (!date) {
        return 0;
    }
    
    ensure_cache(date);
    return date->cached_minute;
}

/**
 * Get seconds (0-59)
 */
uint8_t date_get_seconds(Date* date) {
    if (!date) {
        return 0;
    }
    
    ensure_cache(date);
    return date->cached_second;
}

/**
 * Get string representation of date
 * Format: "YYYY-MM-DD HH:MM:SS"
 */
char* date_to_string(Date* date) {
    char* buffer;
    
    if (!date) {
        return NULL;
    }
    
    /* Ensure cache is valid */
    ensure_cache(date);
    
    /* Allocate buffer for formatted string */
    /* Format: "YYYY-MM-DD HH:MM:SS" = 19 chars + null terminator */
    buffer = (char*)malloc(20);
    if (!buffer) {
        return NULL;
    }
    
    /* Format the date/time string */
    sprintf(buffer, "%04u-%02u-%02u %02u:%02u:%02u",
            (unsigned int)date->cached_year,
            (unsigned int)(date->cached_month + 1),  /* Convert back to 1-based */
            (unsigned int)date->cached_day,
            (unsigned int)date->cached_hour,
            (unsigned int)date->cached_minute,
            (unsigned int)date->cached_second);
    
    return buffer;
}
