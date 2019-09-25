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
// This file contains an example protocol simulation for a situation of a      *
// component that receives silence due to no other component being sending.    *
// *****************************************************************************

#include <anl/anlimpl.h>
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

// _____________________________________________________________________________
static Message msg;

class EchoServerComponent : public Component {
 private:
  // The protocol.
  void doAct(ANLView* view) override {
    if (view->hasPreviousAction()) {
      ComponentAction act = view->getPreviousAction();
      if (act.getType() == ActionType::RECEIVED) {
        view->send(act.getMessage(), 0);
        return;
      }
    }
    view->listen();
  }

  // Converts the component to a textual representation.
  std::string doGetId() const override { return "EchoServer"; }
};

// _____________________________________________________________________________
// Entry point.
ANLIMPL_MAIN(int argc, char** argv) {
  // Create the components.
  EchoServerComponent server;
  Component* comps[1];
  comps[0] = &server;

  // Create the messages.
  Message* msgs[1];
  msgs[0] = &msg;

  // Create the simulation.
  TrivialNetworkTopology tnt;
  Simulator sim(1);
  sim.useTopology(&tnt);
  sim.useComponents(comps, 1);
  sim.useMessages(msgs, 1);

  // Run the simulation.
  sim.run(3);

  return 0;
}
