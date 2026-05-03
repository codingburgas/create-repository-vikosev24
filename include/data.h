#pragma once

#include <string>

struct Contact {
  int contactId = 0;
  std::string firstName;
  std::string lastName;
  std::string phoneNumber;
  std::string emailAddress;
};
