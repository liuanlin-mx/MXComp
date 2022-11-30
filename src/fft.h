#include <fftw3.h>
#include <cstdint>
#include <math.h>

#ifndef __FFT_H__
#define __FFT_H__

class fft
{
public:
    enum fft_window_type
    {
        FFT_WINDOW_NONE = 0,
        FFT_WINDOW_HANN,
    };
    
public:
    fft(std::uint32_t fft_bufsize = 1024, fft_window_type window_type = FFT_WINDOW_HANN);
    ~fft();
    
    
public:
    void reset(std::uint32_t fft_bufsize, fft_window_type window_type = FFT_WINDOW_HANN);
    void calc_fft(const float *data, std::uint32_t datalen);
    void calc_fft(std::uint32_t datalen);
    void calc_power();
    void calc_db();
    void calc_phase();
    
    float *get_fft_data_in() { return _fft_data_in; }
    std::uint32_t get_fft_data_size() { return _fft_bufsize; }
    
    float *get_fft_data() { return _fft_data_out; }
    std::uint32_t get_fft_data_len() { return _fft_bufsize; }
 
    float *get_power() { return _fft_power; }
    std::uint32_t get_power_data_len() { return _fft_bufsize / 2; }
    float *get_db() { return _fft_db; }
    std::uint32_t get_db_data_len() { return _fft_bufsize / 2; }
    float *get_phase() { return _fft_phase; }
    std::uint32_t get_phase_data_len() { return _fft_bufsize / 2; }
    
    float fft_power_at_bin(const uint32_t b, const float norm)
    {
        return _fft_power_at_bin(b, norm);
    }
private:
    void _init_fft();
    void _uninit_fft();
    
    inline float _fft_power_at_bin(const uint32_t b, const float norm)
    {
        const float a = _fft_power[b] * norm;
        return (a > 1e-12)? 10.0 * log10f(a): -INFINITY;
    }
    
private:
    std::uint32_t _fft_bufsize;
    fft_window_type _window_type;
    
    float *_fft_data_in;
    float *_fft_data_out;
    float *_fft_power;
    float *_fft_db;
    float *_fft_phase;
    float *_window;
    
    fftwf_plan _fft_plan;
};

#endif