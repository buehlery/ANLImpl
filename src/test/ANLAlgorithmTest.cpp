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
#include <cstddef>
#include "anl/core/anl.h"
#include "anl/core/anl_algorithm.h"
#include "anl/core/topologies.h"

// We do this here as a test can under no circumstance pollute the namespace as
// it is never included.
using namespace Core;  // NOLINT

// A network topology that represents a directed chain X -> Y -> Z.
class ExampleNetworkTopology final : public NetworkTopology {
 public:
  ExampleNetworkTopology(const Component* frst, const Component* scnd,
    const Component* thrd) : mFrst(frst), mScnd(scnd), mThrd(thrd) {}
 private:
  const Component* mFrst;
  const Component* mScnd;
  const Component* mThrd;
  bool doCanReach(const Component* sndr, const Component* rcvr) const override
    { return (sndr == mFrst && rcvr == mScnd)
        || (sndr == mScnd && rcvr == mThrd); };
};

// _____________________________________________________________________________
TEST(SenderSetComputerDeathTest, noSetup) {
  // Scenario: setup is nullptr fails.
  // Why: abnormal exit point of constructor.
  NetworkSetup setup(20);
  TrivialNetworkTopology topo;
  IntentionAssignment intent(&setup);

  ASSERT_DEATH(SenderSetComputer(nullptr, &topo, &intent), "setup is nullptr");
}

// _____________________________________________________________________________
TEST(SenderSetComputerDeathTest, noTopology) {
  // Scenario: topology is nullptr fails.
  // Why: abnormal exit point of constructor.
  NetworkSetup setup(20);
  IntentionAssignment intent(&setup);

  ASSERT_DEATH(SenderSetComputer(&setup, nullptr, &intent),
    "topology is nullptr");
}

// _____________________________________________________________________________
TEST(SenderSetComputerDeathTest, noIntent) {
  // Scenario: intent is nullptr fails.
  // Why: abnormal exit point of constructor.
  NetworkSetup setup(20);
  TrivialNetworkTopology tnt;

  ASSERT_DEATH(SenderSetComputer(&setup, &tnt, nullptr), "intent is nullptr");
}

// _____________________________________________________________________________
TEST(SenderSetComputerDeathTest, partialIntent) {
  // Scenario: partial intent fails.
  // Why: abnormal exit point of constructor.
  NetworkSetup setup(20);
  TrivialNetworkTopology tnt;
  IntentionAssignment intent(&setup);

  ASSERT_DEATH(SenderSetComputer(&setup, &tnt, &intent), "intent is partial");
}

// _____________________________________________________________________________
TEST(SenderSetComputerTest, noSender) {
  // Scenario: no sending component.
  // Why: common scenario when no component intends to send a message.
  NetworkSetup setup(20);
  TrivialNetworkTopology topo;
  Component comp;
  setup.registerComponent(&comp);

  IntentionAssignment intent(&setup);
  intent.setTraitFor(&comp, ComponentIntention(setup, IntentionType::IDLE, 0,
    nullptr));

  SenderSetComputer ssc(&setup, &topo, &intent);
  SenderSetRepresentation ssr = ssc.getSenderSet();

  // The component is not sending.
  ASSERT_EQ(ActionType::IDLE, ssr.getTraitFor(&comp).getType());
}

// _____________________________________________________________________________
TEST(SenderSetComputerDeathTest, sendInvalidMessageFails) {
  // Scenario: a nullptr as the message fails.
  // Why: abnormal exit point from used method.
  NetworkSetup setup(20);
  TrivialNetworkTopology topo;
  Component c1, c2;
  setup.registerComponent(&c1);
  setup.registerComponent(&c2);
  IntentionAssignment intent(&setup);

  intent.setTraitFor(&c1, ComponentIntention(setup, IntentionType::IDLE, 0,
    nullptr));
  intent.setTraitFor(&c2, ComponentIntention(setup, IntentionType::SEND, 0,
    nullptr));

  SenderSetComputer ssc(&setup, &topo, &intent);

  ASSERT_DEATH(ssc.getSenderSet(), "invalid message");
}

// _____________________________________________________________________________
TEST(SenderSetComputerTest, singleSenderTic0) {
  // Scenario: exactly one component sending. the component sends in tic 0.
  // Why: tic 0 is a corner case. one component sending is common.
  NetworkSetup setup(20);
  TrivialNetworkTopology topo;
  Component c1, c2;
  Message msg;
  setup.registerMessage(&msg);
  setup.registerComponent(&c1);
  setup.registerComponent(&c2);
  IntentionAssignment intent(&setup);

  intent.setTraitFor(&c1, ComponentIntention(setup, IntentionType::IDLE, 0,
    nullptr));
  intent.setTraitFor(&c2, ComponentIntention(setup, IntentionType::SEND, 0,
    &msg));

  SenderSetComputer ssc(&setup, &topo, &intent);
  SenderSetRepresentation ssr = ssc.getSenderSet();

  // Component c2 is sending, component c1 is not sending.
  ASSERT_EQ(ActionType::IDLE, ssr.getTraitFor(&c1).getType());
  ASSERT_EQ(ActionType::SENT, ssr.getTraitFor(&c2).getType());
  ASSERT_EQ(0, ssr.getTraitFor(&c2).getTic());
  ASSERT_EQ(&msg, ssr.getTraitFor(&c2).getMessage());
}

// _____________________________________________________________________________
TEST(SenderSetComputerTest, singleSenderTic5) {
  // Scenario: exactly one component sending. the component sends in tic 5.
  // Why: tic 5 is not a corner case. one component sending is common.
  NetworkSetup setup(20);
  TrivialNetworkTopology topo;
  Component c1, c2;
  Message msg;
  setup.registerMessage(&msg);
  setup.registerComponent(&c1);
  setup.registerComponent(&c2);
  IntentionAssignment intent(&setup);

  intent.setTraitFor(&c1, ComponentIntention(setup, IntentionType::IDLE, 0,
    nullptr));
  intent.setTraitFor(&c2, ComponentIntention(setup, IntentionType::SEND, 5,
    &msg));

  SenderSetComputer ssc(&setup, &topo, &intent);
  SenderSetRepresentation ssr = ssc.getSenderSet();

  // Component c2 is sending, component c1 is not sending.
  ASSERT_EQ(ActionType::IDLE, ssr.getTraitFor(&c1).getType());
  ASSERT_EQ(ActionType::SENT, ssr.getTraitFor(&c2).getType());
  ASSERT_EQ(5, ssr.getTraitFor(&c2).getTic());
  ASSERT_EQ(&msg, ssr.getTraitFor(&c2).getMessage());
}

