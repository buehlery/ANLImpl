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
// This file contains an example protocol simulation for a situation in which  *
// a component sends A, B, and C in succession using a state machine.          *
// *****************************************************************************

#include <anl/anlimpl.h>
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

// _____________________________________________________________________________
class NamedMessage : public Message {
 public:
  // Constructor.
  explicit NamedMessage(const char* name) : mName(name) {}

 private:
  // The name of the message.
  const char* mName;

  // Retrieves a textual representation of the message.
  std::string doToString() const override { return mName; }

  // Retrieves an XML representation of the message.
  std::vector<std::string> doToXML() const override {
    std::vector<std::string> res;
    std::stringstream sstr;
    sstr << "<name>" << mName << "</name>";
    res.push_back(sstr.str());
    return res;
  }
};
static NamedMessage msgA("A");
static NamedMessage msgB("B");
static NamedMessage msgC("C");

enum class SenderState {
  INITIAL,
  SENDING_A,
  SENDING_B,
  SENDING_C
};

class SenderComponent : public StateMachineComponent<SenderState> {
 public:
  SenderComponent() : StateMachineComponent(SenderState::INITIAL) {}

 private:
  // The protocol.
  SenderState doStateAct(ANLView* view, SenderState state) override {
    switch (state) {
      case SenderState::INITIAL:
      default:
        view->send(&msgA, 0);
        return SenderState::SENDING_B;

      case SenderState::SENDING_A:
        view->send(&msgA, 5);
        return SenderState::SENDING_B;

      case SenderState::SENDING_B:
        view->send(&msgB, 5);
        return SenderState::SENDING_C;

      case SenderState::SENDING_C:
        view->send(&msgC, 5);
        return SenderState::SENDING_A;
    }
  }

  // Converts the component to a textual representation.
  std::string doGetId() const override {
    return "Sender";
  }
};

// _____________________________________________________________________________
// Entry point.
ANLIMPL_MAIN(int argc, char** argv) {
  // Create the components.
  SenderComponent sender;
  Component* comps[1];
  comps[0] = &sender;

  // Create the messages.
  Message* msgs[3];
  msgs[0] = &msgA;
  msgs[1] = &msgB;
  msgs[2] = &msgC;

  // Create the simulation.
  TrivialNetworkTopology tnt;
  Simulator sim(10);
  sim.useTopology(&tnt);
  sim.useComponents(comps, 1);
  sim.useMessages(msgs, 3);

  // Run the simulation.
  sim.run(10);

  return 0;
}
