#include "UIManager.h"

void UIManager::glfw_error_callback(int error, const char* description) {
    Logger::error("GLFW", "Error " + std::to_string(error) + ": " + description);
}

UIManager::UIManager() : window(nullptr), io(nullptr), should_close(false) {
}

UIManager::~UIManager() {
    shutdown();
}

bool UIManager::initialize(const std::string& window_title, int width, int height) {
    glfwSetErrorCallback(glfw_error_callback);
    
    if (!glfwInit()) {
        Logger::error("UI", "failed to initialize glfw");
        return false;
    }
    
    // request opengl 3.2 core profile
    const char* glsl_version = "#version 150"; // glsl 150 for opengl 3.2
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2); // use opengl 3.2
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // request core profile
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // needed for macos
    
    window = glfwCreateWindow(width, height, window_title.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        Logger::error("UI", "failed to create window");
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    Logger::info("UI", "initialized imgui ui");
    return true;
}

void UIManager::shutdown() {
    if (window) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        
        glfwDestroyWindow(window);
        glfwTerminate();
        window = nullptr;
        io = nullptr;
        
        Logger::info("UI", "shut down imgui ui");
    }
}

bool UIManager::begin_frame() {
    if (window == nullptr) return false;
    
    should_close = glfwWindowShouldClose(window);
    if (should_close) return false;
    
    glfwPollEvents();
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    return true;
}

void UIManager::end_frame() {
    if (window == nullptr) return;
    
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    glfwSwapBuffers(window);
}

bool UIManager::should_close_window() const {
    return should_close;
}

GLFWwindow* UIManager::get_window() const {
    return window;
} 