// _____________________________________________________________________________
TEST(SenderSetComputerTest, twoSendersTic0) {
  // Scenario: two components intend to send, both in slot 0.
  // Why: corner case of carrier sensing (two at the same time). slot 0 is a
  //  corner case.
  NetworkSetup setup(20);
  TrivialNetworkTopology topo;
  Component c1, c2;
  Message msg;
  setup.registerMessage(&msg);
  setup.registerComponent(&c1);
  setup.registerComponent(&c2);
  IntentionAssignment intent(&setup);

  intent.setTraitFor(&c1, ComponentIntention(setup, IntentionType::SEND, 0,
    &msg));
  intent.setTraitFor(&c2, ComponentIntention(setup, IntentionType::SEND, 0,
    &msg));

  SenderSetComputer ssc(&setup, &topo, &intent);
  SenderSetRepresentation ssr = ssc.getSenderSet();

  // Both components are sending.
  ASSERT_EQ(ActionType::SENT, ssr.getTraitFor(&c1).getType());
  ASSERT_EQ(0, ssr.getTraitFor(&c1).getTic());
  ASSERT_EQ(&msg, ssr.getTraitFor(&c1).getMessage());
  ASSERT_EQ(ActionType::SENT, ssr.getTraitFor(&c2).getType());
  ASSERT_EQ(0, ssr.getTraitFor(&c2).getTic());
  ASSERT_EQ(&msg, ssr.getTraitFor(&c2).getMessage());
}

// _____________________________________________________________________________
TEST(SenderSetComputerTest, twoSendersTic7) {
  // Scenario: two components intend to send, both in slot 7.
  // Why: corner case of carrier sensing (two at the same time). slot 7 is not a
  //  corner case.
  NetworkSetup setup(20);
  TrivialNetworkTopology topo;
  Component c1, c2;
  Message msg;
  setup.registerMessage(&msg);
  setup.registerComponent(&c1);
  setup.registerComponent(&c2);
  IntentionAssignment intent(&setup);

  intent.setTraitFor(&c1, ComponentIntention(setup, IntentionType::SEND, 7,
    &msg));
  intent.setTraitFor(&c2, ComponentIntention(setup, IntentionType::SEND, 7,
    &msg));

  SenderSetComputer ssc(&setup, &topo, &intent);
  SenderSetRepresentation ssr = ssc.getSenderSet();

  // Both components are sending.
  ASSERT_EQ(ActionType::SENT, ssr.getTraitFor(&c1).getType());
  ASSERT_EQ(7, ssr.getTraitFor(&c1).getTic());
  ASSERT_EQ(&msg, ssr.getTraitFor(&c1).getMessage());
  ASSERT_EQ(ActionType::SENT, ssr.getTraitFor(&c2).getType());
  ASSERT_EQ(7, ssr.getTraitFor(&c2).getTic());
  ASSERT_EQ(&msg, ssr.getTraitFor(&c2).getMessage());
}

// _____________________________________________________________________________
TEST(SenderSetComputerTest, twoSendersTic04Unreachable) {
  // Scenario: two components intend to send (tics 0, 4) in a network topology
  //  that isolates components.
  // Why: corner case for topologies. two components sending is common.
  NetworkSetup setup(20);
  IsolatedNetworkTopology topo;
  Component c1, c2;
  Message msg;
  setup.registerMessage(&msg);
  setup.registerComponent(&c1);
  setup.registerComponent(&c2);
  IntentionAssignment intent(&setup);

  intent.setTraitFor(&c1, ComponentIntention(setup, IntentionType::SEND, 0,
    &msg));
  intent.setTraitFor(&c2, ComponentIntention(setup, IntentionType::SEND, 4,
    &msg));

  SenderSetComputer ssc(&setup, &topo, &intent);
  SenderSetRepresentation ssr = ssc.getSenderSet();

  // Both components are sending.
  ASSERT_EQ(ActionType::SENT, ssr.getTraitFor(&c1).getType());
  ASSERT_EQ(0, ssr.getTraitFor(&c1).getTic());
  ASSERT_EQ(&msg, ssr.getTraitFor(&c1).getMessage());
  ASSERT_EQ(ActionType::SENT, ssr.getTraitFor(&c2).getType());
  ASSERT_EQ(4, ssr.getTraitFor(&c2).getTic());
  ASSERT_EQ(&msg, ssr.getTraitFor(&c2).getMessage());
}

// _____________________________________________________________________________
TEST(SenderSetComputerTest, twoSendersTic15LaterForce) {
  // Scenario: two components intend to send (tics 1, 5). the latter component
  //  does not use carrier sensing.
  // Why: test 1/4 for checking sending with & without carrier sensing.
  NetworkSetup setup(20);
  TrivialNetworkTopology topo;
  Component c1, c2;
  Message msg;
  setup.registerMessage(&msg);
  setup.registerComponent(&c1);
  setup.registerComponent(&c2);
  IntentionAssignment intent(&setup);

  intent.setTraitFor(&c1, ComponentIntention(setup, IntentionType::SEND, 1,
    &msg));
  intent.setTraitFor(&c2, ComponentIntention(setup, IntentionType::SEND_FORCE,
    5, &msg));

  SenderSetComputer ssc(&setup, &topo, &intent);
  SenderSetRepresentation ssr = ssc.getSenderSet();

  // Both components are sending.
  ASSERT_EQ(ActionType::SENT, ssr.getTraitFor(&c1).getType());
  ASSERT_EQ(1, ssr.getTraitFor(&c1).getTic());
  ASSERT_EQ(&msg, ssr.getTraitFor(&c1).getMessage());
  ASSERT_EQ(ActionType::SENT, ssr.getTraitFor(&c2).getType());
  ASSERT_EQ(5, ssr.getTraitFor(&c2).getTic());
  ASSERT_EQ(&msg, ssr.getTraitFor(&c2).getMessage());
}

// _____________________________________________________________________________
TEST(SenderSetComputerTest, twoSendersTic26BothForce) {
  // Scenario: two components intend to send (tics 2, 6). both components do not
  //  use carrier sensing.
  // Why: test 2/4 for checking sending with & without carrier sensing.
  NetworkSetup setup(20);
  TrivialNetworkTopology topo;
  Component c1, c2;
  Message msg;
  setup.registerMessage(&msg);
  setup.registerComponent(&c1);
  setup.registerComponent(&c2);
  IntentionAssignment intent(&setup);

  intent.setTraitFor(&c1, ComponentIntention(setup, IntentionType::SEND_FORCE,
    2, &msg));
  intent.setTraitFor(&c2, ComponentIntention(setup, IntentionType::SEND_FORCE,
    6, &msg));

  SenderSetComputer ssc(&setup, &topo, &intent);
  SenderSetRepresentation ssr = ssc.getSenderSet();

  // Both components are sending.
  ASSERT_EQ(ActionType::SENT, ssr.getTraitFor(&c1).getType());
  ASSERT_EQ(2, ssr.getTraitFor(&c1).getTic());
  ASSERT_EQ(&msg, ssr.getTraitFor(&c1).getMessage());
  ASSERT_EQ(ActionType::SENT, ssr.getTraitFor(&c2).getType());
  ASSERT_EQ(6, ssr.getTraitFor(&c2).getTic());
  ASSERT_EQ(&msg, ssr.getTraitFor(&c2).getMessage());
}

