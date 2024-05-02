// C++ system includes
#include <cassert>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

// Own includes
#include "Usage.h"

enum StorageType : uint8_t { BYTES, LINES, UNKNOWN };

/// @brief Struct that will store the parsed command line arguments
struct StorageSettings {
    int32_t numLines{};
    int32_t numBytes{};
    std::string fileName;
    bool verbose = false;
    StorageType storageType = UNKNOWN;
};

/// @brief The object that will accumulate the first num lines/bytes read from file or
/// standard input
class HeadStorage {
public:
    HeadStorage() = delete;

    HeadStorage(const StorageSettings& settings_) : settings(settings_) {
        if (settings.storageType == BYTES) {
            assert(settings.numBytes > 0 && "Num bytes must be positive number");
            storage.reserve(settings.numBytes);
        } else if (settings.storageType == LINES) {
            // Allocate the necessary buffer for num lines
            assert(settings.numLines > 0 && "Num lines must be positive number");
            storage.reserve(settings.numLines);
        } else {
            // Note: throw exceptions here - otherwise the object will be in invalid state
            throw std::invalid_argument("Unknown storage type provided");
        }
    }

    void update(const std::string& inputLine) {
        switch (settings.storageType) {
        case LINES: {
            // Update only the first num lines
            if (storage.size() < (size_t)settings.numLines) {
                storage.push_back(inputLine);
            }
        } break;
        case BYTES: {
            // Static counter to keep track of the read bytes
            static int32_t countBytes = 0;
            // Early return if the num bytes are already read
            if (countBytes >= settings.numBytes) {
                return;
            }

            // Update the storage
            std::string line = "";
            for (size_t i = 0; i < inputLine.size(); i++) {
                if (countBytes < settings.numBytes) {
                    line += inputLine[i];
                }
                countBytes++;
            }
            storage.push_back(line);
        } break;
        default:
            std::cerr << "Error to update - unknown storage type\n";
        }
    }

    void print() const {
        if (settings.verbose) {
            if (!settings.fileName.empty()) {
                std::cout << "============== " << settings.fileName << " ==============\n";
            } else {
                std::cout << "============== std::cin ==============\n";
            }
        }
        for (auto it = storage.begin(); it != storage.end(); it++) {
            std::cout << *it << "\n";
        }
    }

private:
    std::vector<std::string> storage;  ///< Store the first num lines/bytes
    StorageSettings settings;          ///< The read command line arguments
};

/// @brief Utility function to convert command line argument into integer - if possible
bool extractIntegerFromCMDLineArg(const std::string& arg, int32_t& value) {
    std::istringstream istream(arg);
    return istream >> value && istream.eof();
}

int main(int argc, const char** argv) {
    StorageSettings settings;

    // Default state
    settings.numLines = 10;
    settings.storageType = LINES;

    // Parse the command line arguments - if any
    for (int32_t i = 1; i < argc; i++) {
        // Output the first lines
        if ((strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--lines") == 0) && i++) {
            if (i >= argc) {
                std::cerr
                    << "Missing argument for -n/--lines\nTry 'head --help' for more information.";
                exit(EXIT_FAILURE);
            }

            if (!extractIntegerFromCMDLineArg(argv[i], settings.numLines)) {
                std::cerr << "Failed to extract number of line for argument: " << argv[i] << "\n";
                exit(EXIT_FAILURE);
            }
            settings.storageType = LINES;
        }
        // Output the first bytes
        else if ((strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--bytes") == 0) && i++) {
            if (i >= argc) {
                std::cerr
                    << "Missing argument for -c/--bytes\nTry 'head --help' for more information.";
                exit(EXIT_FAILURE);
            }

            if (!extractIntegerFromCMDLineArg(argv[i], settings.numBytes)) {
                std::cerr << "Failed to extract number of bytes for argument: " << argv[i] << "\n";
                exit(EXIT_FAILURE);
            }
            settings.storageType = BYTES;
        }
        // Verbose mode
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            settings.verbose = true;
        }
        // Display help message
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            std::cout << usage << std::endl;
            exit(EXIT_SUCCESS);
        }
        // Read the file - if any
        else {
            settings.fileName = argv[i];
        }
    }

    // Initialize the head storage class with the read command line arguments
    HeadStorage headStorage(settings);

    // In case no file name provided or file name == "-" read the data from standard input
    if (settings.fileName == "-" || settings.fileName == "") {
        // Read from standard input line by line and update the head storage
        std::string line;
        while (std::getline(std::cin, line)) {
            headStorage.update(line);
        }
    }
    // Read data from file
    else {
        std::ifstream inputFile(settings.fileName, std::ios::in);  // Open the file
        if (!inputFile.good()) {
            std::cerr << "Failed to read file: " << settings.fileName << "\n";
            exit(EXIT_FAILURE);
        }

        // Read the file line by line and update the head storage
        std::string line;
        while (std::getline(inputFile, line)) {
            headStorage.update(line);
        }

        inputFile.close();  // Close the file
    }

    // Print the head storage
    headStorage.print();

    return 0;
}
