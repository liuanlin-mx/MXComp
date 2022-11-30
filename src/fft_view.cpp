#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include "fft_view.h"

fft_view::fft_view()
    : _fft(_fft_size)
{
    _ring_buffer.set_max_size(_fft_size);
    _skip = std::max(64u, (std::uint32_t)(_duration * _sample_rate / _window_width));
    _image = (std::uint8_t *)malloc(_window_width * _window_height * 3);
    memset(_image, 0, _window_width * _window_height * 3);
    _make_color_map(_color_map, fft_range_db);
    _make_mel_scale_table(_y_scale_table, _sample_rate, _fft_size / 2);
}

fft_view::~fft_view()
{
    if (_image)
    {
        free(_image);
    }
}

void fft_view::set_fft_size(std::uint32_t fft_size)
{
    _fft_size = fft_size;
    _ring_buffer.set_max_size(_fft_size);
    _fft.reset(_fft_size);
    if (_image)
    {
        free(_image);
    }
    _image = (std::uint8_t *)malloc(_window_width * _window_height * 3);
    memset(_image, 0, _window_width * _window_height * 3);
}

void fft_view::set_window_size(std::uint32_t window_width, std::uint32_t window_height)
{
    _window_width = window_width;
    _window_height = window_height;
    
    _skip = std::max(64u, (std::uint32_t)(_duration * _sample_rate / _window_width));
    if (_image)
    {
        free(_image);
    }
    _image = (std::uint8_t *)malloc(_window_width * _window_height * 3);
    memset(_image, 0, _window_width * _window_height * 3);
}

void fft_view::set_sample_rate(std::uint32_t sample_rate)
{
    _sample_rate = sample_rate;
    _skip = std::max(64u, (std::uint32_t)(_duration * _sample_rate / _window_width));
}

void fft_view::set_duration(float duration)
{
    _duration = duration;
    _skip = std::max(64u, (std::uint32_t)(_duration * _sample_rate / _window_width));
}

void fft_view::put_sample(float v)
{
#define _R(w, h) _image[(h * _window_width + w) * 3]
#define _G(w, h) _image[(h * _window_width + w) * 3 + 1]
#define _B(w, h) _image[(h * _window_width + w) * 3 + 2]

    _ring_buffer.put(v);
    if (++_count >= _skip)
    {
        _count = 0;
        
        std::uint32_t len = _ring_buffer.read(_fft.get_fft_data_in(), _fft.get_fft_data_size());
        _fft.calc_fft(len);
        _fft.calc_power();
        
        std::uint32_t w = _image_col_idx;
        if (++_image_col_idx >= _window_width)
        {
            _image_col_idx = 0;
        }
        
        for (std::uint32_t i = 0; i < _fft_size / 2; i++)
        {
            float level = _fft.fft_power_at_bin(i, 1);
            if (level > 0.0)
            {
                level = 0.0;
            }
            float fft_data_size = _fft_size / 2;
            
            std::int32_t y0 = floorf(_y_scale_table[i] * (float)_window_height / fft_data_size);
            std::int32_t y1 = floorf(_y_scale_table[i + 1] * (float)_window_height / fft_data_size);
            if (y0 == y1)
            {
                continue;
            }
            
            std::uint32_t idx = ((std::uint32_t)floorf(-level)) * 3;
            if (level < -fft_range_db)
            {
                idx = fft_range_db * 3;
            }
            
            for (std::int32_t y = y0; y < y1 && y < (std::int32_t)_window_height; ++y)
            {
                std::int32_t yy = _window_height - 1 - y;
                _R(w, yy) = _color_map[idx];
                _G(w, yy) = _color_map[idx + 1];
                _B(w, yy) = _color_map[idx + 2];
            }
        }
        
    }
}

    
void fft_view::read_rgb_image(std::uint8_t *image)
{
    std::uint32_t w = _image_col_idx;
    for (std::uint32_t y = 0; y < _window_height; y++)
    {
        std::uint8_t *src = _image + (y * _window_width) * 3;
        std::uint8_t *dst = image + (y * _window_width) * 3;
        memcpy(dst, src + w * 3, (_window_width - w) * 3);
        memcpy(dst + (_window_width - w) * 3, src, w * 3);
    }
}

/* h(0 ~ 360) s(0 ~ 1) v (0 ~ 1)  R(0 ~ 1) G (0 ~ 1) B(0 ~ 1) */
void fft_view::_hsv_to_rgb(int h, float s, float v, float *R, float *G, float *B)
{
    float C = 0;
    float X = 0;
    float Y = 0;
    float Z = 0;
    int i = 0;
    float H = (float)(h);
    float S = (float)(s);
    float V = (float)(v);

    if(S <= FLT_MIN)
    {
        *R = *G = *B = V;
    }
    else
    {
        H = H / 60;
        i = (int)H;
        C = H - i;

        X = V * (1 - S);
        Y = V * (1 - S * C);
        Z = V * (1 - S * (1 - C));
        switch(i)
        {
            case 6:
            case 0: *R = V; *G = Z; *B = X; break;
            case 1: *R = Y; *G = V; *B = X; break;
            case 2: *R = X; *G = V; *B = Z; break;
            case 3: *R = X; *G = Y; *B = V; break;
            case 4: *R = Z; *G = X; *B = V; break;
            case 5: *R = V; *G = X; *B = Y; break;
        }
    }
}



void fft_view::_make_color_map(std::uint8_t *map, std::int32_t fft_range_db)
{
    for (std::int32_t i = 0; i <= fft_range_db; ++i)
    {
        std::int32_t level = fft_range_db - i;
        std::int32_t tmp = floorf(level * (180. / fft_range_db));
        float h = (270 + (tmp * tmp) / 180) % 360;
        float v = (float)(level * level) / (fft_range_db * fft_range_db);
        float r;
        float g;
        float b;
        _hsv_to_rgb(h, 0.9, v, &r, &g, &b);
        map[i * 3] = r * 255;
        map[i * 3 + 1] = g * 255;
        map[i * 3 + 2] = b * 255;
    }
}


float fft_view::_hz_to_mel(float hz)
{
    return 1127 * log(1 + hz / 700);
}

void fft_view::_make_mel_scale_table (std::uint16_t *scale, const float sample_rate, const std::uint32_t fft_data_size)
{
    const std::uint32_t fft_bufsize = fft_data_size * 2;
    const float max_mel = _hz_to_mel(sample_rate / 2);

    for (std::uint32_t i = 0; i < fft_data_size; ++i)
    {
        float mel = _hz_to_mel (i * sample_rate / fft_bufsize);
        scale[i] = std::min(fft_data_size - 1u, (std::uint32_t)floorf(mel * (fft_data_size) / max_mel));
    }
}