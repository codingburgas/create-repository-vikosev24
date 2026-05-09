#include "logic.h"

#include <algorithm>
#include <cctype>
#include <string>

namespace {

// lowercase ascii copy, used for case-insensitive compares and search
std::string toLowerCopy(const std::string& original) {
    std::string lowered(original.size(), '\0');
    for (std::size_t characterIndex = 0; characterIndex < original.size(); ++characterIndex) {
        lowered[characterIndex] = static_cast<char>(
            std::tolower(static_cast<unsigned char>(original[characterIndex])));
    }
    return lowered;
}
// order contacts by (lastName, firstName), case-insensitive
bool isContactLessByName(const Contact& leftContact, const Contact& rightContact) {
    const std::string leftLast  = toLowerCopy(leftContact.lastName);
    const std::string rightLast = toLowerCopy(rightContact.lastName);
    if (leftLast != rightLast) {
        return leftLast < rightLast;
    }
    return toLowerCopy(leftContact.firstName) < toLowerCopy(rightContact.firstName);
}
// order contacts by raw phone string
bool isContactLessByPhoneNumber(const Contact& leftContact,
                                const Contact& rightContact) {
    return leftContact.phoneNumber < rightContact.phoneNumber;
}

using ContactComparator = bool (*)(const Contact&, const Contact&);
// Lomuto partition; pivot = element at high
int partitionContactRange(std::vector<Contact>& contactList,
                          int lowIndex,
                          int highIndex,
                          ContactComparator isLess) {
    const Contact pivotContact = contactList[highIndex];
    int storeIndex = lowIndex - 1;
    for (int scanIndex = lowIndex; scanIndex < highIndex; ++scanIndex) {
        if (isLess(contactList[scanIndex], pivotContact)) {
            ++storeIndex;
            std::swap(contactList[storeIndex], contactList[scanIndex]);
        }
    }
    std::swap(contactList[storeIndex + 1], contactList[highIndex]);
    return storeIndex + 1;
}
// classic recursive quick sort over [low, high]
void quickSortContactRange(std::vector<Contact>& contactList,
                           int lowIndex,
                           int highIndex,
                           ContactComparator isLess) {
    if (lowIndex < highIndex) {
        const int pivotIndex = partitionContactRange(contactList, lowIndex, highIndex, isLess);
        quickSortContactRange(contactList, lowIndex, pivotIndex - 1, isLess);
        quickSortContactRange(contactList, pivotIndex + 1, highIndex, isLess);
    }
}
} // namespace

