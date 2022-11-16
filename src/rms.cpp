#include <stdlib.h>
#include <math.h>
#include "rms.h"

rms::rms(std::uint32_t max_size)
    : _max_size(max_size)
    , _size(1)
    , _idx(0)
    , _data(0)
    , _sum(0)
    , _sum_sub(0)
    , _cnt(0)
{
    _data = (float *)calloc(max_size, sizeof(float));
}


rms::~rms()
{
    free(_data);
}


void rms::set_size(std::uint32_t size)
{
    if (size == 0)
    {
        size = 1;
    }
    
    if (size > _max_size)
    {
        size = _max_size;
    }
    
    _sum = 0;
    _sum_sub = 0;
    _size = size;
    _cnt = 0;
    for (std::uint32_t i = 0; i < _size; i++)
    {
        _data[i] = 0;
    }
}


void rms::put(float in)
{
    std::uint32_t idx = _idx % _size;
    _idx++;
        
    _sum_sub += _data[idx] * _data[idx];
    _sum += in * in;
        
    _data[idx] = in;
    
    if (++_cnt > _size * 4)
    {
        _cnt = 0;
        _sum = 0;
        _sum_sub = 0;
        for (std::uint32_t i = 0; i < _size; i++)
        {
            _sum += _data[(_idx + i) % _size] * _data[(_idx + i) % _size];
        }
    }
}


float rms::get_median_value()
{
    std::uint32_t idx = (_idx + _size / 2) % _size;
    return _data[idx];
}

float rms::get_rms_value()
{
    return sqrtf((_sum - _sum_sub) / _size);
}
