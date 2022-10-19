#ifndef __IMGUI_VST_EDITOR_H__
#define __IMGUI_VST_EDITOR_H__

#include <thread>
#include <mutex>
#include <memory>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include "pluginterfaces/vst2.x/aeffeditor.h"

class imgui_vst_editor: public AEffEditor
{
public:
    imgui_vst_editor(AudioEffect* effect = 0, std::int32_t w = 640, std::int32_t h = 480);
    virtual ~imgui_vst_editor();
    
public:
    virtual bool getRect(ERect** rect);
    virtual bool open(void* ptr);
    virtual void close();
    virtual void idle();
    
    virtual void draw(std::int32_t w, std::int32_t h);
    
protected:
    GLFWwindow *get_window() { return _window; }
    
private:
    bool _init_opengl();
    bool _create_window_opengl();
    bool _setup_imgui();
    void _run();
    
private:
    ERect _rect;
    ImGuiContext* _imgui_ctx;
    
    GLFWwindow *_window;
    void* _parent_window;
    volatile bool _running;
    std::shared_ptr<std::thread> _thread;
    
    
    static std::mutex _global_lock;
    static std::atomic<std::int32_t> _glfw_init;
};

#endif