// _____________________________________________________________________________
TEST(SenderSetComputerTest, twoSendersTic37CarrierSensing) {
  // Scenario: two components intend to send (tics 3, 7). both components do use
  //  carrier sensing.
  // Why: test 3/4 for checking sending with & without carrier sensing.
  NetworkSetup setup(20);
  TrivialNetworkTopology topo;
  Component c1, c2;
  Message msg;
  setup.registerMessage(&msg);
  setup.registerComponent(&c1);
  setup.registerComponent(&c2);
  IntentionAssignment intent(&setup);

  intent.setTraitFor(&c1, ComponentIntention(setup, IntentionType::SEND, 3,
    &msg));
  intent.setTraitFor(&c2, ComponentIntention(setup, IntentionType::SEND, 7,
    &msg));

  SenderSetComputer ssc(&setup, &topo, &intent);
  SenderSetRepresentation ssr = ssc.getSenderSet();

  // Only the earlier (c1) component is sending.
  ASSERT_EQ(ActionType::SENT, ssr.getTraitFor(&c1).getType());
  ASSERT_EQ(3, ssr.getTraitFor(&c1).getTic());
  ASSERT_EQ(&msg, ssr.getTraitFor(&c1).getMessage());
  ASSERT_EQ(ActionType::IDLE, ssr.getTraitFor(&c2).getType());
}

// _____________________________________________________________________________
TEST(SenderSetComputerTest, twoSendersTic48EarlyForce) {
  // Scenario: two components intend to send (tics 4, 8). the earlier component
  //  does not use carrier sensing.
  // Why: test 4/4 for checking sending with & without carrier sensing.
  NetworkSetup setup(20);
  TrivialNetworkTopology topo;
  Component c1, c2;
  Message msg;
  setup.registerMessage(&msg);
  setup.registerComponent(&c1);
  setup.registerComponent(&c2);
  IntentionAssignment intent(&setup);

  intent.setTraitFor(&c1, ComponentIntention(setup, IntentionType::SEND_FORCE,
    4, &msg));
  intent.setTraitFor(&c2, ComponentIntention(setup, IntentionType::SEND, 8,
    &msg));

  SenderSetComputer ssc(&setup, &topo, &intent);
  SenderSetRepresentation ssr = ssc.getSenderSet();

  // Only the earlier (c1) component is sending.
  ASSERT_EQ(ActionType::SENT, ssr.getTraitFor(&c1).getType());
  ASSERT_EQ(4, ssr.getTraitFor(&c1).getTic());
  ASSERT_EQ(&msg, ssr.getTraitFor(&c1).getMessage());
  ASSERT_EQ(ActionType::IDLE, ssr.getTraitFor(&c2).getType());
}

// _____________________________________________________________________________
TEST(SenderSetComputerTest, threeSendersMotivationalExample) {
  // Scenario: the motivational example from the report in the subsection for
  //  sender sets.
  // Why: motivational example from the report that made sender sets necessary
  //  in the first place.
  NetworkSetup setup(20);
  Component c1, c2, c3;
  ExampleNetworkTopology topo(&c1, &c2, &c3);
  Message msg;
  setup.registerMessage(&msg);
  setup.registerComponent(&c1);
  setup.registerComponent(&c2);
  setup.registerComponent(&c3);
  IntentionAssignment intent(&setup);

  intent.setTraitFor(&c1, ComponentIntention(setup, IntentionType::SEND, 1,
    &msg));
  intent.setTraitFor(&c2, ComponentIntention(setup, IntentionType::SEND, 2,
    &msg));
  intent.setTraitFor(&c3, ComponentIntention(setup, IntentionType::SEND, 3,
    &msg));

  SenderSetComputer ssc(&setup, &topo, &intent);
  SenderSetRepresentation ssr = ssc.getSenderSet();

  // c1 -> c2 -> c3: c1 and c3 are sending. c2 is not sending.
  ASSERT_EQ(ActionType::SENT, ssr.getTraitFor(&c1).getType());
  ASSERT_EQ(1, ssr.getTraitFor(&c1).getTic());
  ASSERT_EQ(&msg, ssr.getTraitFor(&c1).getMessage());
  ASSERT_EQ(ActionType::IDLE, ssr.getTraitFor(&c2).getType());
  ASSERT_EQ(ActionType::SENT, ssr.getTraitFor(&c3).getType());
  ASSERT_EQ(3, ssr.getTraitFor(&c3).getTic());
  ASSERT_EQ(&msg, ssr.getTraitFor(&c3).getMessage());
}

// _____________________________________________________________________________
TEST(ANLComputerDeathTest, tooRestrictiveFilterFails) {
  // Scenario: a filter function (semantics subset) that blocks everything is
  //  provided and fails.
  // Why: filter functions must retain at least one element.
  NetworkSetup setup(20);
  TrivialNetworkTopology tnt;
  Component comp;
  setup.registerComponent(&comp);

  IntentionAssignment intent(&setup);
  intent.setTraitFor(&comp, ComponentIntention(setup, IntentionType::IDLE, 0,
    nullptr));

  ANLComputer ac(&setup, &tnt, &intent,
    [](const NetworkSetup& setup, std::vector<ComponentAction>* inout) {
      inout->clear();
  });
  ASSERT_DEATH(ac.transition(), "filter removed all possibilities");
}

// _____________________________________________________________________________
TEST(ANLComputerTest, idleLeadsToIdle) {
  // Scenario: an ANL transition makes a component idle that intends to idle.
  // Why: IDL is a component intention with only one possible outcome which we
  //  test here.
  NetworkSetup setup(20);
  TrivialNetworkTopology tnt;
  Component comp;
  setup.registerComponent(&comp);

  IntentionAssignment intent(&setup);
  intent.setTraitFor(&comp, ComponentIntention(setup, IntentionType::IDLE, 0,
    nullptr));

  ANLComputer acCanon(&setup, &tnt, &intent, ANLFilterNothing);
  std::vector<NetworkState> resultCanon = acCanon.transition();

  // Component is idling in canonical semantics.
  ASSERT_EQ(1, resultCanon.size());
  ASSERT_EQ(ActionType::IDLE, resultCanon[0].getTraitFor(&comp).getType());
  ASSERT_EQ(0, resultCanon[0].getTraitFor(&comp).getTic());
  ASSERT_EQ(nullptr, resultCanon[0].getTraitFor(&comp).getMessage());

  ANLComputer acNaive(&setup, &tnt, &intent, ANLFilterNaive);
  std::vector<NetworkState> resultNaive = acNaive.transition();

  // Component is idling in naive semantics.
  ASSERT_EQ(1, resultNaive.size());
  ASSERT_EQ(ActionType::IDLE, resultNaive[0].getTraitFor(&comp).getType());
  ASSERT_EQ(0, resultNaive[0].getTraitFor(&comp).getTic());
  ASSERT_EQ(nullptr, resultNaive[0].getTraitFor(&comp).getMessage());
}

