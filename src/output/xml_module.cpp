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
#include <string>
#include <vector>
#include "anl/output/output.h"

// This file contains an output module.
namespace Output {


// _____________________________________________________________________________
void XMLOutputModule::doSimulationBegin(std::size_t numSlots,
    const Core::NetworkSetup* setup, const Core::NetworkTopology* topology) {
  std::printf("<?xml version=\"1.0\" encoding=\"ascii\"?>\n");
  std::printf("<simulation>\n");
  std::printf("  <slotcount>%zu</slotcount>\n", numSlots);
  std::printf("  <ticsperslot>%zu</ticsperslot>\n", setup->getTicsPerSlot());

  std::printf("  <components>\n");
  setup->forEachComponent([](const Core::Component* comp) {
    std::printf("    <component id=\"%s\">\n", comp->getId().c_str());
    std::vector<std::string> xmlRepr = comp->toXML();
    for (const std::string& rpr : xmlRepr) {
      std::printf("        %s\n", rpr.c_str());
    }
    std::printf("    </component>\n");
  });
  std::printf("  </components>\n");

  std::printf("  <topology>\n");
  setup->forEachComponent([setup, topology](const Core::Component* sndr) {
    setup->forEachComponent([topology, sndr](const Core::Component* rcvr) {
      if (topology->canReach(sndr, rcvr)) {
        std::printf("    <edge>\n");
        std::printf("      <from>%s</from>\n", sndr->getId().c_str());
        std::printf("      <to>%s</to>\n", rcvr->getId().c_str());
        std::printf("    </edge>\n");
      }
    });
  });
  std::printf("  </topology>\n");
  std::printf("  <execution>\n");
}

// _____________________________________________________________________________
void XMLOutputModule::doSlotBegin(std::size_t slotNumber) {
  std::printf("    <slot num=\"%zu\">\n", slotNumber);
}

// _____________________________________________________________________________
void XMLOutputModule::doIntentChosen(
    const Core::IntentionAssignment& intent) {
  std::printf("      <intention>\n");
  std::vector<std::string> xmlRepr = intent.toXML();
  for (const std::string& rpr : xmlRepr) {
    std::printf("        %s\n", rpr.c_str());
  }
  std::printf("      </intention>\n");
}

// _____________________________________________________________________________
void XMLOutputModule::doTransitionComputed(
    const std::vector<Core::NetworkState>& outcomes) {
  std::printf("      <choices>\n");
  for (const Core::NetworkState& state : outcomes) {
    std::printf("        <choice>\n");
    std::vector<std::string> xmlRepr = state.toXML();
    for (const std::string& rpr : xmlRepr) {
      std::printf("          %s\n", rpr.c_str());
    }
    std::printf("        </choice>\n");
  }
  std::printf("      </choices>\n");
}

// _____________________________________________________________________________
void XMLOutputModule::doResultChosen(const Core::NetworkState& state) {
  std::printf("      <result>\n");
  std::vector<std::string> xmlRepr = state.toXML();
  for (const std::string& rpr : xmlRepr) {
    std::printf("        %s\n", rpr.c_str());
  }
  std::printf("      </result>\n");
}

// _____________________________________________________________________________
void XMLOutputModule::doSlotEnd() {
  std::printf("    </slot>\n");
}

// _____________________________________________________________________________
void XMLOutputModule::doSimulationEnd() {
  std::printf("  </execution>\n");
  std::printf("</simulation>\n");
}


}  // namespace Output
