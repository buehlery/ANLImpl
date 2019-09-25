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

#ifndef ANL_OUTPUT_OUTPUT_H_
#define ANL_OUTPUT_OUTPUT_H_

#include <cstddef>
#include <vector>
#include "anl/core/anl.h"
#include "anl/core/types.h"

// This file contains the declaration of output modules.
namespace Output {


// The base class for output modules.
class OutputModule {
 public:
  virtual ~OutputModule() {}

  // Notify the module of the beginning of the simulation.
  void onSimulationBegin(std::size_t numSlots, const Core::NetworkSetup* setup,
    const Core::NetworkTopology* topology);

  // Notify the module of the beginning slot.
  void onSlotBegin(std::size_t slotNumber);

  // Notify the module of the chosen intention assignment
  void onIntentChosen(const Core::IntentionAssignment& intent);

  // Notify the module of the possible results of transitioning.
  void onTransitionComputed(const std::vector<Core::NetworkState>& outcomes);

  // Notify the module of the chosen result of transitioning.
  void onResultChosen(const Core::NetworkState& state);

  // Notify the module of the ending slot.
  void onSlotEnd();

  // Notify the module of the ending simulation.
  void onSimulationEnd();

 private:
  // Notify the module of the beginning of the simulation.
  virtual void doSimulationBegin(std::size_t numSlots,
    const Core::NetworkSetup* setup, const Core::NetworkTopology* topology) = 0;

  // Notify the module of the beginning slot.
  virtual void doSlotBegin(std::size_t slotNumber) = 0;

  // Notify the module of the chosen intention assignment
  virtual void doIntentChosen(const Core::IntentionAssignment& intent) = 0;

  // Notify the module of the possible results of transitioning.
  virtual void doTransitionComputed(
    const std::vector<Core::NetworkState>& outcomes) = 0;

  // Notify the module of the chosen result of transitioning.
  virtual void doResultChosen(const Core::NetworkState& state) = 0;

  // Notify the module of the ending slot.
  virtual void doSlotEnd() = 0;

  // Notify the module of the ending simulation.
  virtual void doSimulationEnd() = 0;
};


// An implementation of the output module that logs to STDOUT.
class StdOutOutputModule : public OutputModule {
 private:
  // Notify the module of the beginning of the simulation.
  void doSimulationBegin(std::size_t numSlots, const Core::NetworkSetup* setup,
    const Core::NetworkTopology* topology) override;

  // Notify the module of the beginning slot.
  void doSlotBegin(std::size_t slotNumber) override;

  // Notify the module of the chosen intention assignment
  void doIntentChosen(const Core::IntentionAssignment& intent) override;

  // Notify the module of the possible results of transitioning.
  void doTransitionComputed(const std::vector<Core::NetworkState>& outcomes)
    override;

  // Notify the module of the chosen result of transitioning.
  void doResultChosen(const Core::NetworkState& state) override;

  // Notify the module of the ending slot.
  void doSlotEnd() override;

  // Notify the module of the ending simulation.
  void doSimulationEnd() override;
};


// An implementation of the output module that logs to STDOUT using XML.
class XMLOutputModule : public OutputModule {
 private:
  // Notify the module of the beginning of the simulation.
  void doSimulationBegin(std::size_t numSlots, const Core::NetworkSetup* setup,
    const Core::NetworkTopology* topology) override;

  // Notify the module of the beginning slot.
  void doSlotBegin(std::size_t slotNumber) override;

  // Notify the module of the chosen intention assignment
  void doIntentChosen(const Core::IntentionAssignment& intent) override;

  // Notify the module of the possible results of transitioning.
  void doTransitionComputed(const std::vector<Core::NetworkState>& outcomes)
    override;

  // Notify the module of the chosen result of transitioning.
  void doResultChosen(const Core::NetworkState& state) override;

  // Notify the module of the ending slot.
  void doSlotEnd() override;

  // Notify the module of the ending simulation.
  void doSimulationEnd() override;
};


}  // namespace Output

#endif  // ANL_OUTPUT_OUTPUT_H_