// _____________________________________________________________________________
TEST(ANLComputerTest, successfulSendLeadsToSent) {
  // Scenario: an ANL transition makes a component send a message if the
  //  component intends to send and is in the sender set.
  // Why: sending is an intention that can have two possible outcomes, of which
  //  this is the first.
  NetworkSetup setup(20);
  TrivialNetworkTopology tnt;
  Component comp;
  setup.registerComponent(&comp);
  Message msg;
  setup.registerMessage(&msg);

  IntentionAssignment intent(&setup);
  intent.setTraitFor(&comp, ComponentIntention(setup, IntentionType::SEND, 4,
    &msg));

  ANLComputer acCanon(&setup, &tnt, &intent, ANLFilterNothing);
  std::vector<NetworkState> resultCanon = acCanon.transition();

  // Component is sending in canonical semantics.
  ASSERT_EQ(1, resultCanon.size());
  ASSERT_EQ(ActionType::SENT, resultCanon[0].getTraitFor(&comp).getType());
  ASSERT_EQ(4, resultCanon[0].getTraitFor(&comp).getTic());
  ASSERT_EQ(&msg, resultCanon[0].getTraitFor(&comp).getMessage());

  ANLComputer acNaive(&setup, &tnt, &intent, ANLFilterNaive);
  std::vector<NetworkState> resultNaive = acNaive.transition();

  // Component is sending in naive semantics.
  ASSERT_EQ(1, resultNaive.size());
  ASSERT_EQ(ActionType::SENT, resultNaive[0].getTraitFor(&comp).getType());
  ASSERT_EQ(4, resultNaive[0].getTraitFor(&comp).getTic());
  ASSERT_EQ(&msg, resultNaive[0].getTraitFor(&comp).getMessage());
}

// _____________________________________________________________________________
TEST(ANLComputerTest, successfulSendForceLeadsToSent) {
  // Scenario: an ANL transition makes a component send a message if the
  //  component intends to send and is not using carrier sensing.
  NetworkSetup setup(20);
  TrivialNetworkTopology tnt;
  Component comp;
  setup.registerComponent(&comp);
  Message msg;
  setup.registerMessage(&msg);

  IntentionAssignment intent(&setup);
  intent.setTraitFor(&comp, ComponentIntention(setup, IntentionType::SEND_FORCE,
    4, &msg));

  ANLComputer acCanon(&setup, &tnt, &intent, ANLFilterNothing);
  std::vector<NetworkState> resultCanon = acCanon.transition();

  // Component is sending in canonical semantics.
  ASSERT_EQ(1, resultCanon.size());
  ASSERT_EQ(ActionType::SENT, resultCanon[0].getTraitFor(&comp).getType());
  ASSERT_EQ(4, resultCanon[0].getTraitFor(&comp).getTic());
  ASSERT_EQ(&msg, resultCanon[0].getTraitFor(&comp).getMessage());

  ANLComputer acNaive(&setup, &tnt, &intent, ANLFilterNaive);
  std::vector<NetworkState> resultNaive = acNaive.transition();

  // Component is sending in naive semantics.
  ASSERT_EQ(1, resultNaive.size());
  ASSERT_EQ(ActionType::SENT, resultNaive[0].getTraitFor(&comp).getType());
  ASSERT_EQ(4, resultNaive[0].getTraitFor(&comp).getTic());
  ASSERT_EQ(&msg, resultNaive[0].getTraitFor(&comp).getMessage());
}

// _____________________________________________________________________________
TEST(ANLComputerTest, carrierSense00LeadsToTwoAttempts) {
  // Scenario: both attempting to send without carrier sensing, tics 4, 5.
  // Why: test 1/4 of with/without carrier sensing interactions.
  NetworkSetup setup(20);
  TrivialNetworkTopology tnt;
  Component comp1;
  Component comp2;
  setup.registerComponent(&comp1);
  setup.registerComponent(&comp2);
  Message msg1;
  Message msg2;
  setup.registerMessage(&msg1);
  setup.registerMessage(&msg2);

  IntentionAssignment intent(&setup);
  intent.setTraitFor(&comp1, ComponentIntention(setup,
    IntentionType::SEND_FORCE, 4, &msg1));
  intent.setTraitFor(&comp2, ComponentIntention(setup,
    IntentionType::SEND_FORCE, 5, &msg2));

  ANLComputer acCanon(&setup, &tnt, &intent, ANLFilterNothing);
  std::vector<NetworkState> resultCanon = acCanon.transition();

  // Both components are sending in canonical semantics.
  ASSERT_EQ(1, resultCanon.size());
  ASSERT_EQ(ActionType::SENT, resultCanon[0].getTraitFor(&comp1).getType());
  ASSERT_EQ(ActionType::SENT, resultCanon[0].getTraitFor(&comp2).getType());
  ASSERT_EQ(4, resultCanon[0].getTraitFor(&comp1).getTic());
  ASSERT_EQ(5, resultCanon[0].getTraitFor(&comp2).getTic());
  ASSERT_EQ(&msg1, resultCanon[0].getTraitFor(&comp1).getMessage());
  ASSERT_EQ(&msg2, resultCanon[0].getTraitFor(&comp2).getMessage());

  ANLComputer acNaive(&setup, &tnt, &intent, ANLFilterNaive);
  std::vector<NetworkState> resultNaive = acNaive.transition();

  // Both components are sending in naive semantics.
  ASSERT_EQ(1, resultNaive.size());
  ASSERT_EQ(ActionType::SENT, resultNaive[0].getTraitFor(&comp1).getType());
  ASSERT_EQ(ActionType::SENT, resultNaive[0].getTraitFor(&comp2).getType());
  ASSERT_EQ(4, resultNaive[0].getTraitFor(&comp1).getTic());
  ASSERT_EQ(5, resultNaive[0].getTraitFor(&comp2).getTic());
  ASSERT_EQ(&msg1, resultNaive[0].getTraitFor(&comp1).getMessage());
  ASSERT_EQ(&msg2, resultNaive[0].getTraitFor(&comp2).getMessage());
}

// _____________________________________________________________________________
TEST(ANLComputerTest, carrierSense10LeadsToTwoAttempts) {
  // Scenario: both attempting to send, second one without carrier sensing.
  // Why: test 2/4 of with/without carrier sensing interactions.
  NetworkSetup setup(20);
  TrivialNetworkTopology tnt;
  Component comp1;
  Component comp2;
  setup.registerComponent(&comp1);
  setup.registerComponent(&comp2);
  Message msg1;
  Message msg2;
  setup.registerMessage(&msg1);
  setup.registerMessage(&msg2);

  IntentionAssignment intent(&setup);
  intent.setTraitFor(&comp1, ComponentIntention(setup, IntentionType::SEND, 4,
    &msg1));
  intent.setTraitFor(&comp2, ComponentIntention(setup,
    IntentionType::SEND_FORCE, 5, &msg2));

  ANLComputer acCanon(&setup, &tnt, &intent, ANLFilterNothing);
  std::vector<NetworkState> resultCanon = acCanon.transition();

  // Both components are sending in canonical semantics.
  ASSERT_EQ(1, resultCanon.size());
  ASSERT_EQ(ActionType::SENT, resultCanon[0].getTraitFor(&comp1).getType());
  ASSERT_EQ(ActionType::SENT, resultCanon[0].getTraitFor(&comp2).getType());
  ASSERT_EQ(4, resultCanon[0].getTraitFor(&comp1).getTic());
  ASSERT_EQ(5, resultCanon[0].getTraitFor(&comp2).getTic());
  ASSERT_EQ(&msg1, resultCanon[0].getTraitFor(&comp1).getMessage());
  ASSERT_EQ(&msg2, resultCanon[0].getTraitFor(&comp2).getMessage());

  ANLComputer acNaive(&setup, &tnt, &intent, ANLFilterNaive);
  std::vector<NetworkState> resultNaive = acNaive.transition();

  // Both components are sending in naive semantics.
  ASSERT_EQ(1, resultNaive.size());
  ASSERT_EQ(ActionType::SENT, resultNaive[0].getTraitFor(&comp1).getType());
  ASSERT_EQ(ActionType::SENT, resultNaive[0].getTraitFor(&comp2).getType());
  ASSERT_EQ(4, resultNaive[0].getTraitFor(&comp1).getTic());
  ASSERT_EQ(5, resultNaive[0].getTraitFor(&comp2).getTic());
  ASSERT_EQ(&msg1, resultNaive[0].getTraitFor(&comp1).getMessage());
  ASSERT_EQ(&msg2, resultNaive[0].getTraitFor(&comp2).getMessage());
}

