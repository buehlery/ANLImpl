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
// Author: Yannick Bühler.
//
// Part of ANL-Impl.

#include "anl/core/simulator.h"
#include <cstdio>
#include <vector>
#include "anl/core/entry_point.h"
#include "anl/misc/asserts.h"

// This file contains the simulator from the CORE module.
namespace Core {


// _____________________________________________________________________________
Output::OutputModule* gDefaultOutModule = nullptr;

// _____________________________________________________________________________
Simulator::Simulator(std::size_t ticsPerSlot) :
    mOutputModule(gDefaultOutModule), mSetup(ticsPerSlot), mTopology(nullptr),
    mSlotNumber(0), mPreviousState(&mSetup),
    mANL(&mSetup, ANLSemantics::NAIVE), mHasBegun(false) {}

// _____________________________________________________________________________
void Simulator::useTopology(const NetworkTopology* topo) {
  mErrorTracer.enter("Simulator::useTopology()");
  mErrorTracer.require(topo != nullptr, "Topology must not be 'nullptr'.");
  mTopology = topo;
  mErrorTracer.leave();
}

// _____________________________________________________________________________
void Simulator::useOutputModule(Output::OutputModule* outModule) {
  mErrorTracer.enter("Simulator::useOutputModule()");
  mErrorTracer.require(outModule != nullptr, "Output module must not be "
    "'nullptr'.");
  mOutputModule = outModule;
  mErrorTracer.leave();
}

// _____________________________________________________________________________
void Simulator::useComponents(Component* const* compStart, std::size_t count) {
  mErrorTracer.enter("Simulator::useComponents()");
  mErrorTracer.require(compStart != nullptr, "Component pointer array must not "
    "be 'nullptr'.");
  for (std::size_t i = 0; i < count; i++) {
    mErrorTracer.enter("Stepping through component pointer array");
    mErrorTracer.require(compStart[i] != nullptr, "Component pointer must not "
      "be 'nullptr'.");
    mErrorTracer.require(!mSetup.isComponent(*compStart[i]), "Components must "
      "not be registered more than once.");
    mSetup.registerComponent(compStart[i]);
    mErrorTracer.leave();
  }
  mErrorTracer.leave();
}

// _____________________________________________________________________________
void Simulator::useMessages(const Message* const* msgStart, std::size_t count) {
  mErrorTracer.enter("Simulator::useMessages()");
  mErrorTracer.require(msgStart != nullptr, "Message pointer array must not "
    "be 'nullptr'.");
  for (std::size_t i = 0; i < count; i++) {
    mErrorTracer.enter("Stepping through message pointer array");
    mErrorTracer.require(msgStart[i] != nullptr, "Message pointer must not be "
      "'nullptr'.");
    mErrorTracer.require(!mSetup.isMessage(msgStart[i]), "Messages must not be "
      "registered more than once.");
    mSetup.registerMessage(msgStart[i]);
    mErrorTracer.leave();
  }
  mErrorTracer.leave();
}

// _____________________________________________________________________________
void Simulator::run(std::size_t numSlots) {
  mErrorTracer.enter("Simulator::run()");
  mErrorTracer.enter("Checking prerequisites");
  mErrorTracer.require(numSlots != 0, "Simulation duration must be greater "
    "than zero.");
  mErrorTracer.leave();

  for (std::size_t i = 0; i < numSlots; i++) {
    runSingle(numSlots);
  }
  endSingle();
  mErrorTracer.leave();
}

// _____________________________________________________________________________
void Simulator::runSingle(std::size_t intendedSlots) {
  mErrorTracer.enter("Simulator::runSingle()");
  mErrorTracer.enter("Checking prerequisites");
  mErrorTracer.require(mTopology != nullptr, "Network topology must be set.");
  mErrorTracer.require(mOutputModule != nullptr, "Output module must be set.");
  mErrorTracer.leave();

  if (!mHasBegun) {
    mHasBegun = true;
    std::fprintf(stderr, "[ INFO ] Simulating %zu slots.\n", intendedSlots);
    mOutputModule->onSimulationBegin(intendedSlots, &mSetup, mTopology);
  }
  runSlot();
  mSlotNumber++;

  mErrorTracer.leave();
}

// _____________________________________________________________________________
void Simulator::endSingle() {
  mErrorTracer.enter("Simulator::endSingle()");
  mErrorTracer.enter("Checking prerequisites");
  mErrorTracer.require(mOutputModule != nullptr, "Output module must be set.");
  mErrorTracer.leave();
  mOutputModule->onSimulationEnd();
  mErrorTracer.leave();
}

// _____________________________________________________________________________
void Simulator::runSlot() {
  mErrorTracer.enter("Running slot");
  mOutputModule->onSlotBegin(mSlotNumber);

  IntentionAssignment targetIntent(&mSetup);
  NetworkState* oldState = (mSlotNumber == 0) ? nullptr : &mPreviousState;

  // Run the protocols.
  std::fprintf(stderr, "[ INFO ] Running network protocol for slot %zu.\n",
    mSlotNumber);
  mANL.runSlot(mSlotNumber, oldState, &targetIntent);

  // Ensure the intent is not partial.
  mErrorTracer.require(!targetIntent.isPartial(), "Protocol produced a partial "
    "intention assignment.");

  // Here: Intent is finished, pass the intent to the output module.
  mOutputModule->onIntentChosen(targetIntent);

  // Perform the transition in the ANL.
  std::vector<NetworkState> outcomes =
    mANL.transition(mTopology, &targetIntent);
  mOutputModule->onTransitionComputed(outcomes);

  // TODO(yb36): non-determinism
  Misc::Asserts::require(outcomes.size() == 1, "can not deal with "
    "non-determinism yet");

  // Here: State is finished, pass the state to the output module.
  mPreviousState = outcomes.front();
  mOutputModule->onResultChosen(mPreviousState);

  mOutputModule->onSlotEnd();
  mErrorTracer.leave();
}


}  // namespace Core
