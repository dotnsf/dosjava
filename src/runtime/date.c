#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "date.h"
#include "system.h"

/* Class ID for Date objects */
#define CLASS_ID_DATE 4

/**
 * Ensure cached date/time values are valid
 * Recalculates from time_sec if cache is invalid
 */
static void ensure_cache(Date* date) {
    DOSDateTime dt;
    
    if (!date || date->cache_valid) {
        return;
    }
    
    /* Convert timestamp to date/time components */
    if (dos_timestamp_to_datetime(date->time_sec, &dt) == 0) {
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
    uint32_t current_time;
    
    /* Get current timestamp */
    current_time = dos_get_timestamp();
    
    return date_new_with_time(current_time);
}

/**
 * Create a new Date with specified time
 */
Date* date_new_with_time(uint32_t time_sec) {
    Date* date;
    
    /* Allocate Date structure */
    date = (Date*)malloc(sizeof(Date));
    if (!date) {
        return NULL;
    }
    
    /* Initialize base Object */
    date->base.class_id = CLASS_ID_DATE;
    date->base.ref_count = 1;
    
    /* Initialize Date fields */
    date->time_sec = time_sec;
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
 * Get time value in seconds
 */
uint32_t date_get_time(Date* date) {
    if (!date) {
        return 0;
    }
    
    return date->time_sec;
}

/**
 * Set time value in seconds
 */
void date_set_time(Date* date, uint32_t time_sec) {
    if (!date) {
        return;
    }
    
    date->time_sec = time_sec;
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

// Made with Bob
