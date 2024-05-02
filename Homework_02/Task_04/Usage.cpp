// Own includes
#include "Usage.h"

const std::string usage = R"(
    Usage: tail [OPTION]... [FILE]
    
    Print the last 10 lines from FILE to standard output if no options are provided.
    With no FILE, or when FILE is -, tail will read from standard input.

    Available options:
    -c, --bytes NUM : output the last NUM bytes from the given file or standard input on separate line
    -n, --lines NUM : output the last NUM lines from the given file or standard input on separate line
    -v, --verbose   : always output headers giving file name
    -h, --help      : display this help message and exit
)";
