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
// This file contains a test for requirement #3                                *
// *****************************************************************************

#include <anl/anlimpl.h>
#include <cstddef>

// _____________________________________________________________________________
static Message msg;

class DoEverythingComponent : public Component {
 public:
  // Constructor.
  explicit DoEverythingComponent(std::size_t bitsSkip) : mSkip(bitsSkip) {}

 private:
  // The number of bits in the sequence to skip. We skip in multiples of 3 in
  //  order to allow to skip 8 different settings at a time. Thus we need 8^n
  //  slots in order to allow n components to explore every setting.
  std::size_t mSkip;

  // The protocol.
  void doAct(ANLView* view) override {
    std::size_t alotted = (view->getSlotNumber() >> mSkip) & 0x7;
    switch (alotted) {
      case 0:
        view->idle();
        break;
      case 1:
        view->listen();
        break;
      case 2:
        view->send(&msg, 3);
        break;
      case 3:
        view->send(&msg, 3, false);
        break;
      case 4:
        view->send(&msg, 6);
        break;
      case 5:
        view->send(&msg, 6, false);
        break;
      case 6:
        view->send(&msg, 9);
        break;
      case 7:
        view->send(&msg, 9, false);
        break;
    }
  }
};

// _____________________________________________________________________________
// Entry point.
ANLIMPL_MAIN(int argc, char** argv) {
  // Create the components.
  DoEverythingComponent comp1(0);
  DoEverythingComponent comp2(3);
  Component* comps[2];
  comps[0] = &comp1;
  comps[1] = &comp2;

  // Create the messages.
  Message* msgs[1];
  msgs[0] = &msg;

  // Create the simulation.
  ExplicitNetworkTopology ent;
  ent.addEdge(comps[0], comps[1]);
  Simulator sim(10);
  sim.useTopology(&ent);
  sim.useComponents(comps, 2);
  sim.useMessages(msgs, 1);

  // Run the simulation.
  sim.run(8*8);

  return 0;
}
