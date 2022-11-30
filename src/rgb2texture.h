#ifndef __RGB2TEXTURE_H__
#define __RGB2TEXTURE_H__
#include <cstdint>
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

class rgb2texture
{
public:
    rgb2texture();
    ~rgb2texture();
    
public:
    void init();
    void uninit();
    GLuint load(std::uint8_t *rgb, std::int32_t w, std::int32_t h);
    
private:
    GLuint _texture = 0;
    bool _inited = false;
};

#endif