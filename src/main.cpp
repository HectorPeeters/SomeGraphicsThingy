#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <iostream>
#include <fstream>
#include <ctime>

#include <sys/stat.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

const char *OPENGL_ERRORS[] = {"GL_INVALID_ENUM", "GL_INVALID_VALUE", "GL_INVALID_OPERATION", "GL_STACK_OVERFLOW", "GL_STACK_UNDERFLOW", "GL_OUT_OF_MEMORY", "GL_INVALID_FRAMEBUFFER_OPERATION"};

struct EngineState
{
    void (*imgui_draw)();
    void (*update)(float);
    void (*resize)(unsigned int, unsigned int);
    void (*render)();
    bool (*init)(void *);
    void (*deinit)();
    void *library_handle;
    int old_library_timestamp;

    std::time_t last_reload_notification_time;
};

struct SharedData
{
    unsigned int width;
    unsigned int height;

    double mouse_x;
    double mouse_y;
};

EngineState engine_state;
SharedData shared_data;

void log_error(const char *format, ...)
{
    va_list arg;

    va_start(arg, format);
    vfprintf(stderr, format, arg);
    va_end(arg);
}

void log(const char *format, ...)
{
    va_list arg;

    va_start(arg, format);
    vfprintf(stdout, format, arg);
    va_end(arg);
}

void gl_check_error_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
        log_error("OpenGL Error (%s)! %s:%d\n", OPENGL_ERRORS[errorCode - 1280], file, line);
}
#ifdef DEBUG
#define gl_check_error() gl_check_error_(__FILE__, __LINE__)
#else
#define gl_check_error()
#endif

void close_library()
{
    if (engine_state.library_handle != nullptr)
    {
        engine_state.deinit();

        dlclose(engine_state.library_handle);
        log("Closed library\n");
    }

    engine_state.imgui_draw = nullptr;
    engine_state.update = nullptr;
    engine_state.render = nullptr;
    engine_state.init = nullptr;
    engine_state.deinit = nullptr;
    engine_state.library_handle = nullptr;
}

bool load_library()
{
#ifdef DEBUG
    std::ifstream src("./bin/Debug/libGame.so", std::ios::binary);
    std::ofstream dst("./bin/Debug/libGame_temp.so", std::ios::binary);
#else
    std::ifstream src("./bin/Release/libGame.so", std::ios::binary);
    std::ofstream dst("./bin/Release/libGame_temp.so", std::ios::binary);
#endif

    dst << src.rdbuf();

    src.close();
    dst.close();

#ifdef DEBUG
    engine_state.library_handle = dlopen("./bin/Debug/libGame_temp.so", RTLD_LAZY);
#else
    engine_state.library_handle = dlopen("./bin/Release/libGame_temp.so", RTLD_LAZY);
#endif

    if (!engine_state.library_handle)
    {
        log_error("Library Error: %s\n", dlerror());
        return false;
    }

    *(void **)(&engine_state.imgui_draw) = dlsym(engine_state.library_handle, "imgui_draw");

    if (!engine_state.imgui_draw)
    {
        log_error("Library Error: %s\n", dlerror());
        close_library();
        return false;
    }

    *(void **)(&engine_state.update) = dlsym(engine_state.library_handle, "update");

    if (!engine_state.update)
    {
        log_error("Library Error: %s\n", dlerror());
        close_library();
        return false;
    }

    *(void **)(&engine_state.resize) = dlsym(engine_state.library_handle, "resize");

    if (!engine_state.resize)
    {
        log_error("Library Error: %s\n", dlerror());
        close_library();
        return false;
    }

    *(void **)(&engine_state.render) = dlsym(engine_state.library_handle, "render");

    if (!engine_state.render)
    {
        log_error("Library Error: %s\n", dlerror());
        close_library();
        return false;
    }

    *(void **)(&engine_state.init) = dlsym(engine_state.library_handle, "init");

    if (!engine_state.init)
    {
        log_error("Library Error: %s\n", dlerror());
        close_library();
        return false;
    }

    *(void **)(&engine_state.deinit) = dlsym(engine_state.library_handle, "deinit");

    if (!engine_state.deinit)
    {
        log_error("Library Error: %s\n", dlerror());
        close_library();
        return false;
    }

    if (!engine_state.init((void *)&shared_data))
    {
        log_error("Failed to init game\n");
        return false;
    }

    log("Loaded library\n");

    engine_state.last_reload_notification_time = std::time(nullptr);

    return true;
}

