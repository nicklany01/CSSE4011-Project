#ifndef SOUND_H
#define SOUND_H

#include <zephyr/kernel.h>
#include <zephyr/drivers/i2s.h>

/**
 * @brief Initialize the sound system
 * @return 0 on success, negative error code on failure
 */
int sound_init(void);

/**
 * @brief Play the sound
 * @return 0 on success, negative error code on failure
 */
int sound_play(void);

/**
 * @brief Clean up sound resources
 */
void sound_cleanup(void);

#endif