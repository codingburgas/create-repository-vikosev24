#pragma once

#include "presentation.h"

#include <functional>
#include <string>

// Opaque window handle
struct ApplicationWindow;

// Window initialization configuration
struct WindowConfig {
    int widthPixels = 1000;
    int heightPixels = 700;
    const char* title = "Contact Management System";
};

// Create and initialize the application window with GLFW and ImGui
ApplicationWindow* createApplicationWindow(const WindowConfig& config);

// Run the main event loop
// The callback is called once per frame to render content
void runMainLoop(ApplicationWindow* window,
                 std::function<void()> onFrameCallback);

// Cleanup and destroy the window
void destroyApplicationWindow(ApplicationWindow* window);

// Check if window should close
bool shouldWindowClose(ApplicationWindow* window);
