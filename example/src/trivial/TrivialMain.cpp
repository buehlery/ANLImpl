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

// *****************************************************************************
// This file contains an example protocol simulation for a trivial protocol.   *
// *****************************************************************************

#include <anl/anlimpl.h>
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

// _____________________________________________________________________________
// The message that represents "HALLO".
class HalloMessage : public Message {
 private:
  // Retrieves a textual representation of the message.
  std::string doToString() const override { return "HALLO"; }

  // Retrieves an XML representation of the message.
  std::vector<std::string> doToXML() const override {
    std::vector<std::string> res;
    res.push_back("<type>HALLO</type>");
    return res;
  };
};
static HalloMessage msgHallo;

// _____________________________________________________________________________
// States for components.
enum class HelloState {
  INITIAL,
  WAITING,
  DONE
};

// _____________________________________________________________________________
// Usual components.
class HalloComponent : public StateMachineComponent<HelloState> {
 public:
  // Constructor.
  explicit HalloComponent(std::size_t prio)
    : StateMachineComponent(HelloState::INITIAL), mPriority(prio) {}

 private:
  // The priority of this component.
  std::size_t mPriority;

  // The protocol callback.
  HelloState doStateAct(ANLView* view, HelloState state) override;

  // Converts the component to a textual representation.
  std::string doGetId() const override {
    std::stringstream sstr;
    sstr << "Inferior" << mPriority;
    return sstr.str();
  }
};

// _____________________________________________________________________________
// The single leader component that initiates the protocol.
class LeaderComponent : public StateMachineComponent<HelloState> {
 public:
  // Constructor.
  LeaderComponent() : StateMachineComponent(HelloState::INITIAL) {}

 private:
  // The protocol callback.
  HelloState doStateAct(ANLView* view, HelloState state) override;

  // Converts the component to a textual representation.
  std::string doGetId() const override { return "Leader"; }
};

// _____________________________________________________________________________
// The "Hallo" protocol (usual case).
HelloState HalloComponent::doStateAct(ANLView* view, HelloState state) {
  // Resend cancelled.
  if (view->hasPreviousAction()) {
    ComponentAction prevAction = view->getPreviousAction();
    if (prevAction.getType() == ActionType::CANCELLED) {
      view->send(prevAction.getMessage(), prevAction.getTic());
      return state;
    }
  }

  // State machine logic.
  switch (state) {
    case HelloState::INITIAL:
      view->listen();
      return HelloState::WAITING;
    case HelloState::WAITING:
      {
        ComponentAction prevAction = view->getPreviousAction();
        if (prevAction.getType() == ActionType::RECEIVED) {
          view->send(prevAction.getMessage(), mPriority);
          return HelloState::DONE;
        } else {
          view->listen();
          return HelloState::WAITING;
        }
      }
    case HelloState::DONE:
      view->idle();
      return state;
  }
  return state;
}

// _____________________________________________________________________________
// The "Hallo" protocol (leader).
HelloState LeaderComponent::doStateAct(ANLView* view,
    HelloState state) {
  // Resend cancelled.
  if (view->hasPreviousAction()) {
    ComponentAction prevAction = view->getPreviousAction();
    if (prevAction.getType() == ActionType::CANCELLED) {
      view->send(prevAction.getMessage(), prevAction.getTic());
      return state;
    }
  }

  // State machine logic.
  switch (state) {
    case HelloState::INITIAL:
      view->send(&msgHallo, 3);
      return HelloState::DONE;
    case HelloState::WAITING:
      view->idle();
      return state;
    case HelloState::DONE:
      view->idle();
      return state;
  }
  return state;
}

// _____________________________________________________________________________
// Entry point.
ANLIMPL_MAIN(int argc, char** argv) {
  // Create the components.
  Component* comps[10];
  comps[0] = new LeaderComponent();
  for (std::size_t i = 1; i < 10; i++) {
    comps[i] = new HalloComponent(i);
  }

  // Create the messages.
  Message* msgs[1];
  msgs[0] = &msgHallo;

  // Create the topology.
  ExplicitNetworkTopology ent;
  ent.addEdge(comps[0], comps[1]);
  ent.addEdge(comps[0], comps[2]);
  ent.addEdge(comps[1], comps[3]);
  ent.addEdge(comps[1], comps[4]);
  ent.addEdge(comps[2], comps[5]);
  ent.addEdge(comps[2], comps[6]);
  ent.addEdge(comps[3], comps[7]);
  ent.addEdge(comps[3], comps[8]);
  ent.addEdge(comps[4], comps[9]);

  ent.addEdge(comps[0], comps[3]);
  ent.addEdge(comps[2], comps[3]);

  // Create the simulation.
  Simulator sim(20);
  sim.useTopology(&ent);
  sim.useComponents(comps, 10);
  sim.useMessages(msgs, 1);

  // Run the simulation.
  sim.run(5);

  // Clean up.
  for (std::size_t i = 0; i < 10; i++) {
    delete comps[i];
  }

  return 0;
}
