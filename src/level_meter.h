#ifndef __LEVEL_METER_H__
#define __LEVEL_METER_H__

#include "rms.h"

class level_meter
{
public:
    level_meter();
    ~level_meter();
    
public:
    void set_rms_size(std::uint32_t rms_size);
    void put(float v);
    
    float get_peek_db();
    float get_rms_db();
    
private:
    rms _rms;
    float _peek;
    std::uint32_t _count;
};

#endif