// _____________________________________________________________________________
TEST(ANLComputerTest, carrierSense01LeadsToOneAttempt) {
  // Scenario: both attempting to send, first one without carrier sensing.
  // Why: test 3/4 of with/without carrier sensing interactions.
  NetworkSetup setup(20);
  TrivialNetworkTopology tnt;
  Component comp1;
  Component comp2;
  setup.registerComponent(&comp1);
  setup.registerComponent(&comp2);
  Message msg1;
  Message msg2;
  setup.registerMessage(&msg1);
  setup.registerMessage(&msg2);

  IntentionAssignment intent(&setup);
  intent.setTraitFor(&comp1, ComponentIntention(setup,
    IntentionType::SEND_FORCE, 4, &msg1));
  intent.setTraitFor(&comp2, ComponentIntention(setup, IntentionType::SEND, 5,
    &msg2));

  ANLComputer acCanon(&setup, &tnt, &intent, ANLFilterNothing);
  std::vector<NetworkState> resultCanon = acCanon.transition();

  // Only the first component is sending in canonical semantics.
  ASSERT_EQ(1, resultCanon.size());
  ASSERT_EQ(ActionType::SENT, resultCanon[0].getTraitFor(&comp1).getType());
  ASSERT_EQ(ActionType::CANCELLED,
    resultCanon[0].getTraitFor(&comp2).getType());
  ASSERT_EQ(4, resultCanon[0].getTraitFor(&comp1).getTic());
  ASSERT_EQ(5, resultCanon[0].getTraitFor(&comp2).getTic());
  ASSERT_EQ(&msg1, resultCanon[0].getTraitFor(&comp1).getMessage());
  ASSERT_EQ(&msg2, resultCanon[0].getTraitFor(&comp2).getMessage());

  ANLComputer acNaive(&setup, &tnt, &intent, ANLFilterNaive);
  std::vector<NetworkState> resultNaive = acNaive.transition();

  // Only the first component is sending in naive semantics.
  ASSERT_EQ(1, resultNaive.size());
  ASSERT_EQ(ActionType::SENT, resultNaive[0].getTraitFor(&comp1).getType());
  ASSERT_EQ(ActionType::CANCELLED,
    resultNaive[0].getTraitFor(&comp2).getType());
  ASSERT_EQ(4, resultNaive[0].getTraitFor(&comp1).getTic());
  ASSERT_EQ(5, resultNaive[0].getTraitFor(&comp2).getTic());
  ASSERT_EQ(&msg1, resultNaive[0].getTraitFor(&comp1).getMessage());
  ASSERT_EQ(&msg2, resultNaive[0].getTraitFor(&comp2).getMessage());
}

// _____________________________________________________________________________
TEST(ANLComputerTest, carrierSense11LeadsToOneAttempt) {
  // Scenario: both attempting to send, both with carrier sensing.
  // Why: test 4/4 of with/without carrier sensing interactions.
  NetworkSetup setup(20);
  TrivialNetworkTopology tnt;
  Component comp1;
  Component comp2;
  setup.registerComponent(&comp1);
  setup.registerComponent(&comp2);
  Message msg1;
  Message msg2;
  setup.registerMessage(&msg1);
  setup.registerMessage(&msg2);

  IntentionAssignment intent(&setup);
  intent.setTraitFor(&comp1, ComponentIntention(setup, IntentionType::SEND, 4,
    &msg1));
  intent.setTraitFor(&comp2, ComponentIntention(setup, IntentionType::SEND, 5,
    &msg2));

  ANLComputer acCanon(&setup, &tnt, &intent, ANLFilterNothing);
  std::vector<NetworkState> resultCanon = acCanon.transition();

  // Only the first component is sending in canonical semantics.
  ASSERT_EQ(1, resultCanon.size());
  ASSERT_EQ(ActionType::SENT, resultCanon[0].getTraitFor(&comp1).getType());
  ASSERT_EQ(ActionType::CANCELLED,
    resultCanon[0].getTraitFor(&comp2).getType());
  ASSERT_EQ(4, resultCanon[0].getTraitFor(&comp1).getTic());
  ASSERT_EQ(5, resultCanon[0].getTraitFor(&comp2).getTic());
  ASSERT_EQ(&msg1, resultCanon[0].getTraitFor(&comp1).getMessage());
  ASSERT_EQ(&msg2, resultCanon[0].getTraitFor(&comp2).getMessage());

  ANLComputer acNaive(&setup, &tnt, &intent, ANLFilterNaive);
  std::vector<NetworkState> resultNaive = acNaive.transition();

  // Only the first component is sending in naive semantics.
  ASSERT_EQ(1, resultNaive.size());
  ASSERT_EQ(ActionType::SENT, resultNaive[0].getTraitFor(&comp1).getType());
  ASSERT_EQ(ActionType::CANCELLED,
    resultNaive[0].getTraitFor(&comp2).getType());
  ASSERT_EQ(4, resultNaive[0].getTraitFor(&comp1).getTic());
  ASSERT_EQ(5, resultNaive[0].getTraitFor(&comp2).getTic());
  ASSERT_EQ(&msg1, resultNaive[0].getTraitFor(&comp1).getMessage());
  ASSERT_EQ(&msg2, resultNaive[0].getTraitFor(&comp2).getMessage());
}

