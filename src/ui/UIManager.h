#pragma once

#include <memory>
#include <string>
#include <functional>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include "../utils/Logger.h"

class UIManager {
private:
    GLFWwindow* window;
    ImGuiIO* io;
    bool should_close;
    
    static void glfw_error_callback(int error, const char* description);

public:
    UIManager();
    ~UIManager();
    
    bool initialize(const std::string& window_title, int width, int height);
    void shutdown();
    
    bool begin_frame();
    void end_frame();
    
    bool should_close_window() const;
    GLFWwindow* get_window() const;
}; 