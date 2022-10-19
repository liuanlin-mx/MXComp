#include <stdio.h>
#include "compressor.h"

compressor::compressor()
    : _sample_rate(44100.)
    , _attack_ms(5.)
    , _release_ms(100.)
    , _attack(0.)
    , _release(0.)
    , _knee_db(3.)
    , _ratio(4.)
    , _thresh_db(0.)
    , _makeup_db(0.)
    , _rms_size_ms(0.)
    , _pre_size_ms(0.)
    , _last_gr(0.)
    , _rms(88200)
    , _delay(88200)
{
    
    _attack = expf(-1.0 * M_PI * 1000.0 / _attack_ms / _sample_rate);
    _release = expf(-1.0 * M_PI * 1000.0 / _release_ms / _sample_rate);
    
    _rms.set_size(1);
    _delay.set_delay(0);
}


compressor::~compressor()
{
}


std::int32_t compressor::get_ratio_map(float *map_in, float *map_out, float min, float max, float setup, std::int32_t buf_size)
{
    std::int32_t idx = 0;
    for (; min < max && idx < buf_size; min += setup)
    {
        float in = _to_amp(min);
        map_in[idx] = min;
        map_out[idx] = _to_db(in * _to_amp(-_calc_gr(in))) + _makeup_db;
        idx++;
    }
    return idx;
}

float compressor::process(float in, float side)
{
    _rms.put(side);
    float gr = _calc_gr_attack_rels(_rms.get_rms_value());
    _last_gr = gr;
    return _delay.process(in) * _to_amp(-gr) *_to_amp(_makeup_db);
}


float compressor::_calc_gr(float in)
{
    float width = _knee_db;
    
    float v_db = fabsf(in);
    v_db = (v_db == 0.)? -200.: _to_db(v_db);
    
    float tmp;
    
    float x = v_db - _thresh_db;
    if (2.0 * x < -width)
    {
        tmp = v_db;
    }
    else if (2.0 * x > width)
    {
        tmp = _thresh_db + x / _ratio;
    }
    else
    {
        tmp = v_db + (1.0 / _ratio - 1.0) * (x + width / 2.0) * (x + width / 2.0) / (2.0 * width);
        //float slope = 1.0 / (_ratio - 1.0);
        //tmp = x + slope * pow(x + width / 2.0, 2) / (2.0 * width);
    }

    tmp = (isnormal(tmp))? tmp: 0.;
    float gr = v_db - tmp;
    return gr;
}

float compressor::_calc_gr_attack_rels(float in)
{
    float gr = _calc_gr(in);
    if (gr < _last_gr)
    {
        gr = _release * _last_gr + (1.0 - _release) * gr;
    }
    else if (gr > _last_gr)
    {
        gr = _attack * _last_gr + (1.0 - _attack) * gr;
    }

    gr = (isnormal(gr))? gr: 0.;
    return gr;
}