// _____________________________________________________________________________
TEST(ANLComputerTest, carrierSense11NoDetectSimultaneous) {
  // Scenario: both attempting to send in the same tic, both with carrier
  //  sensing.
  // Why: corner case of carrier sensing.
  NetworkSetup setup(20);
  TrivialNetworkTopology tnt;
  Component comp1;
  Component comp2;
  setup.registerComponent(&comp1);
  setup.registerComponent(&comp2);
  Message msg1;
  Message msg2;
  setup.registerMessage(&msg1);
  setup.registerMessage(&msg2);

  IntentionAssignment intent(&setup);
  intent.setTraitFor(&comp1, ComponentIntention(setup, IntentionType::SEND, 4,
    &msg1));
  intent.setTraitFor(&comp2, ComponentIntention(setup, IntentionType::SEND, 4,
    &msg2));

  ANLComputer acCanon(&setup, &tnt, &intent, ANLFilterNothing);
  std::vector<NetworkState> resultCanon = acCanon.transition();

  // Both components send in canonical semantics.
  ASSERT_EQ(1, resultCanon.size());
  ASSERT_EQ(ActionType::SENT, resultCanon[0].getTraitFor(&comp1).getType());
  ASSERT_EQ(ActionType::SENT, resultCanon[0].getTraitFor(&comp2).getType());
  ASSERT_EQ(4, resultCanon[0].getTraitFor(&comp1).getTic());
  ASSERT_EQ(4, resultCanon[0].getTraitFor(&comp2).getTic());
  ASSERT_EQ(&msg1, resultCanon[0].getTraitFor(&comp1).getMessage());
  ASSERT_EQ(&msg2, resultCanon[0].getTraitFor(&comp2).getMessage());

  ANLComputer acNaive(&setup, &tnt, &intent, ANLFilterNaive);
  std::vector<NetworkState> resultNaive = acNaive.transition();

  // Both components send in naive semantics.
  ASSERT_EQ(1, resultNaive.size());
  ASSERT_EQ(ActionType::SENT, resultNaive[0].getTraitFor(&comp1).getType());
  ASSERT_EQ(ActionType::SENT, resultNaive[0].getTraitFor(&comp2).getType());
  ASSERT_EQ(4, resultNaive[0].getTraitFor(&comp1).getTic());
  ASSERT_EQ(4, resultNaive[0].getTraitFor(&comp2).getTic());
  ASSERT_EQ(&msg1, resultNaive[0].getTraitFor(&comp1).getMessage());
  ASSERT_EQ(&msg2, resultNaive[0].getTraitFor(&comp2).getMessage());
}

// _____________________________________________________________________________
TEST(ANLComputerTest, listenToNobody) {
  // Scenario: component is listening while noone is sending.
  // Why: this may only result in collision or silence, in the naive semantics
  //  it must result in silence.
  NetworkSetup setup(20);
  TrivialNetworkTopology tnt;
  Component comp;
  setup.registerComponent(&comp);

  IntentionAssignment intent(&setup);
  intent.setTraitFor(&comp, ComponentIntention(setup, IntentionType::LISTEN, 0,
    nullptr));

  ANLComputer acCanon(&setup, &tnt, &intent, ANLFilterNothing);
  std::vector<NetworkState> resultCanon = acCanon.transition();
  ASSERT_EQ(1, resultCanon.size());

  // Ensure both silence and collision are possible in the canonical semantics.
  ASSERT_EQ(ActionType::SILENCE, resultCanon[0].getTraitFor(&comp).getType());
  ASSERT_EQ(0, resultCanon[0].getTraitFor(&comp).getTic());
  ASSERT_EQ(nullptr, resultCanon[0].getTraitFor(&comp).getMessage());

  ANLComputer acNaive(&setup, &tnt, &intent, ANLFilterNaive);
  std::vector<NetworkState> resultNaive = acNaive.transition();

  // Ensure only silence is possible in the naive semantics.
  ASSERT_EQ(1, resultNaive.size());
  ASSERT_EQ(ActionType::SILENCE, resultNaive[0].getTraitFor(&comp).getType());
  ASSERT_EQ(0, resultNaive[0].getTraitFor(&comp).getTic());
  ASSERT_EQ(nullptr, resultNaive[0].getTraitFor(&comp).getMessage());
}

// _____________________________________________________________________________
TEST(ANLComputerTest, listenToOne) {
  // Scenario: component is listening while one component is sending.
  // Why: in the naive semantics, this is successful. in the canonical semantics
  //  it can lead to a collision, silence, or a success.
  NetworkSetup setup(20);
  TrivialNetworkTopology tnt;
  Component comp1;
  Component comp2;
  setup.registerComponent(&comp1);
  setup.registerComponent(&comp2);
  Message msg;
  setup.registerMessage(&msg);

  IntentionAssignment intent(&setup);
  intent.setTraitFor(&comp1, ComponentIntention(setup, IntentionType::LISTEN, 0,
    nullptr));
  intent.setTraitFor(&comp2, ComponentIntention(setup, IntentionType::SEND, 2,
    &msg));

  ANLComputer acCanon(&setup, &tnt, &intent, ANLFilterNothing);
  std::vector<NetworkState> resultCanon = acCanon.transition();
  ASSERT_EQ(2, resultCanon.size());

  // Determine which is which.
  NetworkState* colState = nullptr;
  NetworkState* rcvState = nullptr;
  for (std::size_t i = 0; i < 2; i++) {
    ActionType type = resultCanon[i].getTraitFor(&comp1).getType();
    switch (type) {
      case ActionType::COLLISION:
        colState = &resultCanon[i];
        break;
      case ActionType::RECEIVED:
        rcvState = &resultCanon[i];
        break;
      default: break;
    }
  }
  ASSERT_NE(nullptr, colState);
  ASSERT_NE(nullptr, rcvState);

  // Ensure that all three results are possible in canonical semantics.
  ASSERT_EQ(ActionType::COLLISION, colState->getTraitFor(&comp1).getType());
  ASSERT_EQ(ActionType::RECEIVED, rcvState->getTraitFor(&comp1).getType());
  ASSERT_EQ(0, colState->getTraitFor(&comp1).getTic());
  ASSERT_EQ(2, rcvState->getTraitFor(&comp1).getTic());
  ASSERT_EQ(nullptr, colState->getTraitFor(&comp1).getMessage());
  ASSERT_EQ(&msg, rcvState->getTraitFor(&comp1).getMessage());

  // Ensure that sending works in all cases in canonical semantics.
  ASSERT_EQ(ActionType::SENT, colState->getTraitFor(&comp2).getType());
  ASSERT_EQ(ActionType::SENT, rcvState->getTraitFor(&comp2).getType());
  ASSERT_EQ(2, colState->getTraitFor(&comp2).getTic());
  ASSERT_EQ(2, rcvState->getTraitFor(&comp2).getTic());
  ASSERT_EQ(&msg, colState->getTraitFor(&comp2).getMessage());
  ASSERT_EQ(&msg, rcvState->getTraitFor(&comp2).getMessage());

  ANLComputer acNaive(&setup, &tnt, &intent, ANLFilterNaive);
  std::vector<NetworkState> resultNaive = acNaive.transition();

  // Ensure that only a success is possible in naive semantics.
  ASSERT_EQ(1, resultNaive.size());
  ASSERT_EQ(ActionType::RECEIVED, resultNaive[0].getTraitFor(&comp1).getType());
  ASSERT_EQ(2, resultNaive[0].getTraitFor(&comp1).getTic());
  ASSERT_EQ(&msg, resultNaive[0].getTraitFor(&comp1).getMessage());
  ASSERT_EQ(ActionType::SENT, resultNaive[0].getTraitFor(&comp2).getType());
  ASSERT_EQ(2, resultNaive[0].getTraitFor(&comp2).getTic());
  ASSERT_EQ(&msg, resultNaive[0].getTraitFor(&comp2).getMessage());
}

