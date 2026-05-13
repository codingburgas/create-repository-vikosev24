#include "presentation.h"

#include "imgui.h"

#include <cstring>
#include <string>

namespace {

// safe strncpy that always nul-terminates
void copyStringIntoFixedBuffer(char *destinationBuffer,
                               std::size_t destinationBufferSize,
                               const std::string &sourceString) {
  if (destinationBufferSize == 0) {
    return;
  }
  const std::size_t copyableCharacterCount =
      std::min(sourceString.size(), destinationBufferSize - 1);
  std::memcpy(destinationBuffer, sourceString.data(), copyableCharacterCount);
  destinationBuffer[copyableCharacterCount] = '\0';
}

// reset the form back to "add" mode
void clearAddEditFormBuffers(GuiApplicationState &applicationState) {
  applicationState.firstNameInputBuffer[0] = '\0';
  applicationState.lastNameInputBuffer[0] = '\0';
  applicationState.phoneNumberInputBuffer[0] = '\0';
  applicationState.emailAddressInputBuffer[0] = '\0';
  applicationState.editingContactId =
      GuiApplicationState::NO_CONTACT_BEING_EDITED;
}

// snapshot the input buffers into a Contact value
Contact
buildContactFromInputBuffers(const GuiApplicationState &applicationState) {
  Contact assembledContact;
  assembledContact.contactId = (applicationState.editingContactId !=
                                GuiApplicationState::NO_CONTACT_BEING_EDITED)
                                   ? applicationState.editingContactId
                                   : 0;
  assembledContact.firstName = applicationState.firstNameInputBuffer;
  assembledContact.lastName = applicationState.lastNameInputBuffer;
  assembledContact.phoneNumber = applicationState.phoneNumberInputBuffer;
  assembledContact.emailAddress = applicationState.emailAddressInputBuffer;
  return assembledContact;
}

// post a status message; isError flips the colour
void postStatusMessage(GuiApplicationState &applicationState,
                       const std::string &messageText, bool isError) {
  applicationState.statusMessage = messageText;
  applicationState.statusMessageIsError = isError;
}

// draw the colored status line if anything is queued
void renderStatusBar(const GuiApplicationState &applicationState) {
  if (applicationState.statusMessage.empty()) {
    return;
  }
  const ImVec4 messageColor = applicationState.statusMessageIsError
                                  ? ImVec4(0.95f, 0.35f, 0.35f, 1.00f)
                                  : ImVec4(0.30f, 0.85f, 0.30f, 1.00f);
  ImGui::TextColored(messageColor, "%s",
                     applicationState.statusMessage.c_str());
}

// draw the add/edit form; flag the caller if the user clicked Save
void renderAddEditForm(GuiApplicationState &applicationState,
                       bool &outShouldSave, Contact &outContactToSave) {
  outShouldSave = false;

  const bool isEditingExistingContact =
      (applicationState.editingContactId !=
       GuiApplicationState::NO_CONTACT_BEING_EDITED);
  ImGui::SeparatorText(isEditingExistingContact ? "Edit Contact"
                                                : "Add Contact");

  ImGui::InputText("First Name", applicationState.firstNameInputBuffer,
                   GuiApplicationState::FIRST_NAME_BUFFER_SIZE);
  ImGui::InputText("Last Name", applicationState.lastNameInputBuffer,
                   GuiApplicationState::LAST_NAME_BUFFER_SIZE);
  ImGui::InputText("Phone Number", applicationState.phoneNumberInputBuffer,
                   GuiApplicationState::PHONE_NUMBER_BUFFER_SIZE);
  ImGui::InputText("Email Address", applicationState.emailAddressInputBuffer,
                   GuiApplicationState::EMAIL_ADDRESS_BUFFER_SIZE);

  if (ImGui::Button(isEditingExistingContact ? "Save Changes"
                                             : "Add Contact")) {
    outContactToSave = buildContactFromInputBuffers(applicationState);
    outShouldSave = true;
  }
  ImGui::SameLine();
  if (ImGui::Button("Clear Form")) {
    clearAddEditFormBuffers(applicationState);
  }
}

// draw the top toolbar: sort / save / reload / search
void renderToolbar(GuiApplicationState &applicationState) {
  if (ImGui::Button("Sort by Name")) {
    sortContactList(applicationState.contactList, SortCriterion::ByName);
    postStatusMessage(applicationState, "sorted by name", false);
  }
  ImGui::SameLine();
  if (ImGui::Button("Sort by Phone")) {
    sortContactList(applicationState.contactList, SortCriterion::ByPhoneNumber);
    postStatusMessage(applicationState, "sorted by phone", false);
  }
  ImGui::SameLine();
  if (ImGui::Button("Save to JSON")) {
    const bool saveSucceeded = persistAllContacts(applicationState.dataFilePath,
                                                  applicationState.contactList);
    postStatusMessage(applicationState,
                      saveSucceeded
                          ? "saved to " + applicationState.dataFilePath
                          : "failed to save",
                      !saveSucceeded);
  }
  ImGui::SameLine();
  if (ImGui::Button("Reload from JSON")) {
    loadAllContacts(applicationState.dataFilePath,
                    applicationState.contactList);
    postStatusMessage(applicationState, "reloaded from disk", false);
  }
  ImGui::InputTextWithHint("##SearchQuery", "Search by first or last name",
                           applicationState.searchQueryBuffer,
                           GuiApplicationState::SEARCH_QUERY_BUFFER_SIZE);
}

// draw the contacts table; honours the live search filter
void renderContactTable(GuiApplicationState &applicationState) {
  const std::string searchQueryString = applicationState.searchQueryBuffer;
  const std::vector<Contact> visibleContacts =
      searchQueryString.empty()
          ? applicationState.contactList
          : findContactsByNameLinear(applicationState.contactList,
                                     searchQueryString);

  ImGui::Text("Showing %zu of %zu contact(s).", visibleContacts.size(),
              applicationState.contactList.size());

  const ImGuiTableFlags contactTableFlags =
      ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
      ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY;
  const ImVec2 contactTableSize(0.0f, 260.0f);

  if (!ImGui::BeginTable("ContactsTable", 6, contactTableFlags,
                         contactTableSize)) {
    return;
  }

  ImGui::TableSetupScrollFreeze(0, 1);
  ImGui::TableSetupColumn("Id");
  ImGui::TableSetupColumn("First Name");
  ImGui::TableSetupColumn("Last Name");
  ImGui::TableSetupColumn("Phone Number");
  ImGui::TableSetupColumn("Email Address");
  ImGui::TableSetupColumn("Actions");
  ImGui::TableHeadersRow();

  int pendingDeleteContactId = GuiApplicationState::NO_CONTACT_BEING_EDITED;

  for (const auto &visibleContact : visibleContacts) {
    ImGui::PushID(visibleContact.contactId);
    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    ImGui::Text("%d", visibleContact.contactId);
    ImGui::TableSetColumnIndex(1);
    ImGui::TextUnformatted(visibleContact.firstName.c_str());
    ImGui::TableSetColumnIndex(2);
    ImGui::TextUnformatted(visibleContact.lastName.c_str());
    ImGui::TableSetColumnIndex(3);
    ImGui::TextUnformatted(visibleContact.phoneNumber.c_str());
    ImGui::TableSetColumnIndex(4);
    ImGui::TextUnformatted(visibleContact.emailAddress.c_str());

    ImGui::TableSetColumnIndex(5);
    if (ImGui::SmallButton("Edit")) {
      applicationState.editingContactId = visibleContact.contactId;
      copyStringIntoFixedBuffer(applicationState.firstNameInputBuffer,
                                GuiApplicationState::FIRST_NAME_BUFFER_SIZE,
                                visibleContact.firstName);
      copyStringIntoFixedBuffer(applicationState.lastNameInputBuffer,
                                GuiApplicationState::LAST_NAME_BUFFER_SIZE,
                                visibleContact.lastName);
      copyStringIntoFixedBuffer(applicationState.phoneNumberInputBuffer,
                                GuiApplicationState::PHONE_NUMBER_BUFFER_SIZE,
                                visibleContact.phoneNumber);
      copyStringIntoFixedBuffer(applicationState.emailAddressInputBuffer,
                                GuiApplicationState::EMAIL_ADDRESS_BUFFER_SIZE,
                                visibleContact.emailAddress);
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Delete")) {
      pendingDeleteContactId = visibleContact.contactId;
    }
    ImGui::PopID();
  }

  ImGui::EndTable();

  // delete after the loop so we don't mutate while iterating the filtered copy
  if (pendingDeleteContactId != GuiApplicationState::NO_CONTACT_BEING_EDITED) {
    const bool removalSucceeded =
        removeContactById(applicationState.contactList, pendingDeleteContactId);
    if (removalSucceeded) {
      postStatusMessage(applicationState, "contact deleted", false);
      if (applicationState.editingContactId == pendingDeleteContactId) {
        clearAddEditFormBuffers(applicationState);
      }
    } else {
      postStatusMessage(applicationState, "failed to delete contact", true);
    }
  }
}
