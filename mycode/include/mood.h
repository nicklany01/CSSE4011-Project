#ifndef MOOD_H
#define MOOD_H

#include <zephyr/kernel.h>

#define CLAMP(val, min, max) ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))

// These could be dependent on the pet attributes, but for now they are fixed
// thresholds for happiness
#define ENERGY_THRESHOLD 500
#define NUTRITION_THRESHOLD 500
#define INTERACTION_THRESHOLD 500

// mood from 0 to 1000
struct mood_state {
    int16_t affection;
    int16_t happiness;
    int16_t energy;
    int16_t nutrition;
    int16_t interaction;
};

extern struct mood_state pet_mood;

void mood_reset();
void mood_step(struct mood_state *update);
void mood_print(struct mood_state *state);
void mood_thread();

#endif