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

#include "anl/core/anl_algorithm.h"
#include <algorithm>
#include <functional>
#include "anl/misc/asserts.h"

using std::placeholders::_1;
using std::size_t;

// This file contains algorithms for the ANL from the CORE module.
namespace Core {


// _____________________________________________________________________________
SenderSetComputer::SenderSetComputer(const NetworkSetup* setup,
    const NetworkTopology* topo, const IntentionAssignment* intent)
      : mSetup(setup), mTopology(topo), mIntent(intent), mResult(setup) {
  Misc::Asserts::require(setup != nullptr, "setup is nullptr");
  Misc::Asserts::require(topo != nullptr, "topology is nullptr");
  Misc::Asserts::require(intent != nullptr, "intent is nullptr");
  Misc::Asserts::require(!intent->isPartial(), "intent is partial and thus "
    "not usable");
}

// _____________________________________________________________________________
SenderSetRepresentation SenderSetComputer::getSenderSet() {
  for (size_t tic = 0; tic < mSetup->getTicsPerSlot(); tic++) {
    initializeIteration();
    Misc::Asserts::require(mNewlySendingComponents.size() == 0, "iteration not "
      "initialized!");
    computeTicSet(tic);
    completeIteration();
  }
  finishAlgorithm();
  return mResult;
}

// _____________________________________________________________________________
void SenderSetComputer::initializeIteration() {
  // We need to empty the set of newly sending components.
  mNewlySendingComponents.clear();
}

// _____________________________________________________________________________
void SenderSetComputer::computeTicSet(std::size_t tic) {
  Misc::Asserts::require(tic < mSetup->getTicsPerSlot(), "invalid tic");
  mIterationTic = tic;
  std::function<void(const Component*)> cb =
    std::bind(&Core::SenderSetComputer::updateTicSetForComponent, this,
      _1);
  mSetup->forEachComponent(cb);
  Misc::Asserts::require(mIterationTic == tic, "iteration tic has changed");
}

// _____________________________________________________________________________
void SenderSetComputer::updateTicSetForComponent(const Component* comp) {
  const ComponentIntention& intent = mIntent->getTraitFor(comp);

  // First, we check whether the component intends to send at all.
  if (intent.getType() != IntentionType::SEND
      && intent.getType() != IntentionType::SEND_FORCE) {
    // The component can not be part of the sender set.
    return;
  }
  Misc::Asserts::require(intent.getMessage() != nullptr, "invalid message: "
    "no message");

  // Second, we check that the current tic is matching, i.e. the component
  //  intends to start sending in this tic.
  if (mIterationTic != intent.getTic()) {
    // The component does not influence this iteration's changes to the sender
    //  set.
    return;
  }

  // At this point, we know for certain that the component influences the
  //  changes to the sender set in this iteration.

  // First case: The component intends to send without carrier sensing
  //  (SEND_FORCE). In this case, we just add it to the sender set.
  if (intent.getType() == IntentionType::SEND_FORCE) {
    mNewlySendingComponents.insert(comp);
    mResult.setTraitFor(comp, ComponentAction(*mSetup, ActionType::SENT,
      mIterationTic, intent.getMessage()));
    return;
  }

  // Second case: The component employs carrier sensing. We can heed the intent
  //  if and only if carrier sensing detects a free medium. However, the
  //  definition of the S_i set simplifies this: We need to check if any of the
  //  already sending components can reach this component. If so, carrier
  //  sensing detects an occupied medium and the component does not send.
  //  Otherwise the medium is detected as free and the component does send.
  for (const Component* alreadySending : mSendingComponents) {
    if (mTopology->canReach(alreadySending, comp)) {
      // Component "alreadySending" is detected by carrier sensing.
      //  Thus, "comp" does not send.
      return;
    }
  }

  // No component has been detected by carrier sensing. Thus "comp" does send.
  mNewlySendingComponents.insert(comp);
  mResult.setTraitFor(comp, ComponentAction(*mSetup, ActionType::SENT,
    mIterationTic, intent.getMessage()));
}

// _____________________________________________________________________________
void SenderSetComputer::completeIteration() {
  // We add all the newly sending components *now* to the set of sending
  //  components in order to not influence the now-finished iteration.
  for (const Component* newlySending : mNewlySendingComponents) {
    mSendingComponents.insert(newlySending);
  }
}

// _____________________________________________________________________________
void SenderSetComputer::finishAlgorithm() {
  // We still need to assign a sentinel value ("IDLE" type) to each uncovered
  //  component.
  mSetup->forEachComponent([this](const Component* comp) {
    if (mSendingComponents.count(comp) > 0) {
      // This component is sending and thus already has something assigned to
      //  it.
      return;
    }

    // "comp" is not sending: We need to construct a sentinel and map "comp" to
    //  this sentinel.
    mResult.setTraitFor(comp, ComponentAction(*mSetup, ActionType::IDLE, 0,
      nullptr));
  });
}

// _____________________________________________________________________________
ANLComputer::ANLComputer(const NetworkSetup* setup, const NetworkTopology* topo,
    const IntentionAssignment* intent, FilterFunction filter) : mSetup(setup),
      mTopology(topo), mIntent(intent), mFilter(filter), mSenderSet(setup) {}

// _____________________________________________________________________________
std::vector<NetworkState> ANLComputer::transition() {
  // The transition algorithm consists of two main phases.
  //  In the first phase, we use the SenderSetComputer in order to determine the
  //  sender set.
  //  In the second phase, we determine the possible component actions for each
  //  component individually, as all components are independent from one
  //  another. In order to limit the effect of combinatorial explosion, we use
  //  a filter that determines whether or not we continue on certain paths in
  //  the computation tree. The canonical behavior -- i.e. all possible network
  //  states -- is achieved by supplying the trivial filter that removes
  //  nothing. We strongly advise against the use of this trivial filter for
  //  scenarios with |C| > 7. The exact merging algorithm is described below.

  // Phase 1. We determine the sender set.
  SenderSetComputer SenderSetComputer(mSetup, mTopology, mIntent);
  mSenderSet = SenderSetComputer.getSenderSet();

  // Phase 2. We determine possible actions and build up a set of partial
  //  network states. For merging, we use the filter to determine all
  //  component actions that we will consider for a given component. We then
  //  clone the elements of the result set once per possible component action,
  //  thus creating all partial results that we want to consider. Once an
  //  element is in consideration, there is no way of removing it from the set
  //  of results. The filter must allow at least one possible component
  //  action.
  //  We use two vectors: One is a back buffer, one is a front buffer in order
  //  to allow us to create the network states almost in-place.
  std::vector<NetworkState> buffer1, buffer2;
  std::vector<NetworkState>* frontBuffer = &buffer1;
  std::vector<NetworkState>* backBuffer = &buffer2;
  frontBuffer->emplace_back(mSetup);
  mSetup->forEachComponent(
    [this, &frontBuffer, &backBuffer](const Component* comp) {
      // Sub-step 1. We determine the possible component actions.
      std::vector<ComponentAction> possibleActions = getPossibleActions(comp);

      // Sub-step 2. We use the filter to prune the set of possible component
      //  actions. Currently, only the set of all possible component actions and
      //  the network setup is passed to the filter, as all possible filters
      //  that we intend can be implemented using only this information (i.e.
      //  trivial, counting senders).
      mFilter(*mSetup, &possibleActions);
      Misc::Asserts::require(possibleActions.size() > 0, "filter removed all "
        "possibilities");

      // Sub-step 3. We clone the results for each of the component actions.
      //  We first swap the front buffer (which contains the actual partial
      //  network states) and the back buffer. We then clear the front buffer
      //  and loop throught the back buffer. For each entry we clone and extend
      //  this entry into the front buffer. After this, the front buffer
      //  fulfills the invariant from the beginning. The back buffer is no
      //  longer needed at this point but retained for the next iteration.
      std::vector<NetworkState>* tmp = frontBuffer;
      frontBuffer = backBuffer;
      backBuffer = tmp;

      frontBuffer->clear();
      for (const NetworkState& state : *backBuffer) {
        for (const ComponentAction& action : possibleActions) {
          // Clone and extend into the front buffer.
          frontBuffer->emplace_back(state);  // Here we clone.
          frontBuffer->back().setTraitFor(comp, action);
        }
      }
  });
  return *frontBuffer;
}

// _____________________________________________________________________________
std::vector<ComponentAction> ANLComputer::getPossibleActions(
    const Component* comp) const {
  std::vector<ComponentAction> actions;
  const ComponentIntention& intent = mIntent->getTraitFor(comp);

  // Condition 1: IDLE.
  if (intent.getType() == IntentionType::IDLE) {
    // Has to be ActionType::IDLE.
    actions.emplace_back(*mSetup, ActionType::IDLE, 0, nullptr);
    return actions;
  }

  // Condition 5: SENT and Condition 6: CANCELLED.
  if (intent.getType() == IntentionType::SEND
      || intent.getType() == IntentionType::SEND_FORCE) {
    // Outcome depends on whether "comp" is in the sender set.
    const ComponentAction& senderSetQuery = mSenderSet.getTraitFor(comp);
    if (senderSetQuery.getType() == ActionType::IDLE) {
      // Sentinel value was returned. Thus: Cancelled.
      actions.emplace_back(*mSetup, ActionType::CANCELLED, intent.getTic(),
        intent.getMessage());
      return actions;
    } else {
      // Success value was returned. Thus: Sent.
      actions.emplace_back(*mSetup, ActionType::SENT, intent.getTic(),
        intent.getMessage());
      return actions;
    }
  }

  // Condition 2: COLLISION, Condition 3: SILENCE, and Condition 4: RECEIVED.
  if (intent.getType() == IntentionType::LISTEN) {
    // Receiving is possible if and only if the set of sending neighbors is not
    //  empty. In this case, every transmission of the sending neighbors has a
    //  possibility to be received.
    bool hasMessages = false;
    mSetup->forEachComponent(
      [this, &actions, &comp, &hasMessages](const Component* potential) {
        if (mTopology->canReach(potential, comp)) {
          const ComponentAction& potentialQuery =
            mSenderSet.getTraitFor(potential);
          if (potentialQuery.getType() == ActionType::SENT) {
            // Sending neighbor. Add message to the possibilities.
            actions.emplace_back(*mSetup, ActionType::RECEIVED,
              potentialQuery.getTic(), potentialQuery.getMessage());

            // If not yet happened, add a collision to the possibilities.
            if (!hasMessages) {
              hasMessages = true;
              actions.emplace_back(*mSetup, ActionType::COLLISION, 0, nullptr);
            }
          }
        }
    });

    // Add silence if no message was possible.
    if (!hasMessages) {
      actions.emplace_back(*mSetup, ActionType::SILENCE, 0, nullptr);
    }

    return actions;
  }

  Misc::Asserts::require(false, "this point should not be reached, the "
    "conditions above are exhaustive");
  return actions;
}

// _____________________________________________________________________________
void ANLFilterNothing(const NetworkSetup& setup,
    std::vector<ComponentAction>* inout) {
  // Remove duplicate entries. This is a set, after all.
  std::vector<ComponentAction> tmp;
  for (const ComponentAction& act : *inout) {
    bool add = true;
    for (const ComponentAction& alreadyAdded : tmp) {
      if (alreadyAdded.getType() == act.getType()
          && alreadyAdded.getTic() == act.getTic()
          && alreadyAdded.getMessage() == act.getMessage()) {
        add = false;
        break;
      }
    }
    if (add) {
      tmp.push_back(act);
    }
  }

  // Use the unique-ified "tmp" vector and replace the contents of inout with
  //  it.
  inout->swap(tmp);
}

// _____________________________________________________________________________
void ANLFilterNaive(const NetworkSetup& setup,
    std::vector<ComponentAction>* inout) {
  // Count the number of "RECEIVED"s, this is the number of sending neighbors.
  //  Also, count the number of "COLLISION"s. For every transmission this is
  //  possible. So if this number is 0, then we do not need to do anything as it
  //  is no transmission (at least not on the receiving end).
  int sendingNeighbors = 0;
  int collisions = 0;
  for (const ComponentAction& ca : *inout) {
    if (ca.getType() == ActionType::RECEIVED) {
      sendingNeighbors++;
    } else if (ca.getType() == ActionType::COLLISION) {
      collisions++;
    }
  }

  // Check whether we need to do something.
  if (collisions == 0) {
    return;
  }

  // Case 1. More than one sending neighbor.
  if (sendingNeighbors > 1) {
    // We use a trick as we can construct COLLISION without any information.
    inout->clear();
    inout->emplace_back(setup, ActionType::COLLISION, 0, nullptr);
    return;
  }

  // Case 2. Exactly one sending neighbor.
  // We have no receiving duplicates, at this point those should already be
  //  converted to collisions by the statements above.
  Misc::Asserts::require(sendingNeighbors == 1, "this should be 1, as above "
    "conditions are exhaustive");

  // As there is only one RECEIVED, we just remove every other type.
  inout->erase(std::remove_if(inout->begin(), inout->end(),
    [](const ComponentAction& action) {
      return action.getType() != ActionType::RECEIVED;
    }), inout->end());
  Misc::Asserts::require(inout->size() == 1, "more than one result left after "
    "removing for single sender");
}


}  // namespace Core
