#ifndef SOUND_H
#define SOUND_H

#include <zephyr/kernel.h>

int sound_init();
int sound_play();
void sound_stop();
void sound_cleanup();

#endif // SOUND_H