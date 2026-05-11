#include "application.h"
#include "presentation.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "GLFW/glfw3.h"

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>

namespace {

// pick the contacts.json next to the binary, or fall back to ../data
std::string resolveDefaultDataFilePath() {
  namespace fs = std::filesystem;
  const fs::path nearWorkingDir = fs::path("data") / "contacts.json";
  const fs::path nearParentDir = fs::path("..") / "data" / "contacts.json";
  if (fs::exists(nearWorkingDir))
    return nearWorkingDir.string();
  if (fs::exists(nearParentDir))
    return nearParentDir.string();
  return nearWorkingDir.string();
}

// glfw shouts on stderr when something explodes during init
void onGlfwErrorCallback(int errorCode, const char *errorDescription) {
  std::fprintf(stderr, "GLFW error %d: %s\n", errorCode,
               errorDescription ? errorDescription : "(no description)");
}

} // namespace

// the whole program lives here: window + imgui setup, frame loop, cleanup
int runApplication(int argumentCount, char **argumentValues) {
  constexpr int INITIAL_WINDOW_WIDTH_PIXELS = 1000;
  constexpr int INITIAL_WINDOW_HEIGHT_PIXELS = 700;
  constexpr auto APPLICATION_WINDOW_TITLE = "Contact Management System";
  constexpr auto OPENGL_SHADING_LANGUAGE_VERSION_STRING = "#version 130";

  std::string contactsDataFilePath = resolveDefaultDataFilePath();
  if (argumentCount >= 2 && argumentValues[1] != nullptr) {
    contactsDataFilePath = argumentValues[1];
  }

  glfwSetErrorCallback(onGlfwErrorCallback);
  if (glfwInit() == GLFW_FALSE) {
    std::fprintf(stderr, "Error: Failed to initialize GLFW.\n");
    return EXIT_FAILURE;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  GLFWwindow *mainApplicationWindow = glfwCreateWindow(
      INITIAL_WINDOW_WIDTH_PIXELS, INITIAL_WINDOW_HEIGHT_PIXELS,
      APPLICATION_WINDOW_TITLE, nullptr, nullptr);
  if (mainApplicationWindow == nullptr) {
    std::fprintf(stderr, "Error: Failed to create the application window.\n");
    glfwTerminate();
    return EXIT_FAILURE;
  }

  glfwMakeContextCurrent(mainApplicationWindow);
  glfwSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  if (!ImGui_ImplGlfw_InitForOpenGL(mainApplicationWindow, true)) {
    std::fprintf(stderr,
                 "Error: Failed to initialize the GLFW ImGui backend.\n");
    ImGui::DestroyContext();
    glfwDestroyWindow(mainApplicationWindow);
    glfwTerminate();
    return EXIT_FAILURE;
  }
  if (!ImGui_ImplOpenGL3_Init(OPENGL_SHADING_LANGUAGE_VERSION_STRING)) {
    std::fprintf(stderr,
                 "Error: Failed to initialize the OpenGL3 ImGui backend.\n");
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(mainApplicationWindow);
    glfwTerminate();
    return EXIT_FAILURE;
  }

  GuiApplicationState applicationState;
  initializeApplicationState(applicationState, contactsDataFilePath);

  while (glfwWindowShouldClose(mainApplicationWindow) == GLFW_FALSE) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    renderMainApplicationWindow(applicationState);

    ImGui::Render();

    int framebufferWidthPixels = 0;
    int framebufferHeightPixels = 0;
    glfwGetFramebufferSize(mainApplicationWindow, &framebufferWidthPixels,
                           &framebufferHeightPixels);

    glViewport(0, 0, framebufferWidthPixels, framebufferHeightPixels);
    glClearColor(0.10f, 0.11f, 0.13f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(mainApplicationWindow);
  }

  persistAllContacts(applicationState.dataFilePath,
                     applicationState.contactList);

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(mainApplicationWindow);
  glfwTerminate();

  return EXIT_SUCCESS;
}