// _____________________________________________________________________________
TEST(ANLComputerTest, listenToMultiple) {
  // Scenario: a component listening to multiple sending components.
  // Why: in the naive semantics this is a collision. in the canonical semantics
  //  this can be a collision, silence, or a success with one of the two
  //  messages.
  NetworkSetup setup(20);
  TrivialNetworkTopology tnt;
  Component comp1;
  Component comp2;
  Component comp3;
  setup.registerComponent(&comp1);
  setup.registerComponent(&comp2);
  setup.registerComponent(&comp3);
  Message msg1;
  Message msg2;
  setup.registerMessage(&msg1);
  setup.registerMessage(&msg2);

  IntentionAssignment intent(&setup);
  intent.setTraitFor(&comp1, ComponentIntention(setup, IntentionType::LISTEN, 0,
    nullptr));
  intent.setTraitFor(&comp2, ComponentIntention(setup, IntentionType::SEND, 2,
    &msg1));
  intent.setTraitFor(&comp3, ComponentIntention(setup, IntentionType::SEND, 2,
    &msg2));

  ANLComputer acCanon(&setup, &tnt, &intent, ANLFilterNothing);
  std::vector<NetworkState> resultCanon = acCanon.transition();
  ASSERT_EQ(3, resultCanon.size());

  // Determine which is which.
  NetworkState* colState = nullptr;
  NetworkState* rcv1State = nullptr;
  NetworkState* rcv2State = nullptr;
  for (std::size_t i = 0; i < 3; i++) {
    ActionType type = resultCanon[i].getTraitFor(&comp1).getType();
    const Message* msg = resultCanon[i].getTraitFor(&comp1).getMessage();
    switch (type) {
      case ActionType::COLLISION:
        colState = &resultCanon[i];
        break;
      case ActionType::RECEIVED:
        ASSERT_NE(nullptr, msg);
        if (msg == &msg1) {
          rcv1State = &resultCanon[i];
        } else {
          rcv2State = &resultCanon[i];
        }
        break;
      default: break;
    }
  }
  ASSERT_NE(nullptr, colState);
  ASSERT_NE(nullptr, rcv1State);
  ASSERT_NE(nullptr, rcv2State);

  // Ensure that all four results are possible in canonical semantics.
  ASSERT_EQ(ActionType::COLLISION, colState->getTraitFor(&comp1).getType());
  ASSERT_EQ(ActionType::RECEIVED, rcv1State->getTraitFor(&comp1).getType());
  ASSERT_EQ(ActionType::RECEIVED, rcv2State->getTraitFor(&comp1).getType());
  ASSERT_EQ(0, colState->getTraitFor(&comp1).getTic());
  ASSERT_EQ(2, rcv1State->getTraitFor(&comp1).getTic());
  ASSERT_EQ(2, rcv2State->getTraitFor(&comp1).getTic());
  ASSERT_EQ(nullptr, colState->getTraitFor(&comp1).getMessage());
  ASSERT_EQ(&msg1, rcv1State->getTraitFor(&comp1).getMessage());
  ASSERT_EQ(&msg2, rcv2State->getTraitFor(&comp1).getMessage());

  // Ensure that comp2 is sending in any case in canonical semantics.
  ASSERT_EQ(ActionType::SENT, colState->getTraitFor(&comp2).getType());
  ASSERT_EQ(ActionType::SENT, rcv1State->getTraitFor(&comp2).getType());
  ASSERT_EQ(ActionType::SENT, rcv2State->getTraitFor(&comp2).getType());
  ASSERT_EQ(2, colState->getTraitFor(&comp2).getTic());
  ASSERT_EQ(2, rcv1State->getTraitFor(&comp2).getTic());
  ASSERT_EQ(2, rcv2State->getTraitFor(&comp2).getTic());
  ASSERT_EQ(&msg1, colState->getTraitFor(&comp2).getMessage());
  ASSERT_EQ(&msg1, rcv1State->getTraitFor(&comp2).getMessage());
  ASSERT_EQ(&msg1, rcv2State->getTraitFor(&comp2).getMessage());

  // Ensure that comp3 is sending in any case in canonical semantics.
  ASSERT_EQ(ActionType::SENT, colState->getTraitFor(&comp3).getType());
  ASSERT_EQ(ActionType::SENT, rcv1State->getTraitFor(&comp3).getType());
  ASSERT_EQ(ActionType::SENT, rcv2State->getTraitFor(&comp3).getType());
  ASSERT_EQ(2, colState->getTraitFor(&comp3).getTic());
  ASSERT_EQ(2, rcv1State->getTraitFor(&comp3).getTic());
  ASSERT_EQ(2, rcv2State->getTraitFor(&comp3).getTic());
  ASSERT_EQ(&msg2, colState->getTraitFor(&comp3).getMessage());
  ASSERT_EQ(&msg2, rcv1State->getTraitFor(&comp3).getMessage());
  ASSERT_EQ(&msg2, rcv2State->getTraitFor(&comp3).getMessage());

  ANLComputer acNaive(&setup, &tnt, &intent, ANLFilterNaive);
  std::vector<NetworkState> resultNaive = acNaive.transition();

  // Ensure that only collisions are possible in naive semantics.
  ASSERT_EQ(1, resultNaive.size());
  ASSERT_EQ(ActionType::COLLISION,
    resultNaive[0].getTraitFor(&comp1).getType());
  ASSERT_EQ(0, resultNaive[0].getTraitFor(&comp1).getTic());
  ASSERT_EQ(nullptr, resultNaive[0].getTraitFor(&comp1).getMessage());
  ASSERT_EQ(ActionType::SENT, resultNaive[0].getTraitFor(&comp2).getType());
  ASSERT_EQ(2, resultNaive[0].getTraitFor(&comp2).getTic());
  ASSERT_EQ(&msg1, resultNaive[0].getTraitFor(&comp2).getMessage());
  ASSERT_EQ(ActionType::SENT, resultNaive[0].getTraitFor(&comp3).getType());
  ASSERT_EQ(2, resultNaive[0].getTraitFor(&comp3).getTic());
  ASSERT_EQ(&msg2, resultNaive[0].getTraitFor(&comp3).getMessage());
}

