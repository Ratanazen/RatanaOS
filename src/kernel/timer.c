#include "../include/timer.h"
#include "../include/isr.h"
#include "../include/pic.h"
#include "../include/io.h"

static volatile uint32_t timer_ticks = 0;

static void timer_callback(registers_t* regs) {
    (void)regs;
    timer_ticks++;
}

void timer_init(uint32_t frequency) {
    register_interrupt_handler(IRQ0, timer_callback);

    uint32_t divisor = 1193180 / frequency;

    outb(0x43, 0x36);

    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)((divisor >> 8) & 0xFF);

    outb(0x40, l);
    outb(0x40, h);
}

uint32_t timer_get_ticks(void) {
    return timer_ticks;
}

void timer_sleep(uint32_t ticks) {
    uint32_t end = timer_ticks + ticks;
    while (timer_ticks < end) {
        __asm__ volatile ("hlt");
    }
}

void timer_sleep_ms(uint32_t ms) {
    timer_sleep((ms + 9) / 10);
}
