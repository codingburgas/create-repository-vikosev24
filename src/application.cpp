#include "application.h"
#include "glfw.h"
#include "presentation.h"
#include "logic.h"

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

} // namespace

int runApplication(int argumentCount, char** argumentValues) {
    // Determine data file path
    std::string contactsDataFilePath = resolveDefaultDataFilePath();
    if (argumentCount >= 2 && argumentValues[1] != nullptr) {
        contactsDataFilePath = argumentValues[1];
    }

    // Create application window
    WindowConfig windowConfig;
    windowConfig.widthPixels = 1000;
    windowConfig.heightPixels = 700;
    windowConfig.title = "Contact Management System";

    ApplicationWindow* window = createApplicationWindow(windowConfig);
    if (window == nullptr) {
        return EXIT_FAILURE;
    }

    // Initialize application state
    GuiApplicationState applicationState;
    initializeApplicationState(applicationState, contactsDataFilePath);

    // Run main loop
    runMainLoop(window, [&applicationState]() {
        renderMainApplicationWindow(applicationState);
    });

    // Persist data before shutdown
    persistAllContacts(applicationState.dataFilePath, applicationState.contactList);

    // Cleanup
    destroyApplicationWindow(window);

    return EXIT_SUCCESS;
}
