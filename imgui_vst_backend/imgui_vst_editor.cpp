#include <stdio.h>
#include <atomic>
#include <mutex>
#include "imgui_vst_editor.h"

thread_local ImGuiContext* vst_imgui;

std::mutex imgui_vst_editor::_global_lock;
std::atomic<std::int32_t> imgui_vst_editor::_glfw_init(0);

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

imgui_vst_editor::imgui_vst_editor(AudioEffect* effect, std::int32_t w, std::int32_t h)
    : AEffEditor(effect)
    , _imgui_ctx(NULL)
    , _window(NULL)
    , _parent_window(NULL)
    , _running(false)
{
    _rect.left = 0;
    _rect.right = w;
    _rect.top = 0;
    _rect.bottom = h;
}


imgui_vst_editor::~imgui_vst_editor()
{
}

bool imgui_vst_editor::getRect(ERect** rect)
{
    *rect = &_rect;
    return true;
}

bool imgui_vst_editor::open(void* ptr)
{
    if (_running)
    {
        return false;
    }
    _running = true;
    _parent_window = ptr;
    
    _thread.reset(new std::thread([this] {
        _run();
    }));
    
    return true;
}

void imgui_vst_editor::close()
{
    if (_running)
    {
        _running = false;
        _thread->join();
        _thread.reset();
    }
}

void imgui_vst_editor::idle()
{
}

void imgui_vst_editor::draw(std::int32_t w, std::int32_t h)
{
    {
        ImGui::Begin("Hello, world!", NULL,
                        ImGuiWindowFlags_NoTitleBar 
                        | ImGuiWindowFlags_NoResize
                        | ImGuiWindowFlags_NoMove);                          // Create a window called "Hello, world!" and append into it.
            
        ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(w, h), ImGuiCond_Always);
        
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
}


bool imgui_vst_editor::_init_opengl()
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        return false;
    }
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    return true;
}

bool imgui_vst_editor::_create_window_opengl()
{
    // Create window with graphics context
    _window = glfwCreateWindow(_rect.right - _rect.left, _rect.bottom - _rect.top, "Vst Editor", NULL, NULL);
    if (_window == NULL)
    {
        return false;
    }
    
    if (_parent_window)
    {
#ifdef _WIN32
        SetParent(glfwGetWin32Window(_window), (HWND)_parent_window);
#else
        Window x11_window = glfwGetX11Window(_window);
        XReparentWindow(glfwGetX11Display(), x11_window, (Window)_parent_window, 0, 0);
        XSync(glfwGetX11Display(), false);
        XFlush(glfwGetX11Display());
#endif
    }
    
    glfwMakeContextCurrent(_window);
    glfwSwapInterval(1); // Enable vsync
    return true;
}



bool imgui_vst_editor::_setup_imgui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    _imgui_ctx = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;
    io.LogFilename = NULL;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL2_Init();
    
    //ImFontConfig config;
    //io.Fonts->AddFontFromMemoryCompressedTTF(font_compressed_data, font_compressed_size, 16 , &config);
    return true;
}

void imgui_vst_editor::_run()
{
    {
        std::lock_guard<std::mutex> l(_global_lock);
        if (_glfw_init.fetch_add(1) == 0)
        {
            _init_opengl();
        }
        _create_window_opengl();
        _setup_imgui();
        draw_init();
    }
    
    GLFWwindow *window = _window;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    // Main loop
    while (_running)
    {
        double time = glfwGetTime();
        _global_lock.lock();
        
        ImGui::SetCurrentContext(_imgui_ctx);
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        ImGui_ImplGlfw_PollEvents();
        glfwPollEvents();

#ifndef _WIN32
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureMouse && io.WantTextInput)
        {
            if (!glfwGetWindowAttrib(window, GLFW_FOCUSED))
            {
                XSetInputFocus(glfwGetX11Display(), glfwGetX11Window(window), RevertToParent, CurrentTime);
            }
        }
#endif
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        draw(_rect.right - _rect.left, _rect.bottom - _rect.top);
        
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        // If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
        // you may need to backup/reset/restore other state, e.g. for current shader using the commented lines below.
        //GLint last_program;
        //glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
        //glUseProgram(0);
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        //glUseProgram(last_program);

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
        
        ImGui::SetCurrentContext(NULL);
        _global_lock.unlock();
        
        std::int32_t ms = (glfwGetTime() - time) * 1000;
        std::this_thread::sleep_for(std::chrono::milliseconds(std::max(33 - ms, 1)));
    }

    std::lock_guard<std::mutex> l(_global_lock);
    ImGui::SetCurrentContext(_imgui_ctx);
    
    draw_uninit();
    
    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    
    if (_glfw_init.fetch_add(-1) <= 1)
    {
        glfwTerminate();
    }
}

int main()
{
    imgui_vst_editor ui;
    ui.open(NULL);
    getchar();
    ui.close();
    getchar();
}