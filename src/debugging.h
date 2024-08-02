#ifndef DEBUGGING_H
#define DEBUGGING_H

#include <iostream>
#include <string>

/**
 * @brief Outputs the given formatted message to console.
 * Essentially just a wrapper on top of `std::printf()` which resolves to an empty function if `DEBUG_MODE` is not defined.
 * 
 * @param[in] format The format message string to output
 * @param[in] ... The format arguments
 */
static void OutputLog(std::string_view format, ...)
{
#ifdef DEBUG_MODE
    va_list args;
    va_start(args, format);
    std::vprintf(format.data(), args);
    va_end(args);
#endif
}

#endif