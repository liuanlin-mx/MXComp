#include <math.h>
#include "level_meter.h"

level_meter::level_meter()
    : _rms(1000)
    , _peek(0)
    , _count(0)
{
    _rms.set_size(300);
}

level_meter::~level_meter()
{
}

void level_meter::set_rms_size(std::uint32_t rms_size)
{
    _rms.set_size(rms_size);
}

void level_meter::put(float v)
{
    _rms.put(v);
    
    if (fabsf(v) > _peek)
    {
        _peek = fabsf(v);
    }
    if (++_count > 44100)
    {
        _count = 0;
        _peek = fabsf(v);
    }
}


float level_meter::get_peek_db()
{
    return 20. * log10(_peek);
}

float level_meter::get_rms_db()
{
    return 20. * log10(_rms.get_rms_value());
}
