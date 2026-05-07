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

bool addNewContact(std::vector<Contact>& contactList,
                   const Contact& newContact,
                   std::string& errorMessage);

bool updateExistingContact(std::vector<Contact>& contactList,
                           const Contact& updatedContact,
                           std::string& errorMessage);
                           
bool removeContactById(std::vector<Contact>& contactList, int targetContactId);

void sortContactList(std::vector<Contact>& contactList, SortCriterion criterion);

std::vector<Contact> findContactsByNameLinear(
    const std::vector<Contact>& contactList,
    const std::string& nameQuery);
    int findContactIndexByIdBinary(const std::vector<Contact>& sortedByIdList,
                               int targetContactId);

int countContactsRecursive(const std::vector<Contact>& contactList,
                           std::size_t currentIndex = 0);
int countContactsWithPhonePrefixRecursive(
    const std::vector<Contact>& contactList,
    const std::string& phonePrefix,
    std::size_t currentIndex = 0);

std::string validateContact(const Contact& contact);