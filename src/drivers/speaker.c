#include "../include/speaker.h"
#include "../include/io.h"
#include "../include/timer.h"

void speaker_play_tone(uint32_t frequency_hz) {
    if (frequency_hz == 0) return;

    uint32_t div = 1193180 / frequency_hz;

    // Set PIT channel 2 mode
    outb(0x43, 0xB6);
    outb(0x42, (uint8_t)(div & 0xFF));
    outb(0x42, (uint8_t)((div >> 8) & 0xFF));

    // Enable speaker gate
    uint8_t tmp = inb(0x61);
    if (tmp != (tmp | 3)) {
        outb(0x61, tmp | 3);
    }
}

void speaker_stop(void) {
    uint8_t tmp = inb(0x61) & 0xFC;
    outb(0x61, tmp);
}

void speaker_beep(uint32_t frequency_hz, uint32_t duration_ms) {
    speaker_play_tone(frequency_hz);
    timer_sleep_ms(duration_ms);
    speaker_stop();
}
