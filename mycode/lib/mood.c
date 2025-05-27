#include "mood.h"
#include <zephyr/kernel.h>
#include <stdio.h>
#include <zephyr/logging/log.h>
#include "mpu6886.h"

LOG_MODULE_REGISTER(mood);

// Global mood state
struct mood_state pet_mood = {
    .affection = 500,
    .happiness = 500,
    .energy = 500,
    .health = 500,
    .interaction = 500,
    .expression = EXPRESSION_NEUTRAL
};

struct k_mutex mood_mutex;

void mood_reset() {
    LOG_INF("Mood reset to default values");
    pet_mood.affection = 500;
    pet_mood.happiness = 500;
    pet_mood.energy = 500;
    pet_mood.health = 500;
    pet_mood.interaction = 500;
}

void mood_init(void) {
    k_mutex_init(&mood_mutex);
    mood_reset();
}

void mood_print(struct mood_state *state) {
    if (state == NULL) {
        LOG_ERR("Invalid mood state provided");
        return;
    }

    LOG_INF("Current Mood State:");
    LOG_INF("Affection: %d", state->affection);
    LOG_INF("Happiness: %d", state->happiness);
    LOG_INF("Energy: %d", state->energy);
    LOG_INF("health: %d", state->health);
    LOG_INF("Interaction: %d", state->interaction);
}

void mood_step() {
    // Simulate mood dynamics
    pet_mood.energy -= 1;
    pet_mood.interaction -= 1;
    pet_mood.health += (pet_mood.energy > ENERGY_THRESHOLD) ? 1 : -1;

    pet_mood.happiness += pet_mood.energy > ENERGY_THRESHOLD ? 1 : -1;
    pet_mood.happiness += pet_mood.health > HEALTH_THRESHOLD ? 1 : -1;
    pet_mood.happiness += pet_mood.interaction > INTERACTION_THRESHOLD ? 1 : -1;

    // this basically means affection grows with happiness, but is capped in tiers so that the max affection can only be reached by the max happiness tier. if happiness tier is lower than the affection starts decreasing, otherwise it increases until it hits the cap allowed by the happiness tier.
    uint8_t happiness_tier = pet_mood.happiness / 200;
    uint8_t affection_tier = pet_mood.affection / 200;
    if (happiness_tier < affection_tier) {
        pet_mood.affection -= 1;
    } else if (happiness_tier >= affection_tier) {
        pet_mood.affection += 1;
        // can't grow more affection than happiness tier allows
        CLAMP(pet_mood.affection, 0, happiness_tier * 200);
    }

    pet_mood.affection = CLAMP(pet_mood.affection, 0, MAX_STATE_VALUE);
    pet_mood.happiness = CLAMP(pet_mood.happiness, 0, MAX_STATE_VALUE);
    pet_mood.energy = CLAMP(pet_mood.energy, 0, MAX_STATE_VALUE);
    pet_mood.health = CLAMP(pet_mood.health, 0, MAX_STATE_VALUE);
    pet_mood.interaction = CLAMP(pet_mood.interaction, 0, MAX_STATE_VALUE);

    uint8_t happiness_expression = pet_mood.happiness / 200;

    switch(happiness_expression) {
        case 0:
            pet_mood.expression = EXPRESSION_V_SAD;
            break;
        case 1:
            pet_mood.expression = EXPRESSION_SAD;
            break;
        case 2:
            pet_mood.expression = EXPRESSION_NEUTRAL;
            break;
        case 3:
            pet_mood.expression = EXPRESSION_HAPPY;
            break;
        case 4:
            pet_mood.expression = EXPRESSION_V_HAPPY;
            break;
        default:
            pet_mood.expression = EXPRESSION_NEUTRAL;
            break;
    }

    // special cases
    if (pet_mood.energy < 100) { // TODO: add rtc time in here for sleepy
        pet_mood.expression = EXPRESSION_SLEEPY;
    }
    if (pet_mood.health < 100) { // TODO: add detection for negative interaction with pet
        pet_mood.expression = EXPRESSION_ANGRY; // angry if health is low
    }
    if (pet_mood.affection == 1000 && pet_mood.happiness == 1000) { // can add other special conditions so this is more rare. Maybe only rare variants of the pet can reach this?
        pet_mood.expression = EXPRESSION_ENLIGHTENED; // special case for max happiness and affection
    }
}

void mood_thread(void *arg1, void *arg2, void *arg3) {
    mpu6886_accel_t accel;
    float accel_mag;
    while (1) {
        if(k_mutex_lock(&mood_mutex, K_MSEC(50)) == 0) {
            mood_step();
            mpu6886_read_accel(&accel);
            accel_mag = mpu6886_get_adjusted_accel_magnitude(&accel);
            LOG_INF("Accelerometer magnitude: %.2f", accel_mag);
            if (accel_mag > 1) {
                pet_mood.health -= 50;
                pet_mood.health = CLAMP(pet_mood.health, 0, MAX_STATE_VALUE);
            }
            mood_print(&pet_mood);
            k_mutex_unlock(&mood_mutex);
        }
        display_update_mood();
        k_sleep(K_MSEC(100));
    }
}
