#ifndef __COMPRESSOR_H__
#define __COMPRESSOR_H__
#include <math.h>
#include "rms.h"
#include "delay.h"
#include "ring_buffer.h"

class compressor
{
public:
    compressor();
    ~compressor();
    
public:
    void set_sample_rate(float sample_rate)
    {
        _sample_rate = sample_rate;
        
        _attack = expf(-1.0 * M_PI * 1000.0 / _attack_ms / _sample_rate);
        _release = expf(-1.0 * M_PI * 1000.0 / _release_ms / _sample_rate);
        
        _rms.set_size(_rms_size_ms * _sample_rate / 1000.);
        _delay.set_delay(_pre_size_ms * _sample_rate / 1000.);
    }
    
    void set_thresh(float thresh_db)
    {
        _thresh_db = thresh_db;
    }
    
    void set_ratio(float ratio)
    {
        _ratio = ratio;
    }
    
    void set_knee(float knee_db)
    {
        _knee_db = knee_db;
    }
    
    void set_gain(float gain_db)
    {
        _makeup_db = gain_db;
    }
    
    void set_attack(float attack_ms)
    {
        _attack_ms = attack_ms;
        _attack = expf(-1.0 * M_PI * 1000.0 / _attack_ms / _sample_rate);
    }
    
    void set_release(float rels_ms)
    {
        _release_ms = rels_ms;
        _release = expf(-1.0 * M_PI * 1000.0 / _release_ms / _sample_rate);
    }
    
    void set_rms(float rms_size_ms)
    {
        _rms_size_ms = rms_size_ms;
        _rms.set_size(_rms_size_ms * _sample_rate / 1000.);
    }
    
    void set_pre(float pre_size_ms)
    {
        _pre_size_ms = pre_size_ms;
        _delay.set_delay(_pre_size_ms * _sample_rate / 1000.);
    }
    
    float get_latency()
    {
        return _pre_size_ms * _sample_rate / 1000.;
    }
    
    std::int32_t get_ratio_map(float *map_in, float *map_out, float min, float max, float setup, std::int32_t buf_size);
    
    float get_gr_db()
    {
        return _last_gr;
    }
    
    float process(float main, float side);
    
private:
    inline float _to_db(float v)
    {
        return 20. * log10(v);
    }
    
    inline float _to_amp(float db)
    {
        return powf(10.0f, 0.05 * db);
    }
    
    float _calc_gr(float in);
    float _calc_gr_attack_rels(float in);
    
private:
    float _sample_rate;
    float _attack_ms;
    float _release_ms;
    float _attack;
    float _release;
    float _knee_db;
    float _ratio;
    float _thresh_db;
    float _makeup_db;
    float _rms_size_ms;
    float _pre_size_ms;
    float _last_gr;
    
    rms _rms;
    delay _delay;
};

#endif