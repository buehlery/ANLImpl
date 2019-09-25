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
// This file contains an example protocol simulation for a situation similar.  *
// to the motivational one in the report.                                      *
// *****************************************************************************

#include <anl/anlimpl.h>
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

// _____________________________________________________________________________
static Message msg;
class SendingComponent : public Component {
 public:
  explicit SendingComponent(std::size_t prio) : mPriority(prio) {}

 private:
  // The priority of this component.
  std::size_t mPriority;

  // The protocol.
  void doAct(ANLView* view) override { view->send(&msg, mPriority); }

  // Converts the component to a textual representation.
  std::string doGetId() const override {
    std::stringstream sstr;
    sstr << "Comp" << mPriority;
    return sstr.str();
  }
};

// _____________________________________________________________________________
// Entry point.
ANLIMPL_MAIN(int argc, char** argv) {
  // Create the components.
  SendingComponent comp0(0);
  SendingComponent comp1(1);
  SendingComponent comp2(2);
  Component* comps[3];
  comps[0] = &comp0;
  comps[1] = &comp1;
  comps[2] = &comp2;

  // Create the messages.
  Message* msgs[1];
  msgs[0] = &msg;

  // Create the simulation.
  ExplicitNetworkTopology ent;
  ent.addEdge(comps[0], comps[1]);
  ent.addEdge(comps[1], comps[2]);
  Simulator sim(3);
  sim.useTopology(&ent);
  sim.useComponents(comps, 3);
  sim.useMessages(msgs, 1);

  // Run the simulation.
  sim.run(1);

  return 0;
}
