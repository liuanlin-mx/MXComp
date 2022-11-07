#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__
#include <cstdint>
class ring_buffer
{
public:
    explicit ring_buffer(std::uint32_t max_size = 4410);
    ~ring_buffer();
    
public:
    void put(float v);
    std::uint32_t read(float *buf, std::uint32_t max_cnt);
    
private:
    std::uint32_t _max_size;
    std::uint32_t _in;
    float *_data;
};

#endif