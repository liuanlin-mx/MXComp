#include <assert.h>
#include "fft.h"

fft::fft(std::uint32_t fft_bufsize, fft_window_type window_type)
    : _fft_bufsize(fft_bufsize)
    , _window_type(window_type)
    , _fft_data_in(NULL)
    , _fft_data_out(NULL)
    , _fft_power(NULL)
    , _fft_db(NULL)
    , _window(NULL)
    , _fft_plan(NULL)
{
    _init_fft();
}

fft::~fft()
{
    _uninit_fft();
}

void fft::reset(std::uint32_t fft_bufsize, fft_window_type window_type)
{
    if (fft_bufsize != _fft_bufsize || _window_type != window_type)
    {
        _uninit_fft();
        _fft_bufsize = fft_bufsize;
        _window_type = window_type;
        _init_fft();
    }
}


void fft::calc_fft(const float *data, std::uint32_t datalen)
{
    std::uint32_t s;
    datalen = (datalen > _fft_bufsize)? _fft_bufsize: datalen;
    
    if (_window_type != FFT_WINDOW_NONE)
    {
        for (s = 0; s < datalen; ++s)
        {
            _fft_data_in[s] = data[s] * _window[s];
        }
    }
    else
    {
        for (s = 0; s < datalen; ++s)
        {
            _fft_data_in[s] = data[s];
        }
    }
    
    for (; s < _fft_bufsize; ++s)
    {
        _fft_data_in[s] = 0.;
    }
    
    fftwf_execute(_fft_plan);
}


void fft::calc_power()
{

    _fft_power[0] = _fft_data_out[0] * _fft_data_out[0];
    
#define FRe (_fft_data_out[i])
#define FIm (_fft_data_out[_fft_bufsize - i])
	for (std::uint32_t i = 1; i < _fft_bufsize / 2 - 1; ++i)
    {
        _fft_power[i] = (FRe * FRe) + (FIm * FIm);
    }
#undef FRe
#undef FIm

}

void fft::calc_db()
{
    for (std::uint32_t i = 0; i < _fft_bufsize / 2; i++)
    {
        _fft_db[i] = _fft_power_at_bin(i, 1.0);
    }
}

void fft::calc_phase()
{
    _fft_phase[0] = 0;
#define FRe (_fft_data_out[i])
#define FIm (_fft_data_out[_fft_bufsize - i])
	for (std::uint32_t i = 1; i < _fft_bufsize / 2 - 1; ++i)
    {
        _fft_phase[i] = atan2(FIm, FRe);
    }
#undef FRe
#undef FIm
}

void fft::_init_fft()
{
    if (_fft_plan)
    {
        return;
    }
    
    _fft_data_in  = (float *)fftwf_malloc(sizeof(float) * _fft_bufsize);
    _fft_data_out = (float *)fftwf_malloc(sizeof(float) * _fft_bufsize);
    _fft_power    = (float *)fftwf_malloc(sizeof(float) * _fft_bufsize);
    _fft_db       = (float *)fftwf_malloc(sizeof(float) * _fft_bufsize);
    _fft_phase    = (float *)fftwf_malloc(sizeof(float) * _fft_bufsize);
    _window  = (float *)fftwf_malloc(sizeof(float) * _fft_bufsize);
    _fft_plan = fftwf_plan_r2r_1d(_fft_bufsize, _fft_data_in, _fft_data_out, FFTW_R2HC, FFTW_MEASURE);

    if (_window_type == FFT_WINDOW_HANN)
    {
        double sum = 0.0;
        for (std::uint32_t i = 0; i < _fft_bufsize; ++i)
        {
            _window[i] = 0.5f - (0.5f * (float)cos(2.0f * M_PI * (float)i / (float)(_fft_bufsize)));
            sum += _window[i];
        }
        const double isum = 2.0 / sum;
        for (std::uint32_t i = 0; i < _fft_bufsize; ++i)
        {
            _window[i] *= isum;
        }
    }
}

void fft::_uninit_fft()
{
    if (_fft_plan)
    {
        fftwf_destroy_plan (_fft_plan);
        fftwf_free(_fft_data_in);
        fftwf_free(_fft_data_out);
        fftwf_free(_fft_power);
        fftwf_free(_fft_db);
        fftwf_free(_fft_phase);
        fftwf_free(_window);
        
        _fft_data_in = NULL;
        _fft_data_out = NULL;
        _fft_power = NULL;
        _fft_db = NULL;
        _fft_phase = NULL;
        _window = NULL;
        _fft_plan = NULL;
    }
}