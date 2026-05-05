#include "data.h"

#include "json.hpp"

#include <fstream>
#include <iostream>

using nlohmann::json;

// read the json file into a vector; missing file -> empty list, no fuss
std::vector<Contact> loadContactsFromJsonFile(const std::string& filePath) {
    std::ifstream inputFileStream(filePath);
    if (!inputFileStream.is_open()) {
        return {};
    }

    json parsedDocument;
    try {
        inputFileStream >> parsedDocument;
    } catch (const json::parse_error& parseError) {
        std::cerr << "parse error in " << filePath << ": " << parseError.what() << '\n';
        return {};
    }

    if (!parsedDocument.is_array()) {
        std::cerr << "expected a top-level JSON array\n";
        return {};
    }

    std::vector<Contact> loadedContacts;
    loadedContacts.reserve(parsedDocument.size());
    for (const auto& jsonEntry : parsedDocument) {
        Contact contactRecord;
        contactRecord.contactId    = jsonEntry.value("contactId", 0);
        contactRecord.firstName    = jsonEntry.value("firstName", std::string{});
        contactRecord.lastName     = jsonEntry.value("lastName", std::string{});
        contactRecord.phoneNumber  = jsonEntry.value("phoneNumber", std::string{});
        contactRecord.emailAddress = jsonEntry.value("emailAddress", std::string{});
        loadedContacts.push_back(contactRecord);
    }
    return loadedContacts;
}