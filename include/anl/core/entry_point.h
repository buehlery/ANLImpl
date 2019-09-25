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

#ifndef ANL_CORE_ENTRY_POINT_H_
#define ANL_CORE_ENTRY_POINT_H_


// Macro resolving magic.
#define ANL__MACRO_OVERLOAD(_0, _1, NAME, ...) NAME

// Macro that allows us to collect the main entry point.
#define ANLIMPL_MAIN(...) \
  ANL__MACRO_OVERLOAD(__VA_ARGS__, ANLIMPL_MAIN2, ANLIMPL_MAIN1)(__VA_ARGS__)
#define ANLIMPL_MAIN2(X, Y) int ANLIMPL_main(X, Y);\
                            Core::EntryPointLoader anl__epl(&ANLIMPL_main);\
                            int ANLIMPL_main(X, Y)
#define ANLIMPL_MAIN1(...)  ANLIMPL_MAIN2(int argc, char** argv)


// This file contains the entry point management from the CORE module.
namespace Core {


// Helper typedef for entry point pointer.
using EntryPointFunction = int(*)(int, char**);


// Helper class for loading the entry point.
class EntryPointLoader {
 public:
  explicit EntryPointLoader(EntryPointFunction);
};


// Declaration of the entry point pointer.
extern EntryPointFunction gEntryPointPtr;


}  // namespace Core

#endif  // ANL_CORE_ENTRY_POINT_H_