// _____________________________________________________________________________
TEST(ANLComputerTest, listenToMultipleIdentical) {
  // Scenario: a component listening to multiple sending components sending the
  //  same message at the same time.
  // Why: in the naive semantics this is a collision. in the canonical semantics
  //  this can be a collision, silence, or a success with one of the two
  //  messages.
  NetworkSetup setup(20);
  TrivialNetworkTopology tnt;
  Component comp1;
  Component comp2;
  Component comp3;
  setup.registerComponent(&comp1);
  setup.registerComponent(&comp2);
  setup.registerComponent(&comp3);
  Message msg;
  setup.registerMessage(&msg);

  IntentionAssignment intent(&setup);
  intent.setTraitFor(&comp1, ComponentIntention(setup, IntentionType::LISTEN, 0,
    nullptr));
  intent.setTraitFor(&comp2, ComponentIntention(setup, IntentionType::SEND, 2,
    &msg));
  intent.setTraitFor(&comp3, ComponentIntention(setup, IntentionType::SEND, 2,
    &msg));

  ANLComputer acCanon(&setup, &tnt, &intent, ANLFilterNothing);
  std::vector<NetworkState> resultCanon = acCanon.transition();
  ASSERT_EQ(2, resultCanon.size());

  // Determine which is which.
  NetworkState* colState = nullptr;
  NetworkState* rcvState = nullptr;
  for (std::size_t i = 0; i < 2; i++) {
    ActionType type = resultCanon[i].getTraitFor(&comp1).getType();
    switch (type) {
      case ActionType::COLLISION:
        colState = &resultCanon[i];
        break;
      case ActionType::RECEIVED:
        rcvState = &resultCanon[i];
        break;
      default: break;
    }
  }
  ASSERT_NE(nullptr, colState);
  ASSERT_NE(nullptr, rcvState);

  // Ensure that all four results are possible in canonical semantics.
  ASSERT_EQ(ActionType::COLLISION, colState->getTraitFor(&comp1).getType());
  ASSERT_EQ(ActionType::RECEIVED, rcvState->getTraitFor(&comp1).getType());
  ASSERT_EQ(0, colState->getTraitFor(&comp1).getTic());
  ASSERT_EQ(2, rcvState->getTraitFor(&comp1).getTic());
  ASSERT_EQ(nullptr, colState->getTraitFor(&comp1).getMessage());
  ASSERT_EQ(&msg, rcvState->getTraitFor(&comp1).getMessage());

  // Ensure that comp2 is sending in any case in canonical semantics.
  ASSERT_EQ(ActionType::SENT, colState->getTraitFor(&comp2).getType());
  ASSERT_EQ(ActionType::SENT, rcvState->getTraitFor(&comp2).getType());
  ASSERT_EQ(2, colState->getTraitFor(&comp2).getTic());
  ASSERT_EQ(2, rcvState->getTraitFor(&comp2).getTic());
  ASSERT_EQ(&msg, colState->getTraitFor(&comp2).getMessage());
  ASSERT_EQ(&msg, rcvState->getTraitFor(&comp2).getMessage());

  // Ensure that comp3 is sending in any case in canonical semantics.
  ASSERT_EQ(ActionType::SENT, colState->getTraitFor(&comp3).getType());
  ASSERT_EQ(ActionType::SENT, rcvState->getTraitFor(&comp3).getType());
  ASSERT_EQ(2, colState->getTraitFor(&comp3).getTic());
  ASSERT_EQ(2, rcvState->getTraitFor(&comp3).getTic());
  ASSERT_EQ(&msg, colState->getTraitFor(&comp3).getMessage());
  ASSERT_EQ(&msg, rcvState->getTraitFor(&comp3).getMessage());

  ANLComputer acNaive(&setup, &tnt, &intent, ANLFilterNaive);
  std::vector<NetworkState> resultNaive = acNaive.transition();

  // Ensure that only collisions are possible in naive semantics.
  ASSERT_EQ(1, resultNaive.size());
  ASSERT_EQ(ActionType::COLLISION,
    resultNaive[0].getTraitFor(&comp1).getType());
  ASSERT_EQ(0, resultNaive[0].getTraitFor(&comp1).getTic());
  ASSERT_EQ(nullptr, resultNaive[0].getTraitFor(&comp1).getMessage());
  ASSERT_EQ(ActionType::SENT, resultNaive[0].getTraitFor(&comp2).getType());
  ASSERT_EQ(2, resultNaive[0].getTraitFor(&comp2).getTic());
  ASSERT_EQ(&msg, resultNaive[0].getTraitFor(&comp2).getMessage());
  ASSERT_EQ(ActionType::SENT, resultNaive[0].getTraitFor(&comp3).getType());
  ASSERT_EQ(2, resultNaive[0].getTraitFor(&comp3).getTic());
  ASSERT_EQ(&msg, resultNaive[0].getTraitFor(&comp3).getMessage());
}

// _____________________________________________________________________________
TEST(ANLComputerTest, threeSendersMotivationalExample) {
  // Scenario: the motivational example from the sender set subsection from the
  //  report.
  // Why: it is the motivational example which justified sender sets.
  NetworkSetup setup(20);
  Component c1, c2, c3;
  ExampleNetworkTopology topo(&c1, &c2, &c3);
  Message msg;
  setup.registerMessage(&msg);
  setup.registerComponent(&c1);
  setup.registerComponent(&c2);
  setup.registerComponent(&c3);

  IntentionAssignment intent(&setup);
  intent.setTraitFor(&c1, ComponentIntention(setup, IntentionType::SEND, 1,
    &msg));
  intent.setTraitFor(&c2, ComponentIntention(setup, IntentionType::SEND, 2,
    &msg));
  intent.setTraitFor(&c3, ComponentIntention(setup, IntentionType::SEND, 3,
    &msg));

  ANLComputer acCanon(&setup, &topo, &intent, ANLFilterNothing);
  std::vector<NetworkState> resultCanon = acCanon.transition();
  ASSERT_EQ(1, resultCanon.size());
  ASSERT_EQ(ActionType::SENT, resultCanon[0].getTraitFor(&c1).getType());
  ASSERT_EQ(ActionType::CANCELLED, resultCanon[0].getTraitFor(&c2).getType());
  ASSERT_EQ(ActionType::SENT, resultCanon[0].getTraitFor(&c3).getType());
  ASSERT_EQ(1, resultCanon[0].getTraitFor(&c1).getTic());
  ASSERT_EQ(2, resultCanon[0].getTraitFor(&c2).getTic());
  ASSERT_EQ(3, resultCanon[0].getTraitFor(&c3).getTic());
  ASSERT_EQ(&msg, resultCanon[0].getTraitFor(&c1).getMessage());
  ASSERT_EQ(&msg, resultCanon[0].getTraitFor(&c2).getMessage());
  ASSERT_EQ(&msg, resultCanon[0].getTraitFor(&c3).getMessage());

  ANLComputer acNaive(&setup, &topo, &intent, ANLFilterNaive);
  std::vector<NetworkState> resultNaive = acNaive.transition();
  ASSERT_EQ(1, resultNaive.size());
  ASSERT_EQ(ActionType::SENT, resultNaive[0].getTraitFor(&c1).getType());
  ASSERT_EQ(ActionType::CANCELLED, resultNaive[0].getTraitFor(&c2).getType());
  ASSERT_EQ(ActionType::SENT, resultNaive[0].getTraitFor(&c3).getType());
  ASSERT_EQ(1, resultNaive[0].getTraitFor(&c1).getTic());
  ASSERT_EQ(2, resultNaive[0].getTraitFor(&c2).getTic());
  ASSERT_EQ(3, resultNaive[0].getTraitFor(&c3).getTic());
  ASSERT_EQ(&msg, resultNaive[0].getTraitFor(&c1).getMessage());
  ASSERT_EQ(&msg, resultNaive[0].getTraitFor(&c2).getMessage());
  ASSERT_EQ(&msg, resultNaive[0].getTraitFor(&c3).getMessage());
}
