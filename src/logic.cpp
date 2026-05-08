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
    target ContactList = loadContactsFromJsonFile(filePath);
    return true;
}
// thin wrapper, mirrors loadAllContacts
bool persistAllContacts(const std::string& filePath,
                        const std::vector<Contact>& contactList) {
    return saveContactsToJsonFile(filePath, contactList);
}