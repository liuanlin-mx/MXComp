#ifndef __EQ_ANALYSIS_H__
#define __EQ_ANALYSIS_H__
#include <cstdint>
#include <cstdint>
#include <math.h>
#include "fft.h"

class eq_analysis
{
    enum
    {
        SAMPLE_SIZE = 4096,
    };
    
public:
    eq_analysis();
    ~eq_analysis();
    
public:
    void set_sample_rate(float sample_rate);
    
    inline float get_sample(std::uint32_t idx) { return _raw_sample[idx]; }
    inline float set_sample(std::uint32_t idx, float v) { return _sample[idx] = v; }
    std::uint32_t get_sample_len() { return SAMPLE_SIZE; }
    
    void analysis();
    float *get_db() { return _fft_db; }
    float *get_phase() { return _fft_phase; }
    float *get_freq_scale() { return _freq_scale; }
    std::uint32_t get_result_len() { return SAMPLE_SIZE / 2; }
    
private:
    void _gen_white_noise();
    float _hz_to_mel(float hz);
    void _make_mel_scale_table(uint16_t *scale, float sample_rate, const uint32_t fft_data_size);
    
private:
    float _sample_rate = 44100.;
    float _raw_sample[SAMPLE_SIZE];
    float _sample[SAMPLE_SIZE];
    
    std::uint32_t _fft_bufsize = SAMPLE_SIZE;
    std::uint32_t _fft_data_size = SAMPLE_SIZE / 2;
    
    float _raw_fft_db[SAMPLE_SIZE];
    float _raw_fft_phase[SAMPLE_SIZE];
    float _fft_db[SAMPLE_SIZE];
    float _fft_phase[SAMPLE_SIZE];
    float _hann_window[SAMPLE_SIZE];
    float _freq_scale[SAMPLE_SIZE / 2];
    fft _fft;
};

#endif