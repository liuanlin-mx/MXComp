#include "eq_analysis.h"

eq_analysis::eq_analysis()
    : _fft(SAMPLE_SIZE, fft::FFT_WINDOW_NONE)
{
    _gen_white_noise();
    _fft.calc_fft(_raw_sample, SAMPLE_SIZE);
    _fft.calc_power();
    _fft.calc_db();
    _fft.calc_phase();
    std::uint32_t len = _fft.get_db_data_len();
    for (std::uint32_t i = 0; i < len; i++)
    {
        _raw_fft_db[i] = _fft.get_db()[i];
        _raw_fft_phase[i] = _fft.get_phase()[i] * 180 / M_PI;
    }
    
    for (std::int32_t i = 0; i < SAMPLE_SIZE / 2; i++)
    {
        _freq_scale[i] = i * _sample_rate / SAMPLE_SIZE;
    }
    //_make_mel_scale_table(_freq_scale, _sample_rate, SAMPLE_SIZE / 2);
}

eq_analysis::~eq_analysis()
{
}

void eq_analysis::set_sample_rate(float sample_rate)
{
    _sample_rate = sample_rate;
    for (std::int32_t i = 0; i < SAMPLE_SIZE / 2; i++)
    {
        _freq_scale[i] = i * _sample_rate / SAMPLE_SIZE;
    }
}

void eq_analysis::_gen_white_noise()
{
    float div = ((float) RAND_MAX) / 2.0f;
    for (std::uint32_t i = 0; i < sizeof(_raw_sample) / sizeof(_raw_sample[0]); i++)
    {
        _raw_sample[i] = 0.5 * ((rand() / div) - 1.0f);
        _sample[i] = _raw_sample[i];
    }
}

float eq_analysis::_hz_to_mel(float hz)
{
	return 1127 * log(1 + hz / 700);
}

void eq_analysis::_make_mel_scale_table(uint16_t *scale, float sample_rate, const uint32_t fft_data_size)
{
    const uint32_t fft_bufsize = fft_data_size * 2;
    const float max_mel = _hz_to_mel(sample_rate / 2);

    for (uint32_t i = 0; i < fft_data_size; ++i)
    {
        float mel = _hz_to_mel (i * sample_rate / fft_bufsize);
        scale[i] = std::fmin(fft_data_size - 1u, (uint32_t)floorf(mel * (fft_data_size) / max_mel));
    }
}

void eq_analysis::analysis()
{
    _fft.calc_fft(_sample, SAMPLE_SIZE);
    _fft.calc_power();
    _fft.calc_db();
    _fft.calc_phase();
    std::uint32_t len = _fft.get_db_data_len();
    
    _fft_db[0] = _fft.get_db()[0] - _raw_fft_db[0];
    _fft_db[1] = _fft.get_db()[1] - _raw_fft_db[1];
    
    for (std::uint32_t i = 2; i < len; i++)
    {
        _fft_db[i] = _fft.get_db()[i] - _raw_fft_db[i];
        if (isnormal(_fft_db[i - 1]) && isnormal(_fft_db[i - 2]))
        {
            _fft_db[i] = (_fft_db[i] + _fft_db[i - 1] + _fft_db[i - 2]) / 3;
        }
    }
    
    _fft_phase[0] = _fft.get_phase()[0] * 180 / M_PI - _raw_fft_phase[0];
    _fft_phase[1] = _fft.get_phase()[1] * 180 / M_PI - _raw_fft_phase[1];
    for (std::uint32_t i = 2; i < len; i++)
    {
        _fft_phase[i] = _fft.get_phase()[i] * 180 / M_PI - _raw_fft_phase[i];
        _fft_phase[i] = fmod(_fft_phase[i] + 720, 360);
        if (_fft_phase[i] > 180)
        {
            _fft_phase[i] = _fft_phase[i] - 360;
        }
    }
    
}
