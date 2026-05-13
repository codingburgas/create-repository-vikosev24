#include "glfw.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "GLFW/glfw3.h"

#include <cstdio>
#include <cstdlib>

namespace {

// GLFW error callback
void onGlfwErrorCallback(int errorCode, const char *errorDescription) {
  std::fprintf(stderr, "GLFW error %d: %s\n", errorCode,
               errorDescription ? errorDescription : "(no description)");
}

} // namespace

// Opaque window implementation
struct ApplicationWindow {
  GLFWwindow *glfwWindow = nullptr;
};

ApplicationWindow *createApplicationWindow(const WindowConfig &config) {
  constexpr auto OPENGL_SHADING_LANGUAGE_VERSION_STRING = "#version 130";

  glfwSetErrorCallback(onGlfwErrorCallback);
  if (glfwInit() == GLFW_FALSE) {
    std::fprintf(stderr, "Error: Failed to initialize GLFW.\n");
    return nullptr;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  GLFWwindow *glfwWindow = glfwCreateWindow(
      config.widthPixels, config.heightPixels, config.title, nullptr, nullptr);
  if (glfwWindow == nullptr) {
    std::fprintf(stderr, "Error: Failed to create the application window.\n");
    glfwTerminate();
    return nullptr;
  }

  glfwMakeContextCurrent(glfwWindow);
  glfwSwapInterval(1);

  // Initialize ImGui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  if (!ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true)) {
    std::fprintf(stderr,
                 "Error: Failed to initialize the GLFW ImGui backend.\n");
    ImGui::DestroyContext();
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
    return nullptr;
  }

  if (!ImGui_ImplOpenGL3_Init(OPENGL_SHADING_LANGUAGE_VERSION_STRING)) {
    std::fprintf(stderr,
                 "Error: Failed to initialize the OpenGL3 ImGui backend.\n");
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
    return nullptr;
  }

  ApplicationWindow *window = new ApplicationWindow();
  window->glfwWindow = glfwWindow;
  return window;
}

void runMainLoop(ApplicationWindow *window,
                 std::function<void()> onFrameCallback) {
  if (window == nullptr || window->glfwWindow == nullptr) {
    return;
  }

  while (glfwWindowShouldClose(window->glfwWindow) == GLFW_FALSE) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Call the frame callback to render content
    onFrameCallback();

    ImGui::Render();

    int framebufferWidthPixels = 0;
    int framebufferHeightPixels = 0;
    glfwGetFramebufferSize(window->glfwWindow, &framebufferWidthPixels,
                           &framebufferHeightPixels);

    glViewport(0, 0, framebufferWidthPixels, framebufferHeightPixels);
    glClearColor(0.10f, 0.11f, 0.13f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window->glfwWindow);
  }
}

void destroyApplicationWindow(ApplicationWindow *window) {
  if (window == nullptr) {
    return;
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  if (window->glfwWindow != nullptr) {
    glfwDestroyWindow(window->glfwWindow);
  }

  glfwTerminate();

  delete window;
}

bool shouldWindowClose(ApplicationWindow *window) {
  if (window == nullptr || window->glfwWindow == nullptr) {
    return true;
  }
  return glfwWindowShouldClose(window->glfwWindow) == GLFW_TRUE;
}
