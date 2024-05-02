// C++ system includes
#include <cassert>
#include <cstring>
#include <deque>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <variant>

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

/// @brief The object that will accumulate the last num lines/bytes read from file or
/// standard input
class TailStorage {
    // Note: for tail storage the usage of std::deque as underlying data structure is more
    // appropriate than std::vector because we can push_back and pop_front in constant time and we
    // dont need to store the entire file in order to get the last num bytes/lines from it
    using Storage = std::variant<std::deque<char>, std::deque<std::string>>;

public:
    TailStorage() = delete;

    TailStorage(const StorageSettings& settings_) : settings(settings_) {
        if (settings.storageType == BYTES) {
            storage = std::deque<char>{};
        } else if (settings.storageType == LINES) {
            storage = std::deque<std::string>{};
        } else {
            // Note: throw exceptions here - otherwise the object will be in invalid state
            throw std::invalid_argument("Unknown storage type provided");
        }
    }

    void update(const std::string& inputLine) {
        switch (settings.storageType) {
        case LINES: {
            assert(settings.numLines > 0 && "Num lines must be positive number");
            auto& lineStorage = std::get<std::deque<std::string>>(storage);
            lineStorage.push_back(inputLine);
            if (lineStorage.size() > (size_t)settings.numLines) {
                lineStorage.pop_front();
            }
        } break;
        case BYTES: {
            assert(settings.numBytes > 0 && "Num bytes must be positive number");
            auto& byteStorage = std::get<std::deque<char>>(storage);
            for (size_t i = 0; i < inputLine.size(); i++) {
                byteStorage.push_back(inputLine[i]);
                if (byteStorage.size() > (size_t)settings.numBytes) {
                    byteStorage.pop_front();
                }
            }
            // Note: here the new line also matters
            constexpr char newLineASCIIEncoding = 10;
            byteStorage.push_back(newLineASCIIEncoding);
            if (byteStorage.size() > (size_t)settings.numBytes) {
                byteStorage.pop_front();
            }
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
        switch (settings.storageType) {
        case LINES: {
            const auto& lineStorage = std::get<std::deque<std::string>>(storage);
            for (auto it = lineStorage.begin(); it != lineStorage.end(); it++) {
                std::cout << *it << std::endl;
            }
        } break;
        case BYTES: {
            const auto& byteStorage = std::get<std::deque<char>>(storage);
            for (auto it = byteStorage.begin(); it != byteStorage.end(); it++) {
                std::cout << *it;
            }
        } break;
        default: {
            std::cerr << "Error to print - unknown storage type\n";
        }
        }
    }

private:
    Storage storage;           ///< Store either the last num lines or the last num bytes
    StorageSettings settings;  ///< The read command line arguments
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
        // Output the last lines
        if ((strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--lines") == 0) && i++) {
            if (i >= argc) {
                std::cerr
                    << "Missing argument for -n/--lines\nTry 'tail --help' for more information.";
                exit(EXIT_FAILURE);
            }

            if (!extractIntegerFromCMDLineArg(argv[i], settings.numLines)) {
                std::cerr << "Failed to extract number of line for argument: " << argv[i] << "\n";
                exit(EXIT_FAILURE);
            }
            settings.storageType = LINES;
        }
        // Output the last bytes
        else if ((strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--bytes") == 0) && i++) {
            if (i >= argc) {
                std::cerr
                    << "Missing argument for -c/--bytes\nTry 'tail --help' for more information.";
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

    // Initialize the tail storage class with the read cmd line arguments
    TailStorage tailStorage(settings);

    // In case no file name provided or file name == "-" read the data from standard input
    if (settings.fileName == "-" || settings.fileName == "") {
        // Read form standard input line by line and update the tail storage
        std::string line;
        while (std::getline(std::cin, line)) {
            tailStorage.update(line);
        }
    }
    // Read data from file
    else {
        std::ifstream inputFile(settings.fileName, std::ios::in);  // Open the file
        if (!inputFile.good()) {
            std::cerr << "Failed to read file: " << settings.fileName << "\n";
            exit(EXIT_FAILURE);
        }

        // Read the file line by line and update the storage
        std::string line;
        while (std::getline(inputFile, line)) {
            tailStorage.update(line);
        }

        inputFile.close();  // Close the file
    }

    // Print the tail storage
    tailStorage.print();

    return 0;
}
