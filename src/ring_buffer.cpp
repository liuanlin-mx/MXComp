#include <stdlib.h>
#include <string.h>
#include "ring_buffer.h"

ring_buffer::ring_buffer(std::uint32_t max_size)
    : _max_size(max_size)
    , _in(0)
{
    _data = (float *)malloc(max_size * sizeof(float));
    if (_data)
    {
        memset(_data, 0, max_size * sizeof(float));
    }
}

ring_buffer::~ring_buffer()
{
    if (_data)
    {
        free(_data);
    }
}

void ring_buffer::put(float v)
{
    _data[_in % _max_size] = v;
    _in++;
}

std::uint32_t ring_buffer::read(float * buf, std::uint32_t max_cnt)
{
    std::uint32_t in = _in;
    std::uint32_t i;
    for (i = 0; i < _max_size && i < max_cnt; i++)
    {
        buf[i] = _data[(in + i) % _max_size];
    }
    return i;
}