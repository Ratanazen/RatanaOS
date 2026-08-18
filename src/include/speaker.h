#ifndef SPEAKER_H
#define SPEAKER_H

#include "types.h"

void speaker_play_tone(uint32_t frequency_hz);
void speaker_stop(void);
void speaker_beep(uint32_t frequency_hz, uint32_t duration_ms);

#endif // SPEAKER_H
