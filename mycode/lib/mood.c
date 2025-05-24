#include "mood.h"
#include <zephyr/kernel.h>
#include <stdio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(mood);

// Global mood state
struct mood_state pet_mood = {
    .affection = 500,
    .happiness = 500,
    .energy = 500,
    .nutrition = 500,
    .interaction = 500
};

// Clamp macro
#define CLAMP(val, min, max) ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))

void mood_reset() {
    LOG_INF("Mood reset to default values");
    pet_mood.affection = 500;
    pet_mood.happiness = 500;
    pet_mood.energy = 500;
    pet_mood.nutrition = 500;
    pet_mood.interaction = 500;
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
    LOG_INF("Nutrition: %d", state->nutrition);
    LOG_INF("Interaction: %d", state->interaction);
}

void mood_step(struct mood_state *update) {
    // Simulate mood dynamics
    pet_mood.energy -= 1;
    pet_mood.nutrition -= 1;
    pet_mood.interaction -= 1;

    pet_mood.energy += update.energy;
    pet_mood.nutrition += update.nutrition;
    pet_mood.interaction += update.interaction;

    pet_mood.happiness += pet_mood.energy > ENERGY_THRESHOLD ? 1 : -1;
    pet_mood.happiness += pet_mood.nutrition > NUTRITION_THRESHOLD ? 1 : -1;
    pet_mood.happiness += pet_mood.interaction > INTERACTION_THRESHOLD ? 1 : -1;

    pet_mood.affection += (pet_mood.happiness > 500) ? 1 : -1;

    // this basically means affection grows with happiness, but is capped in tiers so that the max affection can only be reached by the max happiness tier. if happiness tier is lower than the affection starts decreasing, otherwise it increases until it hits the cap allowed by the happiness tier.
    happiness_tier = pet_mood.happiness / 200;
    affection_tier = pet_mood.affection / 200;
    if (happiness_tier < affection_tier) {
        pet_mood.affection -= 1;
    } else if (happiness_tier >= affection_tier) {
        pet_mood.affection += 1;
        // can't grow more affection than happiness tier allows
        CLAMP(pet_mood.affection, 0, happiness_tier * 200);
    }

    pet_mood.affection = CLAMP(pet_mood.affection, 0, 1000);
    pet_mood.happiness = CLAMP(pet_mood.happiness, 0, 1000);
    pet_mood.energy = CLAMP(pet_mood.energy, 0, 1000);
    pet_mood.nutrition = CLAMP(pet_mood.nutrition, 0, 1000);
    pet_mood.interaction = CLAMP(pet_mood.interaction, 0, 1000);
}

void mood_thread() {
    // todo, implement sensor moving average, interaction detection, k_uptime_get() to do this all in one thread
    while (1) {
        mood_step();
        mood_print(&pet_mood);
        k_sleep(K_MSEC(100));
    }
}
