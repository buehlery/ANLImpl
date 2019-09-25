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

#ifndef ANL_CORE_ANL_ALGORITHM_H_
#define ANL_CORE_ANL_ALGORITHM_H_

#include <cstddef>
#include <vector>
#include "anl/core/anl.h"

// This file contains algorithms for the ANL from the CORE module.
namespace Core {


// An alias for representing a sender set as a network state.
using SenderSetRepresentation = NetworkState;


// The class that provides the sender-set algorithm.
class SenderSetComputer {
 public:
  // Constructor.
  explicit SenderSetComputer(const NetworkSetup* setup,
    const NetworkTopology* topo, const IntentionAssignment* intent);

  // Determines the sender set.
  SenderSetRepresentation getSenderSet();

 private:
  // The underlying network setup.
  const NetworkSetup* mSetup;

  // The underlying network topology.
  const NetworkTopology* mTopology;

  // The underlying intention assignment.
  const IntentionAssignment* mIntent;

  // The tic of the current iteration.
  std::size_t mIterationTic;

  // The sending components. In between iterations this contains the key set
  //  of the SenderSetRepresentation.
  std::unordered_set<const Component*> mSendingComponents;

  // The newly sending components. In between iterations this is empty.
  std::unordered_set<const Component*> mNewlySendingComponents;

  // The resulting sender set. We will use the same object for all sets S_i as
  //  the sets just grow. Thus we are able to create the final sender set using
  //  "mResult" as an accumulator. In this set, we represent sending components
  //  by "SENT" type component actions together with their message and tic.
  //  Non-sending components are represented by sentinel component actions
  //  consisting of the "IDLE" type.
  SenderSetRepresentation mResult;

  // Initializes an iteration of the algorithm.
  void initializeIteration();

  // Computes the tic set for the given tic.
  void computeTicSet(std::size_t tic);

  // Updates the current tic set for the given component.
  void updateTicSetForComponent(const Component* comp);

  // Completes an iteration of the algorithm.
  void completeIteration();

  // Finishes the algorithm by preparing the result.
  void finishAlgorithm();
};


// Alias for filtering functions that remove unwanted component actions from a
//  vector.
using FilterFunction = std::function<void(const NetworkSetup&,
  std::vector<ComponentAction>*)>;


// The class that provides the algorithms for the transition function.
class ANLComputer {
 public:
  // Constructor.
  ANLComputer(const NetworkSetup* setup, const NetworkTopology* topo,
    const IntentionAssignment* intent, FilterFunction filter);

  // This method provides \psi.
  std::vector<NetworkState> transition();

 private:
  // The underlying network setup.
  const NetworkSetup* mSetup;

  // The underlying network topology.
  const NetworkTopology* mTopology;

  // The underlying intention assignment.
  const IntentionAssignment* mIntent;

  // The filter function that will be used for pruning.
  const FilterFunction mFilter;

  // The sender set that is determined for the computation.
  SenderSetRepresentation mSenderSet;

  // Determines all possible component actions using the semantics of the ANL.
  std::vector<ComponentAction> getPossibleActions(const Component* comp) const;
};


// Filter function that filters nothing.
void ANLFilterNothing(const NetworkSetup& setup,
  std::vector<ComponentAction>* inout);

// Filter function that filters RECEIVE, SILENCE, and COLLISION using the
//  following rules.
//  1. If there are no neighbors sending, retain only SILENCE.
//  2. If there is one neighbor sending, retain only RECEIVE.
//  3. If there is more than one neighbor sending, retain only COLLISION.
void ANLFilterNaive(const NetworkSetup& setup,
  std::vector<ComponentAction>* inout);


}  // namespace Core

#endif  // ANL_CORE_ANL_ALGORITHM_H_
