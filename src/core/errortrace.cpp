// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <http://unlicense.org/>
//
// Author: Yannick Bühler
//
// Part of ANL-Impl.

#include "anl/core/errortrace.h"
#include <cstdio>
#include <cstdlib>
#include "anl/misc/asserts.h"

// This file contains the error tracing helper from the CORE module.
namespace Core {


// _____________________________________________________________________________
void ErrorTracer::enter(const std::string& name) {
  Misc::Asserts::require(name.size() > 0, "empty section name");
  mSections.push_back(name);
}

// _____________________________________________________________________________
void ErrorTracer::leave() {
  Misc::Asserts::require(mSections.size() > 0, "empty section stack");
  mSections.pop_back();
}

// _____________________________________________________________________________
void ErrorTracer::require(bool expr, const char* message) {
  Misc::Asserts::require(message != nullptr, "invalid message");

  // Print a backtrace and fail, if required.
  if (!expr) {
    std::fprintf(stderr, "An error occurred: %s\n", message);
    if (mSections.size() > 0) {
      std::fprintf(stderr, "This error occurred from:\n");
      for (const std::string& str : mSections) {
        std::fprintf(stderr, "  => %s\n", str.c_str());
      }
    }
    std::fprintf(stderr, "The program will be terminated.\n");
    std::exit(2);
  }
}


}  // namespace Core
