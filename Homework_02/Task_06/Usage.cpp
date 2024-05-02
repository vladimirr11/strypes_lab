// Own includes
#include "Usage.h"

const std::string usage = R"(
    Usage: wc [OPTION]... [FILE]
    
    Print newline, word, and byte counts for FILE if no options are provided.
    With no FILE, or when FILE is -, wc will read from standard input.

    The options below may be used to select which counts are printed, always in
    the following order: newline, word, character, byte.
    -c, --bytes : print the byte counts
    -m, --chars : print the character counts
    -l, --lines : print the newline counts
    -w, --words : print the word counts
    -h, --help  : display this help message and exit
)";
