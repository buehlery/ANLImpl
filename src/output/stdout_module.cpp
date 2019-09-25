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

#include <cstdio>
#include "anl/output/output.h"

// This file contains an output module.
namespace Output {


// _____________________________________________________________________________
void StdOutOutputModule::doSimulationBegin(std::size_t numSlots,
    const Core::NetworkSetup* setup, const Core::NetworkTopology* topology) {
  std::printf("# Starting simulation with %zu slots `a %zu tics.\n", numSlots,
    setup->getTicsPerSlot());
  std::printf("# The following components will be used in the following "
    "order:\n");
  setup->forEachComponent([](const Core::Component* comp) {
    std::printf("#  - %s\n", comp->getId().c_str());
  });
  std::printf("\n");
}

// _____________________________________________________________________________
void StdOutOutputModule::doSlotBegin(std::size_t slotNumber) {
  std::printf("# Beginning simulation of slot %zu.\n", slotNumber);
}

// _____________________________________________________________________________
void StdOutOutputModule::doIntentChosen(
    const Core::IntentionAssignment& intent) {
  std::printf("# Protocol executed. Chosen intentions:\n");
  std::printf("%s\n", intent.toString().c_str());
}

// _____________________________________________________________________________
void StdOutOutputModule::doTransitionComputed(
    const std::vector<Core::NetworkState>& outcomes) {
  std::printf("# ANL returned %zu possible successor states.\n",
    outcomes.size());
}

// _____________________________________________________________________________
void StdOutOutputModule::doResultChosen(const Core::NetworkState& state) {
  std::printf("# Result chosen from possible results.\n");
  std::printf("%s\n", state.toString().c_str());
}

// _____________________________________________________________________________
void StdOutOutputModule::doSlotEnd() {
  std::printf("\n");
}

// _____________________________________________________________________________
void StdOutOutputModule::doSimulationEnd() {}


}  // namespace Output
