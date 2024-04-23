/*
 * ParseTime.cpp
 *
 * Project: PKS Edit for Windows
 *
 * purpose: parse date and time from a string given a format. C++ replacement
 * for missing strptime on win32.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "pch.h"
#include <time.h>
#include <iomanip>
#include <sstream>

/*
 * C replacement of strptime. Parse the string s given a format f into a time struct. If successful
 * return end of the parsed string NULL otherwise.
 */
extern "C" __declspec(dllexport) char* strptime(const char* s,
    const char* f,
    struct tm* tm) {
    memset(tm, 0, sizeof * tm);
    std::istringstream input(s);
    input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
    input >> std::get_time(tm, f);
    if (input.fail()) {
        return nullptr;
    }
    if (tm->tm_mday == 0) {
        // Time only - use default standard date
        tm->tm_year = 2020 - 1900;  // 2020
        tm->tm_mon = 2 - 1;         // February
        tm->tm_mday = 15;           // 15th    
    }
    return (char*)(s + input.tellg());
}