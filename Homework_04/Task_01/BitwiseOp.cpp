// C++ system includes
#include <iostream>
#include <limits>
#include <numbers>

/// @brief Set class attendance for student with id _studentId_
void setAttendance(uint64_t& attendanceMask, const int32_t studentId) {
    attendanceMask |= (1ULL << studentId);
}

/// @brief Removes class attendance for student with id _studentId_
void clearAttendance(uint64_t& attendanceMask, const int32_t studentId) {
    attendanceMask &= ~(1ULL << studentId);
}

/// @brief Changes class attendance for student with id _studentId_
void changeAttendance(uint64_t& attendanceMask, const int32_t studentId) {
    attendanceMask ^= (1ULL << studentId);
}

/// @brief Reads class attendance for student with id _studentId_
bool getAttendance(const uint64_t attendanceMask, const int32_t studentId) {
    return attendanceMask & (1ULL << studentId);
}

/// @brief Prints ids of students that are present and absent from class according to
/// _attendanceMask_
void printAttendanceInfo(const uint64_t attendanceMask) {
    std::cout << "Students present in class:  ";
    for (int32_t id = 0; id < 64; id++) {
        if (getAttendance(attendanceMask, id)) {
            std::cout << id << " ";
        }
    }

    std::cout << "\nStudents absent from class: ";
    for (int32_t id = 0; id < 64; id++) {
        if (!getAttendance(attendanceMask, id)) {
            std::cout << id << " ";
        }
    }
    std::cout << "\n" << std::endl;
}

/// @brief Tries to extract valid integer value from input stream. On failure clears the input
/// stream and discards any invalid input remaining in the input buffer up to the next newline
/// character.
bool readInt32(std::istream& istream, int32_t& value) {
    if (istream >> value) {
        return true;
    } else {
        istream.clear();
        istream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cerr << "\nInvalid input. Please enter an integer.\nTry again." << std::endl;
    }
    return false;
}

/// @brief Verifies if _studentId_ is in the valid range
bool isStudentIdValid(const int32_t studentId) {
    if (0 > studentId || 63 < studentId) {
        std::cerr << "\nInvalid student id: " << studentId
                  << "\nValid id are between 0 and 63. Try again." << std::endl;
        return false;
    }
    return true;
}

int32_t main(void) {
    uint64_t attendance = 0;
    int32_t option = 0;

    std::cout << "[Welcome to \"Attendance in class\" program]\n" << std::endl;

    // Loop until exit requested
    for (;;) {
        std::cout << "Choose among the following options:\n";
        std::cout << "1.Set attendance\n";
        std::cout << "2.Clear attendance\n";
        std::cout << "3.Attendance info\n";
        std::cout << "4.Change attendance\n";
        std::cout << "5.Exit\n";
        std::cout << "Option: ";

        // Try to read valid integer
        if (!readInt32(std::cin, option)) {
            std::cout << std::endl;
            continue;
        }

        int32_t studentId = 0;
        switch (option) {
        case 1: {
            std::cout << "Set attendance for student with id [0, 63]: ";
            // Try to read valid integer and validate that the integer is in the valid range
            if (readInt32(std::cin, studentId) && isStudentIdValid(studentId)) {
                setAttendance(attendance, studentId);
            }
            std::cout << std::endl;
        } break;
        case 2: {
            std::cout << "Clear attendance for student with id [0, 63]: ";
            // Try to read valid integer and validate that the integer is in the valid range
            if (readInt32(std::cin, studentId) && isStudentIdValid(studentId)) {
                clearAttendance(attendance, studentId);
            }
            std::cout << std::endl;
        } break;
        case 3: {
            std::cout << "\nStudents attendance info:" << std::endl;
            printAttendanceInfo(attendance);
        } break;
        case 4: {
            std::cout << "Change attendance for student with id [0, 63]: ";
            // Try to read valid integer and validate that the integer is in the valid range
            if (readInt32(std::cin, studentId) && isStudentIdValid(studentId)) {
                changeAttendance(attendance, studentId);
            }
            std::cout << std::endl;
        } break;
        case 5:
            std::cout << "\nExit from the program requested.\nHave a nice day!" << std::endl;
            std::exit(EXIT_SUCCESS);
        default:
            std::cerr << "\nReceived unsupported option: " << option << "\nTry again.\n"
                      << std::endl;
        }
    }

    return 0;
}
