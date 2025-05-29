#ifndef MOOD_H
#define MOOD_H

#include <zephyr/kernel.h>

#define MOOD_THREAD_STACK_SIZE 4096
#define MOOD_THREAD_PRIORITY 7


#define CLAMP(val, min, max) ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))

// These could be dependent on the pet attributes, but for now they are fixed
// thresholds for happiness
#define ENERGY_THRESHOLD 500
#define HEALTH_THRESHOLD 500
#define INTERACTION_THRESHOLD 500
#define MAX_STATE_VALUE 1000

enum expression {
    EXPRESSION_ENLIGHTENED,
    EXPRESSION_V_HAPPY,
    EXPRESSION_HAPPY,
    EXPRESSION_NEUTRAL,
    EXPRESSION_SAD,
    EXPRESSION_V_SAD,
    EXPRESSION_ANGRY,
    EXPRESSION_SLEEPY
};

// mood from 0 to 1000
struct mood_state {
    int16_t affection;  // grows if happiness is high
    int16_t happiness;  // grows based on health, energy and interaction
    int16_t energy;     // increases by feeding, decreases from interaction
    int16_t health;     // increases by when energy is above threshold, decreases from disruptions
    int16_t interaction;// increases by meeting other pets
    enum expression expression;
};

static const char* expression_strings[] = {
    "ENLIGHTENED",  // EXPRESSION_ENLIGHTENED
    "VERY HAPPY",   // EXPRESSION_V_HAPPY
    "HAPPY",        // EXPRESSION_HAPPY
    "NEUTRAL",      // EXPRESSION_NEUTRAL
    "SAD",          // EXPRESSION_SAD
    "VERY SAD",     // EXPRESSION_V_SAD
    "ANGRY",        // EXPRESSION_ANGRY
    "SLEEPY"        // EXPRESSION_SLEEPY
};

extern struct mood_state pet_mood;
extern struct k_mutex mood_mutex;

void mood_init();
void mood_reset();
void mood_step();
void mood_print(struct mood_state *state);
void mood_thread(void *arg1, void *arg2, void *arg3);

#endif
