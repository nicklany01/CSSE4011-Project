#include "sound.h"
#include <zephyr/drivers/i2s.h>
#include <zephyr/sys/iterable_sections.h>
#include <stdio.h>

#define SAMPLE_NO 64
#define NUM_BLOCKS 20
#define BLOCK_SIZE (2 * SAMPLE_NO * sizeof(int16_t))

// sine wave data
static int16_t data[SAMPLE_NO] = {
    3211,   6392,   9511,  12539,  15446,  18204,  20787,  23169,
    25329,  27244,  28897,  30272,  31356,  32137,  32609,  32767,
    32609,  32137,  31356,  30272,  28897,  27244,  25329,  23169,
    20787,  18204,  15446,  12539,   9511,   6392,   3211,      0,
    -3212,  -6393,  -9512, -12540, -15447, -18205, -20788, -23170,
    -25330, -27245, -28898, -30273, -31357, -32138, -32610, -32767,
    -32610, -32138, -31357, -30273, -28898, -27245, -25330, -23170,
    -20788, -18205, -15447, -12540,  -9512,  -6393,  -3212,     -1,
};

#ifdef CONFIG_NOCACHE_MEMORY
    #define MEM_SLAB_CACHE_ATTR __nocache
#else
    #define MEM_SLAB_CACHE_ATTR
#endif

static char MEM_SLAB_CACHE_ATTR __aligned(WB_UP(32))
    _k_mem_slab_buf_tx_0_mem_slab[(NUM_BLOCKS) * WB_UP(BLOCK_SIZE)];

static STRUCT_SECTION_ITERABLE(k_mem_slab, tx_0_mem_slab) =
    Z_MEM_SLAB_INITIALIZER(tx_0_mem_slab, _k_mem_slab_buf_tx_0_mem_slab,
            WB_UP(BLOCK_SIZE), NUM_BLOCKS);

static const struct device *dev_i2s = DEVICE_DT_GET(DT_ALIAS(i2s_tx));
static void *tx_block[NUM_BLOCKS];

static void fill_buf(int16_t *tx_block, int att)
{
    int r_idx;

    for (int i = 0; i < SAMPLE_NO; i++) {
        // left channel is sine wave
        tx_block[2 * i] = data[i] / (1 << att);
        // right channel is sine wave shifted by 90 degrees
        r_idx = (i + (ARRAY_SIZE(data) / 4)) % ARRAY_SIZE(data);
        tx_block[2 * i + 1] = data[r_idx] / (1 << att);
    }
}

int sound_init(void)
{
    if (!device_is_ready(dev_i2s)) {
        printf("I2S device not ready\n");
        return -ENODEV;
    }

    // configure i2s stream
    struct i2s_config i2s_cfg = {
        .word_size = 16U,
        .channels = 2U,
        .format = I2S_FMT_DATA_FORMAT_I2S,
        .frame_clk_freq = 44100,
        .block_size = BLOCK_SIZE,
        .timeout = 2000,
        .options = I2S_OPT_FRAME_CLK_MASTER | I2S_OPT_BIT_CLK_MASTER,
        .mem_slab = &tx_0_mem_slab
    };

    int ret = i2s_configure(dev_i2s, I2S_DIR_TX, &i2s_cfg);
    if (ret < 0) {
        printf("Failed to configure I2S stream\n");
        return ret;
    }

    // prepare all TX blocks
    for (uint32_t tx_idx = 0; tx_idx < NUM_BLOCKS; tx_idx++) {
        ret = k_mem_slab_alloc(&tx_0_mem_slab, &tx_block[tx_idx], K_FOREVER);
        if (ret < 0) {
            printf("Failed to allocate TX block\n");
            return ret;
        }
        fill_buf((uint16_t *)tx_block[tx_idx], tx_idx % 3);
    }

    return 0;
}

int sound_play(void)
{
    int ret;
    uint32_t tx_idx = 0;

    // send first block
    ret = i2s_write(dev_i2s, tx_block[tx_idx++], BLOCK_SIZE);
    if (ret < 0) {
        printf("Could not write TX buffer %d\n", tx_idx);
        return ret;
    }

    ret = i2s_trigger(dev_i2s, I2S_DIR_TX, I2S_TRIGGER_START);
    if (ret < 0) {
        printf("Could not trigger I2S tx\n");
        return ret;
    }

    // send remaining blocks
    for (; tx_idx < NUM_BLOCKS; tx_idx++) {
        ret = i2s_write(dev_i2s, tx_block[tx_idx], BLOCK_SIZE);
        if (ret < 0) {
            printf("Could not write TX buffer %d\n", tx_idx);
            return ret;
        }
    }

    ret = i2s_trigger(dev_i2s, I2S_DIR_TX, I2S_TRIGGER_DRAIN);
    if (ret < 0) {
        printf("Could not trigger I2S tx\n");
        return ret;
    }

    printf("Sound played successfully\n");
    return 0;
}

void sound_cleanup(void)
{
    /* Free allocated blocks */
    for (uint32_t tx_idx = 0; tx_idx < NUM_BLOCKS; tx_idx++) {
        k_mem_slab_free(&tx_0_mem_slab, &tx_block[tx_idx]);
    }
}