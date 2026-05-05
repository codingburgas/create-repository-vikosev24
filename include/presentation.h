#pragma once



#include <string>
#include <vector>

struct GuiApplicationState {
    static constexpr std::size_t FIRST_NAME_BUFFER_SIZE    = 64;
    static constexpr std::size_t LAST_NAME_BUFFER_SIZE     = 64;
    static constexpr std::size_t PHONE_NUMBER_BUFFER_SIZE  = 32;
    static constexpr std::size_t EMAIL_ADDRESS_BUFFER_SIZE = 128;
    static constexpr std::size_t SEARCH_QUERY_BUFFER_SIZE  = 64;
    static constexpr std::size_t PHONE_PREFIX_BUFFER_SIZE  = 16;
    static constexpr int         NO_CONTACT_BEING_EDITED   = -1;

    std::vector<Contact> contactList;
    std::string dataFilePath;

    char firstNameInputBuffer[FIRST_NAME_BUFFER_SIZE]       = {};
    char lastNameInputBuffer[LAST_NAME_BUFFER_SIZE]         = {};
    char phoneNumberInputBuffer[PHONE_NUMBER_BUFFER_SIZE]   = {};
    char emailAddressInputBuffer[EMAIL_ADDRESS_BUFFER_SIZE] = {};

    int editingContactId = NO_CONTACT_BEING_EDITED;

    char searchQueryBuffer[SEARCH_QUERY_BUFFER_SIZE] = {};
    char phonePrefixBuffer[PHONE_PREFIX_BUFFER_SIZE] = {};

    std::string statusMessage;
    bool statusMessageIsError = false;
};

void initializeApplicationState(GuiApplicationState& applicationState,
                                const std::string& dataFilePath);

void renderMainApplicationWindow(GuiApplicationState& applicationState);
