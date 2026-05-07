#include "presentation.h"

#include "imgui.h"

#include <cstring>
#include <string>

namespace {

// safe strncpy that always nul-terminates
void copyStringIntoFixedBuffer(char* destinationBuffer,
                               std::size_t destinationBufferSize,
                               const std::string& sourceString) {
    if (destinationBufferSize == 0) {
        return;
    }
    const std::size_t copyableCharacterCount =
        std::min(sourceString.size(), destinationBufferSize - 1);
    std::memcpy(destinationBuffer, sourceString.data(), copyableCharacterCount);
    destinationBuffer[copyableCharacterCount] = '\0';
}

// reset the form back to "add" mode
void clearAddEditFormBuffers(GuiApplicationState& applicationState) {
    applicationState.firstNameInputBuffer[0]    = '\0';
    applicationState.lastNameInputBuffer[0]     = '\0';
    applicationState.phoneNumberInputBuffer[0]  = '\0';
    applicationState.emailAddressInputBuffer[0] = '\0';
    applicationState.editingContactId = GuiApplicationState::NO_CONTACT_BEING_EDITED;
}

// snapshot the input buffers into a Contact value
Contact buildContactFromInputBuffers(const GuiApplicationState& applicationState) {
    Contact assembledContact;
    assembledContact.contactId =
        (applicationState.editingContactId != GuiApplicationState::NO_CONTACT_BEING_EDITED)
            ? applicationState.editingContactId
            : 0;
    assembledContact.firstName    = applicationState.firstNameInputBuffer;
    assembledContact.lastName     = applicationState.lastNameInputBuffer;
    assembledContact.phoneNumber  = applicationState.phoneNumberInputBuffer;
    assembledContact.emailAddress = applicationState.emailAddressInputBuffer;
    return assembledContact;
}}