// thin wrapper so presentation never sees the data layer
bool loadAllContacts(const std::string& filePath,
                     std::vector<Contact>& targetContactList) {
    targetContactList ContactList = loadContactsFromJsonFile(filePath);
    return true;
}
// thin wrapper, mirrors loadAllContacts
bool persistAllContacts(const std::string& filePath,
                        const std::vector<Contact>& contactList) {
    return saveContactsToJsonFile(filePath, contactList);
}
// validate, then push with a fresh id
bool addNewContact(std::vector<Contact>& contactList,
                   const Contact& newContact,
                   std::string& errorMessage) {
    errorMessage = validateContact(newContact);
    if (!errorMessage.empty()) {
        return false;
    }
    Contact contactWithAssignedId = newContact;
    contactWithAssignedId.contactId = generateNextContactId(contactList);
    contactList.push_back(contactWithAssignedId);
    return true;
}
// find by id and overwrite the editable fields
bool updateExistingContact(std::vector<Contact>& contactList,
                           const Contact& updatedContact,
                           std::string& errorMessage) {
    errorMessage = validateContact(updatedContact);
    if (!errorMessage.empty()) {
        return false;
    }
    for (auto& existingContact : contactList) {
        if (existingContact.contactId == updatedContact.contactId) {
            existingContact.firstName    = updatedContact.firstName;
            existingContact.lastName     = updatedContact.lastName;
            existingContact.phoneNumber  = updatedContact.phoneNumber;
            existingContact.emailAddress = updatedContact.emailAddress;
            return true;
        }
    }
    errorMessage = "contact id was not found";
    return false;
}
// drop the matching id; false if nothing matched
bool removeContactById(std::vector<Contact>& contactList, int targetContactId) {
    const auto removalIterator = std::remove_if(
        contactList.begin(), contactList.end(),
        [targetContactId](const Contact& candidateContact) {
            return candidateContact.contactId == targetContactId;
        });
    if (removalIterator == contactList.end()) {
        return false;
    }
    contactList.erase(removalIterator, contactList.end());
    return true;
}
// quick-sort entry point, picks the right comparator
void sortContactList(std::vector<Contact>& contactList, SortCriterion criterion) {
    if (contactList.size() < 2) {
        return;
    }
    const ContactComparator isLess = (criterion == SortCriterion::ByName)
        ? &isContactLessByName
        : &isContactLessByPhoneNumber;
    quickSortContactRange(contactList, 0, static_cast<int>(contactList.size()) - 1, isLess);
}
// linear search: substring match against first or last name
std::vector<Contact> findContactsByNameLinear(
        const std::vector<Contact>& contactList,
        const std::string& nameQuery) {
    const std::string loweredQuery = toLowerCopy(nameQuery);
    std::vector<Contact> matchedContacts;
    for (const auto& candidateContact : contactList) {
        if (loweredQuery.empty() ||
            toLowerCopy(candidateContact.firstName).find(loweredQuery) != std::string::npos ||
            toLowerCopy(candidateContact.lastName).find(loweredQuery) != std::string::npos) {
            matchedContacts.push_back(candidateContact);
        }
    }
    return matchedContacts;
} 
// binary search; list MUST be sorted by contactId ascending
int findContactIndexByIdBinary(const std::vector<Contact>& sortedByIdList,
                               int targetContactId) {
    int lowIndex  = 0;
    int highIndex = static_cast<int>(sortedByIdList.size()) - 1;
    while (lowIndex <= highIndex) {
        const int middleIndex = lowIndex + (highIndex - lowIndex) / 2;
        const int middleContactId = sortedByIdList[middleIndex].contactId;
        if (middleContactId == targetContactId) {
            return middleIndex;
        }
        if (middleContactId < targetContactId) {
            lowIndex = middleIndex + 1;
        } else {
            highIndex = middleIndex - 1;
        }
    }
    return -1;
}
// recursive count, used to demo recursion in the GUI
int countContactsRecursive(const std::vector<Contact>& contactList,
                           std::size_t currentIndex) {
    if (currentIndex >= contactList.size()) {
        return 0;
    }
    return 1 + countContactsRecursive(contactList, currentIndex + 1);
}
// recursive count restricted to a phone-number prefix
int countContactsWithPhonePrefixRecursive(
        const std::vector<Contact>& contactList,
        const std::string& phonePrefix,
        std::size_t currentIndex) {
    if (currentIndex >= contactList.size()) {
        return 0;
    }
    const bool prefixMatches =
        contactList[currentIndex].phoneNumber.rfind(phonePrefix, 0) == 0;
    return (prefixMatches ? 1 : 0) +
           countContactsWithPhonePrefixRecursive(contactList, phonePrefix, currentIndex + 1);
}
// minimal sanity rules for a contact record
std::string validateContact(const Contact& contact) {
    if (contact.firstName.empty() && contact.lastName.empty()) {
        return "contact must have a first or last name";
    }
    if (contact.phoneNumber.empty()) {
        return "phone number is required";
    }
    const std::string allowedPhoneCharacters = "0123456789+-() ";
    if (contact.phoneNumber.find_first_not_of(allowedPhoneCharacters) != std::string::npos) {
        return "phone number contains invalid characters";
    }
    return "";
}