void check_library_reload()
{
    struct stat st;
    int ierr = stat("./bin/Debug/libGame.so", &st);
    if (ierr != 0)
    {
        return;
    }

    if (st.st_size == 0)
        return;

    int date = st.st_mtime;

    if (date != engine_state.old_library_timestamp)
    {
        close_library();

        load_library();
        // exit(0);

        engine_state.old_library_timestamp = date;
    }
}

void glfw_error_callback(int error, const char *description)
{
    log_error("GLFW Error(0x%x): %s\n", error, description);
}

void APIENTRY gl_debug_output(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    // if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    // return;

    log_error("---------------");
    log_error("Debug message (%d): %s", id, message);

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        log_error("Source: API");
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        log_error("Source: Window System");
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        log_error("Source: Shader Compiler");
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        log_error("Source: Third Party");
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        log_error("Source: Application");
        break;
    case GL_DEBUG_SOURCE_OTHER:
        log_error("Source: Other");
        break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        log_error("Type: Error");
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        log_error("Type: Deprecated Behaviour");
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        log_error("Type: Undefined Behaviour");
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        log_error("Type: Portability");
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        log_error("Type: Performance");
        break;
    case GL_DEBUG_TYPE_MARKER:
        log_error("Type: Marker");
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        log_error("Type: Push Group");
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        log_error("Type: Pop Group");
        break;
    case GL_DEBUG_TYPE_OTHER:
        log_error("Type: Other");
        break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        log_error("Severity: high");
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        log_error("Severity: medium");
        break;
    case GL_DEBUG_SEVERITY_LOW:
        log_error("Severity: low");
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        log_error("Severity: notification");
        break;
    }
}

bool init_glfw()
{
    if (!glfwInit())
    {
        log_error("GLFW Error: Failed to init GLFW\n");
        return false;
    }

    glfwSetErrorCallback(glfw_error_callback);

    const char *glfwVersion = glfwGetVersionString();
    log("Loaded GLFW (version %s)\n", glfwVersion);

    return true;
}

typedef GLFWwindow *Window;

void framebuffer_size_callback(Window window, int width, int height)
{
    glViewport(0, 0, width, height);
    shared_data.width = width;
    shared_data.height = height;

    engine_state.resize(width, height);
}

Window create_window()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

#ifdef DEBUG
    // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // int flags;
    // glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    // if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    // {
    //TODO: add logging back in
    // glEnable(GL_DEBUG_OUTPUT);
    // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    // glDebugMessageCallback(gl_debug_output, nullptr);
    // glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    // }

#endif

    Window window = glfwCreateWindow(640, 480, "Reloading", NULL, NULL);

    shared_data.width = 640;
    shared_data.height = 480;

    if (!window)
    {
        log_error("GLFW Error; Failed to create GLFW window\n");
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        log_error("GLAD Error: Failed to init GLAD\n");
        return nullptr;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSwapInterval(1);

    return window;
}

bool setup_imgui(Window window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430 core");

    ImGui::StyleColorsDark();

    log("Loaded ImGui (version %s)\n", IMGUI_VERSION);
}

void imgui_begin()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void imgui_end()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void draw_menu_bar()
{
    if (ImGui::BeginMainMenuBar())
    {
        bool red_reload = std::time(nullptr) - engine_state.last_reload_notification_time < 2;
        if (red_reload)
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(0.76f, 0.16f, 0.0f));

        if (ImGui::MenuItem("Reload"))
        {
            close_library();
            load_library();
        }

        if (red_reload)
            ImGui::PopStyleColor();

        ImGui::Spacing();

        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::EndMainMenuBar();
    }
}

int main()
{
    init_glfw();

    Window window = create_window();
    if (!window)
        return EXIT_FAILURE;

    setup_imgui(window);

    while (!glfwWindowShouldClose(window))
    {
        gl_check_error();

        check_library_reload();

        glfwPollEvents();

        glfwGetCursorPos(window, &shared_data.mouse_x, &shared_data.mouse_y);

        if (engine_state.update)
            engine_state.update(0.016f);
        if (engine_state.render)
            engine_state.render();

        imgui_begin();

        draw_menu_bar();
        if (engine_state.imgui_draw)
            engine_state.imgui_draw();

        imgui_end();

        glfwSwapBuffers(window);
    }

    close_library();

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
