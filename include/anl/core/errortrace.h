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

#ifndef ANL_CORE_ERRORTRACE_H_
#define ANL_CORE_ERRORTRACE_H_

#include <string>
#include <vector>

// This file contains the error tracing helper from the CORE module.
namespace Core {


// The helper class that allows errors in a scope to be printed.
class ErrorTracer {
 public:
  // Enters a section with the given name.
  void enter(const std::string& name);

  // Leaves the most recent section.
  void leave();

  // Asserts that the given boolean is true. If the assertion fails, an error
  //  trace is printed and the program is terminated.
  void require(bool expr, const char* message);

 private:
  // The vector that stores the current section stack.
  std::vector<std::string> mSections;
};


}  // namespace Core

#endif  // ANL_CORE_ERRORTRACE_H_
