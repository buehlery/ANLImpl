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

#include <getopt.h>
// cpplint forbids this header for some reason. Quick research of the problem
//  shows that this seems to be related to chromium, which we do not use.
#include <chrono>  // NOLINT
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include "anl/core/entry_point.h"
#include "anl/core/simulator.h"

using std::chrono::milliseconds;

// This file contains the entry point management from the CORE module.
namespace Core {


// _____________________________________________________________________________
static int dummy(int x, char** y) { return 0; }
EntryPointFunction gEntryPointPtr = &dummy;

// _____________________________________________________________________________
EntryPointLoader::EntryPointLoader(EntryPointFunction cb) {
  gEntryPointPtr = cb;
}


}  // namespace Core


// _____________________________________________________________________________
static void printHeader() {
  std::fprintf(stderr, "[ INFO ] ******************** ANL-Impl ANL simulator "
    "v0.1.0 ********************\n");
  std::fprintf(stderr, "[ INFO ] This is free and unencumbered software "
    "released into the public domain.\n");
  std::fprintf(stderr, "[ INFO ] For the full license text, visit "
    "<https://unlicense.org/>\n");
  std::fprintf(stderr, "[ INFO ] ***************************************"
    "********************************\n");
  std::fprintf(stderr, "[ INFO ]\n");
}


// _____________________________________________________________________________
void printUsage(const char* binName) {
  std::fprintf(stderr, "Usage: %s [options]\n", binName);
  std::fprintf(stderr, "Options:\n");
  std::fprintf(stderr, "  -h, --help:    Shows this help.\n");
  std::fprintf(stderr, "  -x, --xml:     Outputs the simulation execution "
    "using XML unless the\n                 simulation overrides this.\n");
  std::fprintf(stderr, "  -v, --version: Shows only information about "
    "ANL-Impl\n");
}


// _____________________________________________________________________________
void parseCommandLineArguments(int argc, char** argv) {
  struct option options[] = {
    { "xml", 0, NULL, 'x' },
    { "version", 0, NULL, 'v' },
    { "help", 0, NULL, 'h' },
    { NULL, 0, NULL, 0 }
  };
  optind = 1;
  while (true) {
    char c = getopt_long(argc, argv, "xvh", options, NULL);
    if (c == -1) {
      // No more options.
      break;
    }

    switch (c) {
      case 'h':
        // Usage information is requested.
        printUsage(argv[0]);
        std::exit(0);
        break;
      case 'x':
        // Requesting XML.
        Core::gDefaultOutModule = new Output::XMLOutputModule();
        break;
      case 'v':
        // Requesting header only.
        printHeader();
        std::exit(0);
        break;
    }
  }
}


// _____________________________________________________________________________
int main(int argc, char** argv) {
  auto startTime = std::chrono::high_resolution_clock::now();

  // Parse the command line.
  parseCommandLineArguments(argc, argv);

  // Set the default output module to the plain STDOUT module if none was set.
  if (Core::gDefaultOutModule == nullptr) {
    Core::gDefaultOutModule = new Output::StdOutOutputModule();
  }

  printHeader();
  std::fprintf(stderr, "[ INFO ] Starting ANL-Impl ANL simulator.\n");

  // Check that an entry point was set.
  if (Core::gEntryPointPtr == &Core::dummy) {
    std::fprintf(stderr, "[SEVERE] No entry point for simulation "
      "(ANLIMPL_MAIN) declared!\n");
    return 1;
  }

  // Remove "our" arguments from the command line for the case the simulation
  //  parses these on its own.
  argc -= optind - 1;
  argv = &argv[optind - 1];

  // Invoke the simulation entry point.
  int result = Core::gEntryPointPtr(argc, argv);
  if (result != 0) {
    std::fprintf(stderr, "[ WARN ] Result of simulation entry point is "
      "non-zero: %d\n", result);
  }

  // Notify the user of the simulation's end.
  auto endTime = std::chrono::high_resolution_clock::now();
  milliseconds diffTime =
    std::chrono::duration_cast<milliseconds>(endTime - startTime);
  std::fprintf(stderr, "[ INFO ] Simulation completed in %" PRId64 "ms.\n",
    static_cast<std::int64_t>(diffTime.count()));

  // Remove any created output module.
  if (Core::gDefaultOutModule != nullptr) {
    delete Core::gDefaultOutModule;
  }

  // Return the return value from the wrapped main function.
  return result;
}
