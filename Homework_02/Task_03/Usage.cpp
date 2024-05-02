// Own includes
#include "Usage.h"

const std::string usage = R"(
    Usage: head [OPTION]... [FILE]
    
    Print the first 10 lines from FILE to standard output if no options are provided.
    With no FILE, or when FILE is -, head will read from standard input.

    Available options:
    -c, --bytes NUM : output the first NUM bytes from the given file or standard input
    -n, --lines NUM : output the first NUM lines from the given file or standard input
    -v, --verbose   : always output headers giving file name
    -h, --help      : display this help message and exit
)";
