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

#include <gtest/gtest.h>
#include "anl/core/simulator.h"
#include "anl/core/topologies.h"
#include "anl/output/output.h"

// We do this here as a test can under no circumstance pollute the namespace as
// it is never included.
using namespace Core;  // NOLINT

// _____________________________________________________________________________
TEST(SimulatorDeathTest, invalidTopology) {
  // Scenario: using nullptr as topology fails.
  // Why: abnormal exit point of method.
  Simulator sim(20);
  ASSERT_DEATH(sim.useTopology(nullptr), "useTopology()");
  ASSERT_DEATH(sim.useTopology(nullptr), "Topology must not be 'nullptr'");
}

// _____________________________________________________________________________
TEST(SimulatorDeathTest, invalidOutputModule) {
  // Scenario: using nullptr as output module fails.
  // Why: abnormal exit point of method.
  Simulator sim(20);
  ASSERT_DEATH(sim.useOutputModule(nullptr), "useOutputModule()");
  ASSERT_DEATH(sim.useOutputModule(nullptr), "Output module must not be "
    "'nullptr'");
}

// _____________________________________________________________________________
TEST(SimulatorDeathTest, invalidCompArray) {
  // Scenario: using nullptr as component array fails.
  // Why: abnormal exit point of method.
  Simulator sim(20);
  ASSERT_DEATH(sim.useComponents(nullptr, 0), "useComponents()");
  ASSERT_DEATH(sim.useComponents(nullptr, 0), "Component pointer array must "
    "not be 'nullptr'");
  ASSERT_DEATH(sim.useComponents(nullptr, 1), "useComponents()");
  ASSERT_DEATH(sim.useComponents(nullptr, 1), "Component pointer array must "
    "not be 'nullptr'");
  ASSERT_DEATH(sim.useComponents(nullptr, 10), "useComponents()");
  ASSERT_DEATH(sim.useComponents(nullptr, 10), "Component pointer array must "
    "not be 'nullptr'");
}

// _____________________________________________________________________________
TEST(SimulatorDeathTest, invalidMsgArray) {
  // Scenario: using nullptr as message array fails.
  // Why: abnormal exit point of method.
  Simulator sim(20);
  ASSERT_DEATH(sim.useMessages(nullptr, 0), "useMessages()");
  ASSERT_DEATH(sim.useMessages(nullptr, 0), "Message pointer array must "
    "not be 'nullptr'");
  ASSERT_DEATH(sim.useMessages(nullptr, 1), "useMessages()");
  ASSERT_DEATH(sim.useMessages(nullptr, 1), "Message pointer array must "
    "not be 'nullptr'");
  ASSERT_DEATH(sim.useMessages(nullptr, 10), "useMessages()");
  ASSERT_DEATH(sim.useMessages(nullptr, 10), "Message pointer array must "
    "not be 'nullptr'");
}

// _____________________________________________________________________________
TEST(SimulatorDeathTest, invalidCompInArray) {
  // Scenario: component array containing a nullptr fails.
  // Why: abnormal exit point of method. choose second entry to ensure that not
  //  only the first entry is checked.
  Component* arr[2];
  Component c;
  arr[0] = &c;
  arr[1] = nullptr;

  Simulator sim(20);
  ASSERT_DEATH(sim.useComponents(arr, 2), "useComponents()");
  ASSERT_DEATH(sim.useComponents(arr, 2), "Stepping through component");
  ASSERT_DEATH(sim.useComponents(arr, 2), "Component pointer must not be "
    "'nullptr'");
}

// _____________________________________________________________________________
TEST(SimulatorDeathTest, invalidMsgInArray) {
  // Scenario: message array containing a nullptr fails.
  // Why: abnormal exit point of method. choose second entry to ensure that not
  //  only the first entry is checked.
  Message* arr[2];
  Message m;
  arr[0] = &m;
  arr[1] = nullptr;

  Simulator sim(20);
  ASSERT_DEATH(sim.useMessages(arr, 2), "useMessages()");
  ASSERT_DEATH(sim.useMessages(arr, 2), "Stepping through message");
  ASSERT_DEATH(sim.useMessages(arr, 2), "Message pointer must not be "
    "'nullptr'");
}

