#pragma once

#include "data.h"

#include <string>
#include <vector>

enum class SortCriterion {
    ByName,
    ByPhoneNumber
};

bool loadAllContacts(const std::string& filePath,
                     std::vector<Contact>& targetContactList);

bool persistAllContacts(const std::string& filePath,
                        const std::vector<Contact>& contactList);
