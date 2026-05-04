#pragma once

#include <string>
#include <vector>

struct Contact {
  int contactId = 0;
  std::string firstName;
  std::string lastName;
  std::string phoneNumber;
  std::string emailAddress;
};

std::vector<Contact> loadContactsFromJsonFile(const std::string &filePath);

bool saveContactsToJsonFile(const std::string &filePath,
                            const std::vector<Contact> &contactList);

int generateNextContactId(const std::vector<Contact> &contactList);
