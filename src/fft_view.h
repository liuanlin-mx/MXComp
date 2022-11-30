#ifndef __FFT_VIEW_H__
#define __FFT_VIEW_H__
#include "ring_buffer.h"
#include "fft.h"

class fft_view
{
    enum
    {
        fft_range_db = 120
    };
public:
    fft_view();
    ~fft_view();
    
public:
    void set_fft_size(std::uint32_t fft_size);
    void set_window_size(std::uint32_t window_width, std::uint32_t window_height);
    void set_sample_rate(std::uint32_t sample_rate);
    void set_duration(float duration);
    
    void put_sample(float v);
    
    std::int32_t get_image_width() { return _window_width; }
    std::int32_t get_image_height() { return _window_height; }
    void read_rgb_image(std::uint8_t *image);
    
private:
    void _hsv_to_rgb(int h, float s, float v, float *R, float *G, float *B);
    void _make_color_map(std::uint8_t *map, std::int32_t fft_range_db);
    
    float _hz_to_mel(float hz);
    void _make_mel_scale_table (std::uint16_t *scale, const float sample_rate, const std::uint32_t fft_data_size);
private:
    ring_buffer _ring_buffer;
    std::uint32_t _sample_rate = 44100;
    std::uint32_t _fft_size = 1024;
    std::uint32_t _window_width = 640;
    std::uint32_t _window_height = 240;
    float _duration = 10;
    std::uint32_t _count = 0;
    std::uint32_t _skip = 0;
    fft _fft;
    std::uint8_t *_image = NULL;
    std::uint32_t _image_col_idx = 0;
    std::uint8_t _color_map[(fft_range_db + 1) * 3];
    std::uint16_t _y_scale_table[4096];
};

#endif