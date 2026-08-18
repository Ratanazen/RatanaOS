#include "../include/rtc.h"
#include "../include/io.h"
#include "../include/stdio.h"

#define CMOS_ADDRESS 0x70
#define CMOS_DATA    0x71

static int get_update_in_progress_flag(void) {
    outb(CMOS_ADDRESS, 0x0A);
    return (inb(CMOS_DATA) & 0x80);
}

static uint8_t get_rtc_register(int reg) {
    outb(CMOS_ADDRESS, (uint8_t)reg);
    return inb(CMOS_DATA);
}

void rtc_init(void) {
    // CMOS ready
}

static rtc_time_t read_raw_rtc(void) {
    rtc_time_t t;
    t.second = get_rtc_register(0x00);
    t.minute = get_rtc_register(0x02);
    t.hour   = get_rtc_register(0x04);
    t.day    = get_rtc_register(0x07);
    t.month  = get_rtc_register(0x08);
    t.year   = get_rtc_register(0x09);
    return t;
}

rtc_time_t rtc_get_time(void) {
    rtc_time_t t;
    rtc_time_t last;
    uint8_t century = 0;
    uint8_t registerB;

    int timeout = 10000;
    while (get_update_in_progress_flag() && --timeout > 0);

    t = read_raw_rtc();

    // Read until two consecutive reads match (guards against midpoint register updates)
    int max_retries = 5;
    do {
        last = t;
        t = read_raw_rtc();
    } while ((last.second != t.second || last.minute != t.minute || last.hour != t.hour ||
              last.day != t.day || last.month != t.month || last.year != t.year) && --max_retries > 0);

    century = get_rtc_register(0x32);
    registerB = get_rtc_register(0x0B);

    // Convert BCD to binary values if necessary
    if (!(registerB & 0x04)) {
        t.second = ((t.second & 0xF0) >> 4) * 10 + (t.second & 0x0F);
        t.minute = ((t.minute & 0xF0) >> 4) * 10 + (t.minute & 0x0F);
        t.hour   = (((t.hour & 0xF0) >> 4) * 10 + (t.hour & 0x0F));
        t.day    = ((t.day & 0xF0) >> 4) * 10 + (t.day & 0x0F);
        t.month  = ((t.month & 0xF0) >> 4) * 10 + (t.month & 0x0F);
        t.year   = ((t.year & 0xF0) >> 4) * 10 + (t.year & 0x0F);
        if (century != 0) {
            century = ((century & 0xF0) >> 4) * 10 + (century & 0x0F);
        }
    }

    // Convert 12 hour clock to 24 hour clock if necessary
    if (!(registerB & 0x02) && (t.hour & 0x80)) {
        t.hour = ((t.hour & 0x7F) + 12) % 24;
    }

    // Calculate full 4-digit year
    if (century != 0) {
        t.year += (uint32_t)century * 100;
    } else {
        t.year += 2000;
    }

    return t;
}

void rtc_print_formatted(void) {
    rtc_time_t t = rtc_get_time();
    kprintf("RTC Date & Time: %u-%02u-%02u %02u:%02u:%02u UTC\n",
            t.year, t.month, t.day, t.hour, t.minute, t.second);
}
