#ifndef __WAVE_VIEW_H__
#define __WAVE_VIEW_H__
#include "ring_buffer.h"
class wave_view
{
public:
    wave_view();
    ~wave_view();
    
public:
    void set_window_size(std::uint32_t window_size);
    void set_sample_rate(std::uint32_t sample_rate);
    void set_duration(float duration);
    void set_gain(float gain_db)
    {
        _gain_db = gain_db;
    }
    void put_sample(float v);
    std::uint32_t read_wave(float *buf, std::uint32_t max_cnt);
    
private:
    ring_buffer _ring_buffer;
    std::uint32_t _sample_rate = 44100;
    std::uint32_t _window_size = 1024;
    float _duration = 10;
    float _max = -1.0;
    float _min = 1.0;
    std::uint32_t _count = 0;
    std::uint32_t _skip = 0;
    float _gain_db = 0;
};

#endif