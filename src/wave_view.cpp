#include <math.h>
#include "wave_view.h"

wave_view::wave_view()
{
    _ring_buffer.set_max_size(_window_size);
    _skip = _duration * _sample_rate / _window_size;
}

wave_view::~wave_view()
{
}

void wave_view::set_window_size(std::uint32_t window_size, std::uint32_t sample_rate, float duration)
{
    _window_size = window_size;
    _sample_rate = sample_rate;
    _duration = duration;
    
    _ring_buffer.set_max_size(_window_size);
    _skip = _duration * _sample_rate / _window_size;
}

void wave_view::put_sample(float v)
{
    v = v * powf(10.0f, 0.05 * _gain_db);
    if (v > _max)
    {
        _max = v;
    }
        
    if (v < _min)
    {
        _min = v;
    }
    
    if (++_count >= _skip)
    {
        _count = 0;
        _ring_buffer.put(_max);
        _ring_buffer.put(_min);
        _max = -1.0;
        _min = 1.0;
    }
}

std::uint32_t wave_view::read_wave(float * buf, std::uint32_t max_cnt)
{
    return _ring_buffer.read(buf, max_cnt);
}