// _____________________________________________________________________________
TEST(SimulatorDeathTest, runNeedsDuration) {
  // Scenario: running with duration zero fails.
  // Why: abnormal exit point of method.
  Simulator sim(20);
  TrivialNetworkTopology tnt;
  sim.useTopology(&tnt);
  ASSERT_DEATH(sim.run(0), "run()");
  ASSERT_DEATH(sim.run(0), "Checking prerequisites");
  ASSERT_DEATH(sim.run(0), "Simulation duration must be greater");
}

// _____________________________________________________________________________
TEST(SimulatorDeathTest, runNeedsTopology) {
  // Scenario: running without topology fails.
  // Why: abnormal exit point of method (no longer, test kept).
  Simulator sim(20);
  ASSERT_DEATH(sim.run(5), "run()");
  ASSERT_DEATH(sim.run(5), "Checking prerequisites");
  ASSERT_DEATH(sim.run(5), "Network topology must be set");
}

// _____________________________________________________________________________
TEST(SimulatorDeathTest, runSingleNeedsTopology) {
  // Scenario: running in single step mode without topology fails.
  // Why: abnormal exit point of method.
  Simulator sim(20);
  ASSERT_DEATH(sim.runSingle(1), "runSingle()");
  ASSERT_DEATH(sim.runSingle(1), "Checking prerequisites");
  ASSERT_DEATH(sim.runSingle(1), "Network topology must be set");
}

// _____________________________________________________________________________
TEST(SimulatorDeathTest, runNeedsOutputModule) {
  // Scenario: running without output module fails.
  // Why: abnormal exit point of method (no longer, test kept).
  Simulator sim(20);
  TrivialNetworkTopology tnt;
  sim.useTopology(&tnt);
  ASSERT_DEATH(sim.run(5), "run()");
  ASSERT_DEATH(sim.run(5), "Checking prerequisites");
  ASSERT_DEATH(sim.run(5), "Output module must be set");
}

// _____________________________________________________________________________
TEST(SimulatorDeathTest, runSingleNeedsOutputModule) {
  // Scenario: running without output module in single step mode fails.
  // Why: abnormal exit point of method.
  Simulator sim(20);
  TrivialNetworkTopology tnt;
  sim.useTopology(&tnt);
  ASSERT_DEATH(sim.runSingle(1), "runSingle()");
  ASSERT_DEATH(sim.runSingle(1), "Checking prerequisites");
  ASSERT_DEATH(sim.runSingle(1), "Output module must be set");
}

// _____________________________________________________________________________
TEST(SimulatorTest, run) {
  // Scenario: we run a component whose protocol sets a boolean flag to true.
  //  the test then checks this flag.
  // Why: we want to ensure that the protocol is actually invoked.

  // Component type for this test.
  class TestComponent : public Component {
   public:
    // Constructor.
    explicit TestComponent(bool* out) : mOut(out) {}

   private:
    // The output variable.
    bool* mOut;

    // The protocol callback.
    void doAct(ANLView* view) override { *mOut = true; view->idle(); };
  };

  bool testVar = false;
  TrivialNetworkTopology tnt;
  Output::StdOutOutputModule out;
  TestComponent comp(&testVar);
  Component* comps[1];
  comps[0] = &comp;

  Simulator sim(20);
  sim.useOutputModule(&out);
  sim.useTopology(&tnt);
  sim.useComponents(comps, 1);

  ASSERT_FALSE(testVar);
  sim.run(2);
  ASSERT_TRUE(testVar);
}

// _____________________________________________________________________________
TEST(SimulatorTest, runSingle) {
  // Scenario: we run a component whose protocol sets a boolean flag to true.
  //  the test then checks this flag.
  // Why: we want to ensure that the protocol is actually invoked in single step
  //  mode.

  // Component type for this test.
  class TestComponent : public Component {
   public:
    // Constructor.
    explicit TestComponent(bool* out) : mOut(out) {}

   private:
    // The output variable.
    bool* mOut;

    // The protocol callback.
    void doAct(ANLView* view) override { *mOut = true; view->idle(); };
  };

  bool testVar = false;
  TrivialNetworkTopology tnt;
  Output::StdOutOutputModule out;
  TestComponent comp(&testVar);
  Component* comps[1];
  comps[0] = &comp;

  Simulator sim(20);
  sim.useOutputModule(&out);
  sim.useTopology(&tnt);
  sim.useComponents(comps, 1);

  ASSERT_FALSE(testVar);
  sim.runSingle(2);
  ASSERT_TRUE(testVar);
  sim.runSingle(2);
  ASSERT_TRUE(testVar);
  sim.endSingle();
  ASSERT_TRUE(testVar);
}
