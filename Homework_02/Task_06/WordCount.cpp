// C++ system includes
#include <bitset>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

// Own includes
#include "Usage.h"

enum CounterUnitType : uint8_t { NEW_LINES, WORDS, CHARS, BYTES };

/// @brief This struct will set the WordCounter state
struct CounterSettings {
    std::string fileName;      ///< The file name
    std::bitset<4> counts{};   ///< Track the units that will print - if not in default state
    bool defaultState = true;  ///< If no options provided print the default state
};

/// @brief Count words, chars, bytes, and new lines based on the provided settings
class WordCounter {
public:
    WordCounter() = delete;

    WordCounter(const CounterSettings& settings_) : settings(settings_) {}

    /// @brief Update the word counter units
    void update(const std::string& inputLine) {
        units.numBytes += inputLine.size() + 1;  // Note: +1 for the new line char
        units.numNewLines++;
        std::istringstream istream(inputLine);
        std::string word;
        while (istream >> word) {
            units.numWords++;
            units.numChars += word.size() + 1;  // Note: +1 for the new line char
        }
    }

    void print() const {
        if (settings.defaultState) {
            std::cout << units.numNewLines << " " << units.numWords << " " << units.numBytes << " "
                      << settings.fileName << std::endl;
            return;
        }

        for (size_t i = 0; i < settings.counts.size(); i++) {
            if (settings.counts.test(i)) {
                std::cout << units.countsArr[i] << " ";
            }
        }
        std::cout << settings.fileName << std::endl;
    }

private:
    /// @brief Store the count for the different units 
    union CounterUnits {
        // Here the order matters
        struct {
            uint64_t numNewLines, numWords, numChars, numBytes;
        };

        uint64_t countsArr[4]{}; // Set all to zero
    };

private:
    CounterUnits units;
    CounterSettings settings;
};

int main(int argc, const char** argv) {
    CounterSettings settings;

    for (int32_t i = 1; i < argc; i++) {
        // Count num bytes
        if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--bytes") == 0) {
            settings.counts[BYTES] = true;
            settings.defaultState = false;
        }
        // Count num characters
        else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--chars") == 0) {
            settings.counts[CHARS] = true;
            settings.defaultState = false;
        }
        // Count num new lines
        else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--lines") == 0) {
            settings.counts[NEW_LINES] = true;
            settings.defaultState = false;
        }
        // Count num words
        else if (strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--words") == 0) {
            settings.counts[WORDS] = true;
            settings.defaultState = false;
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

    // Initialize the word counter with the parsed command line arguments
    WordCounter counter(settings);

    // In case no file name provided or file name == "-" read the data from standard input
    if (settings.fileName == "-" || settings.fileName == "") {
        // Read form standard input line by line and update the counter
        std::string line;
        while (std::getline(std::cin, line)) {
            counter.update(line);
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
            counter.update(line);
        }

        inputFile.close();  // Close the file
    }

    // Print the counter state
    counter.print();

    return 0;
}
