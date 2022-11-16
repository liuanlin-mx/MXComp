#ifndef __RMS_H__
#define __RMS_H__
#include <cstdint>

class rms
{
public:
    rms(std::uint32_t max_size = 44100);
    ~rms();
    
public:
    void set_size(std::uint32_t size);
    void put(float in);
    float get_median_value();
    float get_rms_value();
    
private:
    std::uint32_t _max_size;
    std::uint32_t _size;
    std::uint32_t _idx;
    float *_data;
    float _sum;
    float _sum_sub;
    std::uint32_t _cnt;
};

#endif