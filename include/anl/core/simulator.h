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

#ifndef ANL_CORE_SIMULATOR_H_
#define ANL_CORE_SIMULATOR_H_

#include <cstddef>
#include "anl/core/anl.h"
#include "anl/core/errortrace.h"
#include "anl/core/types.h"
#include "anl/output/output.h"

// This file contains the simulator from the CORE module.
namespace Core {


// The interface of the simulator that is used by the protocol designer in order
//  to perform simulations.
class Simulator {
 public:
  // Constructor.
  explicit Simulator(std::size_t ticsPerSlot);

  // Sets the topology used by the simulator.
  void useTopology(const NetworkTopology* topo);

  // Sets the output module used by the simulator.
  //  This is not required to be used, as a default value will be used if
  //  no output module is set. The method can however be used to enforce a
  //  specific (potentially custom) output module.
  void useOutputModule(Output::OutputModule* outModule);

  // Adds components to the simulation. The components are expected in a
  //  C-array.
  void useComponents(Component* const* compStart, std::size_t count);

  // Adds messages to the simulation. The messages are expected in a C-array.
  void useMessages(const Message* const* msgStart, std::size_t count);

  // Performs the simulation for the given amount of slots. Must not be
  //  repeated. Must not be combined with runSingle.
  void run(std::size_t numSlots);

  // Runs a single slot. May be repeated multiple times. A sequence of runSingle
  //  calls must be terminated by a single call to endSingle. Must not be
  //  combined with run(). The argument shall be passed as the intended number
  //  of slots that will be run.
  void runSingle(std::size_t intendedSlots);

  // Terminates a sequence of runSingle calls.
  void endSingle();

 private:
  // The error tracing helper.
  ErrorTracer mErrorTracer;

  // The output module that is used for storing the abstract protocol execution.
  Output::OutputModule* mOutputModule;

  // The network setup that is being simulated.
  NetworkSetup mSetup;

  // The current network topology.
  const NetworkTopology* mTopology;

  // The current slot number.
  std::size_t mSlotNumber;

  // The previous network state.
  NetworkState mPreviousState;

  // The ANL that is the backend of the simulator.
  ANL mANL;

  // Whether or not simulation has already begun.
  bool mHasBegun;

  // Simulates a single slot.
  void runSlot();
};


// Declaration of the default output module set by the entry point.
extern Output::OutputModule* gDefaultOutModule;


}  // namespace Core

#endif  // ANL_CORE_SIMULATOR_H_
