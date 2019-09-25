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
// three components send distinct messages in a circle (trivial topology).     *
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

class InoutComponent : public Component {
 public:
  InoutComponent(const char* name, const Message* in, const Message* out)
    : mName(name), mIn(in), mOut(out) {}

 private:
  // The name of this component.
  const char* mName;

  // The message this component is waiting for.
  const Message* mIn;

  // The message this component will send in response.
  const Message* mOut;

  // The protocol.
  void doAct(ANLView* view) override {
    // Jump start if initial.
    if (view->getSlotNumber() == 0 && mIn == &msgA) {
      view->send(mOut, 0);
      return;
    }

    if (view->hasPreviousAction()) {
      ComponentAction act = view->getPreviousAction();
      if (act.getType() == ActionType::RECEIVED) {
        if (act.getMessage() == mIn) {
          view->send(mOut, 0);
          return;
        }
      }
    }
    view->listen();
  }

  // Converts the component to a textual representation.
  std::string doGetId() const override {
    return mName;
  }
};

// _____________________________________________________________________________
// Entry point.
ANLIMPL_MAIN(int argc, char** argv) {
  // Create the components.
  InoutComponent circAB("CircAB", &msgA, &msgB);
  InoutComponent circBC("CircBC", &msgB, &msgC);
  InoutComponent circCA("CircCA", &msgC, &msgA);

  Component* comps[3];
  comps[0] = &circAB;
  comps[1] = &circBC;
  comps[2] = &circCA;

  // Create the messages.
  Message* msgs[3];
  msgs[0] = &msgA;
  msgs[1] = &msgB;
  msgs[2] = &msgC;

  // Create the simulation.
  TrivialNetworkTopology tnt;
  Simulator sim(1);
  sim.useTopology(&tnt);
  sim.useComponents(comps, 3);
  sim.useMessages(msgs, 3);

  // Run the simulation.
  sim.run(10);

  return 0;
}
