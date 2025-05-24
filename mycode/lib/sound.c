void generate_sine_wave(int16_t *buffer, size_t count, uint16_t frequency)
{
    for (size_t i = 0; i < count; i++) {
        buffer[i] = INT16_MAX * sin(2 * M_PI * frequency * i / SAMPLE_RATE);
    }
}
