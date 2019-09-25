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
#include <string>
#include <vector>
#include "anl/core/anl.h"
#include "anl/core/statemachine.h"

// We do this here as a test can under no circumstance pollute the namespace as
// it is never included.
using namespace Core;  // NOLINT

// _____________________________________________________________________________
TEST(MessageTest, toString) {
  // Scenario: we obtain the textual representation of two distinct messages.
  // Why: unit testing the default method behavior.
  Message m1;
  ASSERT_EQ("Message", m1.toString());

  Message m2;
  ASSERT_EQ("Message", m2.toString());
}

// _____________________________________________________________________________
TEST(MessageTest, toXML) {
  // Scenario: we obtain the XML representation of two distinct message. the
  //  base method returns an empty representation.
  // Why: unit testing the default method behavior.
  Message m1;
  ASSERT_EQ(0, m1.toXML().size());

  Message m2;
  ASSERT_EQ(0, m2.toXML().size());
}

// _____________________________________________________________________________
TEST(ComponentTest, getId) {
  // Scenario: we obtain the textual representation of two distinct components.
  // Why: unit testing the default method behavior.
  Component comp1;
  ASSERT_EQ("default", comp1.getId());

  Component comp2;
  ASSERT_EQ("default", comp2.getId());
}

// _____________________________________________________________________________
TEST(ComponentTest, toXML) {
  // Scenario: we obtain the XML representation of two distinct components. the
  //  base method returns an empty representation.
  // Why: unit testing the default method behavior.
  Component comp1;
  ASSERT_EQ(0, comp1.toXML().size());

  Component comp2;
  ASSERT_EQ(0, comp2.toXML().size());
}

// _____________________________________________________________________________
TEST(ComponentActionDeathTest, tooBigTicFails) {
  // Scenario: supplying a too big tic fails. we test tics 5..25.
  // Why: 5 is a corner case, the others are regular cases.
  NetworkSetup setup(5);
  Message m;
  setup.registerMessage(&m);

  for (std::size_t i = 5; i < 26; i++) {
    ASSERT_DEATH(ComponentAction(setup, ActionType::RECEIVED, i, &m),
      "invalid tic number");
  }
}

// _____________________________________________________________________________
TEST(ComponentActionTest, validActionCanBeCreated) {
  // Scenario: we create valid component actions, the test does complete
  //  sucessfully.
  // Why: we create one regular instance of every action type.
  NetworkSetup setup(20);
  Message msg;
  setup.registerMessage(&msg);

  ComponentAction m1(setup, ActionType::IDLE, 0, nullptr);
  ComponentAction m2(setup, ActionType::COLLISION, 0, nullptr);
  ComponentAction m3(setup, ActionType::SILENCE, 0, nullptr);
  ComponentAction m4(setup, ActionType::RECEIVED, 3, &msg);
  ComponentAction m5(setup, ActionType::CANCELLED, 5, &msg);
  ComponentAction m6(setup, ActionType::SENT, 7, &msg);

  // We have no assertion to ensure the program is still running. Thus we rely
  //  on the developer to notice if the test does not complete successfully.
}

// _____________________________________________________________________________
TEST(ComponentActionTest, messagesDoNotHaveToBeRegistered) {
  // Scenario: we create component actions without registering the messages.
  //  this is possible but creates warnings in the output.
  // Why: the created instances are two regular instances of component actions.
  NetworkSetup setup(20);
  Message msgA;
  Message msgB;
  setup.registerMessage(&msgA);

  ComponentAction m1(setup, ActionType::RECEIVED, 1, &msgA);
  ComponentAction m2(setup, ActionType::CANCELLED, 2, &msgB);

  // We have no assertion to ensure the program is still running. Thus we rely
  //  on the developer to notice if the test does not complete successfully.
}

// _____________________________________________________________________________
TEST(ComponentActionTest, toXML) {
  // Scenario: we get the XML representation for each of the component action
  //  types.
  // Why: component action types are the main distinction, thus we use the same
  //  message and tic.
  NetworkSetup setup(20);
  Message msg;
  setup.registerMessage(&msg);

  ComponentAction m1(setup, ActionType::IDLE, 0, nullptr);
  ComponentAction m2(setup, ActionType::COLLISION, 0, nullptr);
  ComponentAction m3(setup, ActionType::SILENCE, 0, nullptr);
  ComponentAction m4(setup, ActionType::RECEIVED, 3, &msg);
  ComponentAction m5(setup, ActionType::CANCELLED, 3, &msg);
  ComponentAction m6(setup, ActionType::SENT, 3, &msg);

  std::vector<std::string> r1 = m1.toXML();
  std::vector<std::string> r2 = m2.toXML();
  std::vector<std::string> r3 = m3.toXML();
  std::vector<std::string> r4 = m4.toXML();
  std::vector<std::string> r5 = m5.toXML();
  std::vector<std::string> r6 = m6.toXML();

  ASSERT_EQ(3, r1.size());
  ASSERT_EQ(3, r2.size());
  ASSERT_EQ(3, r3.size());
  ASSERT_EQ(6, r4.size());
  ASSERT_EQ(6, r5.size());
  ASSERT_EQ(6, r6.size());

  ASSERT_EQ("<trait>", r1[0]);
  ASSERT_EQ("<trait>", r2[0]);
  ASSERT_EQ("<trait>", r3[0]);
  ASSERT_EQ("<trait>", r4[0]);
  ASSERT_EQ("<trait>", r5[0]);
  ASSERT_EQ("<trait>", r6[0]);

  ASSERT_EQ("  <type>IDL</type>", r1[1]);
  ASSERT_EQ("  <type>COL</type>", r2[1]);
  ASSERT_EQ("  <type>SIL</type>", r3[1]);
  ASSERT_EQ("  <type>RCVD</type>", r4[1]);
  ASSERT_EQ("  <type>CCLD</type>", r5[1]);
  ASSERT_EQ("  <type>SENT</type>", r6[1]);

  ASSERT_EQ("  <msg>", r4[2]);
  ASSERT_EQ("  <msg>", r5[2]);
  ASSERT_EQ("  <msg>", r6[2]);

  ASSERT_EQ("  </msg>", r4[3]);
  ASSERT_EQ("  </msg>", r5[3]);
  ASSERT_EQ("  </msg>", r6[3]);

  ASSERT_EQ("  <tic>3</tic>", r4[4]);
  ASSERT_EQ("  <tic>3</tic>", r5[4]);
  ASSERT_EQ("  <tic>3</tic>", r6[4]);

  ASSERT_EQ("</trait>", r1[2]);
  ASSERT_EQ("</trait>", r2[2]);
  ASSERT_EQ("</trait>", r3[2]);
  ASSERT_EQ("</trait>", r4[5]);
  ASSERT_EQ("</trait>", r5[5]);
  ASSERT_EQ("</trait>", r6[5]);
}

// _____________________________________________________________________________
TEST(ComponentActionTest, operatorEqTest) {
  // Scenario: we test the equality operator for component actions using four
  //  component actions of which there are two pairs of semantically equal
  //  component actions.
  // Why: we ensure that "equal" component actions in distinct places in memory
  //  are still equal.
  NetworkSetup setup(20);
  Message msg;
  setup.registerMessage(&msg);

  ComponentAction m1(setup, ActionType::RECEIVED, 3, &msg);
  ComponentAction m2(setup, ActionType::RECEIVED, 3, &msg);

  // m1 and m2 are equal.
  ASSERT_TRUE(m1 == m1);
  ASSERT_TRUE(m2 == m2);
  ASSERT_TRUE(m1 == m2);
  ASSERT_TRUE(m2 == m1);

  ComponentAction m3(setup, ActionType::IDLE, 0, nullptr);
  ComponentAction m4(setup, ActionType::IDLE, 0, nullptr);

  // m3 and m4 are equal.
  ASSERT_TRUE(m3 == m3);
  ASSERT_TRUE(m4 == m4);
  ASSERT_TRUE(m3 == m4);
  ASSERT_TRUE(m4 == m3);

  // All other equality relations using m1, m2, m3, m4 are false.
  ASSERT_FALSE(m1 == m3);
  ASSERT_FALSE(m3 == m1);
  ASSERT_FALSE(m1 == m4);
  ASSERT_FALSE(m4 == m1);
  ASSERT_FALSE(m2 == m3);
  ASSERT_FALSE(m3 == m2);
  ASSERT_FALSE(m2 == m4);
  ASSERT_FALSE(m4 == m2);
}

// _____________________________________________________________________________
TEST(ComponentActionTest, operatorNeTest) {
  // Scenario: we test the inequality operator for component actions using four
  //  component actions of which there are two pairs of semantically equal
  //  component actions.
  // Why: this test is very similar to the above test with operators and
  //  expectations swapped. this is to ensure that == is the opposite to !=.
  NetworkSetup setup(20);
  Message msg;
  setup.registerMessage(&msg);

  ComponentAction m1(setup, ActionType::RECEIVED, 3, &msg);
  ComponentAction m2(setup, ActionType::RECEIVED, 3, &msg);

  // m1 and m2 are equal.
  ASSERT_FALSE(m1 != m1);
  ASSERT_FALSE(m2 != m2);
  ASSERT_FALSE(m1 != m2);
  ASSERT_FALSE(m2 != m1);

  ComponentAction m3(setup, ActionType::IDLE, 0, nullptr);
  ComponentAction m4(setup, ActionType::IDLE, 0, nullptr);

  // m3 and m4 are equal.
  ASSERT_FALSE(m3 != m3);
  ASSERT_FALSE(m4 != m4);
  ASSERT_FALSE(m3 != m4);
  ASSERT_FALSE(m4 != m3);

  // All other inequalities using m1, m2, m3, m4 are true.
  ASSERT_TRUE(m1 != m3);
  ASSERT_TRUE(m3 != m1);
  ASSERT_TRUE(m1 != m4);
  ASSERT_TRUE(m4 != m1);
  ASSERT_TRUE(m2 != m3);
  ASSERT_TRUE(m3 != m2);
  ASSERT_TRUE(m2 != m4);
  ASSERT_TRUE(m4 != m2);
}

// _____________________________________________________________________________
TEST(ComponentActionTest, toString) {
  // Scenario: we ensure that the textual representation of a component action
  //  is not empty.
  // Why: we choose a component action without message and one with message to
  //  have both groups represented.
  NetworkSetup setup(20);
  Message msg;
  setup.registerMessage(&msg);

  ComponentAction act1(setup, ActionType::SILENCE, 0, nullptr);
  ASSERT_GT(act1.toString().size(), 0);
  ComponentAction act2(setup, ActionType::SENT, 4, &msg);
  ASSERT_GT(act2.toString().size(), 0);
}

// _____________________________________________________________________________
TEST(ComponentActionTest, getType) {
  // Scenario: we check that the type of an action declared as a collision is
  //  indeed "collision".
  // Why: we picked one action at random, getType() is a simple getter.
  NetworkSetup setup(20);
  ComponentAction act(setup, ActionType::COLLISION, 0, nullptr);
  ASSERT_EQ(ActionType::COLLISION, act.getType());
}

// _____________________________________________________________________________
TEST(ComponentActionTest, getTic) {
  // Scenario: we check that the tic of an action with an associated tic is
  //  indeed that tic.
  // Why: we picked one tic at random, getTic() is a simple getter.
  NetworkSetup setup(20);
  Message msg;
  setup.registerMessage(&msg);
  ComponentAction act(setup, ActionType::CANCELLED, 7, &msg);
  ASSERT_EQ(7, act.getTic());
}

// _____________________________________________________________________________
TEST(ComponentActionTest, getMessage) {
  // Scenario: we check that the message of an action with an associated message
  //  is indeed that message.
  // Why: we picked one message at random, getMessage() is a simple getter.
  NetworkSetup setup(20);
  Message msg;
  setup.registerMessage(&msg);
  ComponentAction act(setup, ActionType::SENT, 1, &msg);
  ASSERT_EQ(&msg, act.getMessage());
}

// _____________________________________________________________________________
TEST(ComponentIntentionDeathTest, tooBigTicFails) {
  // See the analoguous test for ComponentActions.
  NetworkSetup setup(5);
  Message m;
  setup.registerMessage(&m);

  for (std::size_t i = 5; i < 26; i++) {
    ASSERT_DEATH(ComponentIntention(setup, IntentionType::SEND, i, &m),
      "invalid tic number");
  }
}

// _____________________________________________________________________________
TEST(ComponentIntentionTest, validIntentionCanBeCreated) {
  // See the analoguous test for ComponentActions.
  NetworkSetup setup(20);
  Message msg;
  setup.registerMessage(&msg);

  ComponentIntention m1(setup, IntentionType::IDLE, 0, nullptr);
  ComponentIntention m2(setup, IntentionType::LISTEN, 0, nullptr);
  ComponentIntention m3(setup, IntentionType::SEND, 3, &msg);
  ComponentIntention m4(setup, IntentionType::SEND_FORCE, 5, &msg);
}

// _____________________________________________________________________________
TEST(ComponentIntentionTest, messagesDoNotHaveToBeRegistered) {
  // See the analoguous test for ComponentActions.
  NetworkSetup setup(20);
  Message msgA;
  Message msgB;
  setup.registerMessage(&msgA);

  ComponentIntention m1(setup, IntentionType::SEND, 1, &msgA);
  ComponentIntention m2(setup, IntentionType::SEND_FORCE, 2, &msgB);
}

// _____________________________________________________________________________
TEST(ComponentIntentionTest, toXML) {
  // See the analoguous test for ComponentActions.
  NetworkSetup setup(20);
  Message msg;
  setup.registerMessage(&msg);

  ComponentIntention m1(setup, IntentionType::IDLE, 0, nullptr);
  ComponentIntention m2(setup, IntentionType::LISTEN, 0, nullptr);
  ComponentIntention m3(setup, IntentionType::SEND, 3, &msg);
  ComponentIntention m4(setup, IntentionType::SEND_FORCE, 3, &msg);

  std::vector<std::string> r1 = m1.toXML();
  std::vector<std::string> r2 = m2.toXML();
  std::vector<std::string> r3 = m3.toXML();
  std::vector<std::string> r4 = m4.toXML();

  ASSERT_EQ(3, r1.size());
  ASSERT_EQ(3, r2.size());
  ASSERT_EQ(6, r3.size());
  ASSERT_EQ(6, r4.size());

  ASSERT_EQ("<trait>", r1[0]);
  ASSERT_EQ("<trait>", r2[0]);
  ASSERT_EQ("<trait>", r3[0]);
  ASSERT_EQ("<trait>", r4[0]);

  ASSERT_EQ("  <type>IDL</type>", r1[1]);
  ASSERT_EQ("  <type>LST</type>", r2[1]);
  ASSERT_EQ("  <type>SEND</type>", r3[1]);
  ASSERT_EQ("  <type>SEND!</type>", r4[1]);

  ASSERT_EQ("  <msg>", r3[2]);
  ASSERT_EQ("  <msg>", r4[2]);

  ASSERT_EQ("  </msg>", r3[3]);
  ASSERT_EQ("  </msg>", r4[3]);

  ASSERT_EQ("  <tic>3</tic>", r3[4]);
  ASSERT_EQ("  <tic>3</tic>", r4[4]);

  ASSERT_EQ("</trait>", r1[2]);
  ASSERT_EQ("</trait>", r2[2]);
  ASSERT_EQ("</trait>", r3[5]);
  ASSERT_EQ("</trait>", r4[5]);
}

// _____________________________________________________________________________
TEST(ComponentIntentionTest, operatorEqTest) {
  // See the analoguous test for ComponentActions.
  NetworkSetup setup(20);
  Message msg;
  setup.registerMessage(&msg);

  ComponentIntention m1(setup, IntentionType::SEND, 3, &msg);
  ComponentIntention m2(setup, IntentionType::SEND, 3, &msg);
  ASSERT_TRUE(m1 == m1);
  ASSERT_TRUE(m2 == m2);
  ASSERT_TRUE(m1 == m2);
  ASSERT_TRUE(m2 == m1);

  ComponentIntention m3(setup, IntentionType::IDLE, 0, nullptr);
  ComponentIntention m4(setup, IntentionType::IDLE, 0, nullptr);
  ASSERT_TRUE(m3 == m3);
  ASSERT_TRUE(m4 == m4);
  ASSERT_TRUE(m3 == m4);
  ASSERT_TRUE(m4 == m3);

  ASSERT_FALSE(m1 == m3);
  ASSERT_FALSE(m3 == m1);
  ASSERT_FALSE(m1 == m4);
  ASSERT_FALSE(m4 == m1);
  ASSERT_FALSE(m2 == m3);
  ASSERT_FALSE(m3 == m2);
  ASSERT_FALSE(m2 == m4);
  ASSERT_FALSE(m4 == m2);
}

// _____________________________________________________________________________
TEST(ComponentIntentionTest, operatorNeTest) {
  // See the analoguous test for ComponentActions.
  NetworkSetup setup(20);
  Message msg;
  setup.registerMessage(&msg);

  ComponentIntention m1(setup, IntentionType::SEND_FORCE, 3, &msg);
  ComponentIntention m2(setup, IntentionType::SEND_FORCE, 3, &msg);
  ASSERT_FALSE(m1 != m1);
  ASSERT_FALSE(m2 != m2);
  ASSERT_FALSE(m1 != m2);
  ASSERT_FALSE(m2 != m1);

  ComponentIntention m3(setup, IntentionType::IDLE, 0, nullptr);
  ComponentIntention m4(setup, IntentionType::IDLE, 0, nullptr);
  ASSERT_FALSE(m3 != m3);
  ASSERT_FALSE(m4 != m4);
  ASSERT_FALSE(m3 != m4);
  ASSERT_FALSE(m4 != m3);

  ASSERT_TRUE(m1 != m3);
  ASSERT_TRUE(m3 != m1);
  ASSERT_TRUE(m1 != m4);
  ASSERT_TRUE(m4 != m1);
  ASSERT_TRUE(m2 != m3);
  ASSERT_TRUE(m3 != m2);
  ASSERT_TRUE(m2 != m4);
  ASSERT_TRUE(m4 != m2);
}

// _____________________________________________________________________________
TEST(ComponentIntentionTest, toString) {
  // See the analoguous test for ComponentActions.
  NetworkSetup setup(20);
  ComponentIntention act(setup, IntentionType::LISTEN, 0, nullptr);
  ASSERT_GT(act.toString().size(), 0);
}

// _____________________________________________________________________________
TEST(ComponentIntentionTest, getType) {
  // See the analoguous test for ComponentActions.
  NetworkSetup setup(20);
  ComponentIntention act(setup, IntentionType::LISTEN, 0, nullptr);
  ASSERT_EQ(IntentionType::LISTEN, act.getType());
}

// _____________________________________________________________________________
TEST(ComponentIntentionTest, getTic) {
  // See the analoguous test for ComponentActions.
  NetworkSetup setup(20);
  Message msg;
  setup.registerMessage(&msg);
  ComponentIntention act(setup, IntentionType::SEND, 7, &msg);
  ASSERT_EQ(7, act.getTic());
}

// _____________________________________________________________________________
TEST(ComponentIntentionTest, getMessage) {
  // See the analoguous test for ComponentActions.
  NetworkSetup setup(20);
  Message msg;
  setup.registerMessage(&msg);
  ComponentIntention act(setup, IntentionType::SEND_FORCE, 1, &msg);
  ASSERT_EQ(&msg, act.getMessage());
}

// _____________________________________________________________________________
TEST(NetworkSetupDeathTest, invalidMaxTicFails) {
  // Scenario: we can not create a network setup with 0 tics per slot.
  // Why: abnormal exit point.
  ASSERT_DEATH(NetworkSetup(0), "at least one tic");
}

// _____________________________________________________________________________
TEST(NetworkSetupTest, getTicsPerSlot) {
  // Scenario: we test getTicsPerSlot() with a regular TPS value, a small TPS
  //  value and a huge TPS value.
  // Why: three different values in different magnitudes.
  NetworkSetup setupA(20);
  ASSERT_EQ(20, setupA.getTicsPerSlot());

  NetworkSetup setupB(5);
  ASSERT_EQ(5, setupB.getTicsPerSlot());

  NetworkSetup setupC(100000);
  ASSERT_EQ(100000, setupC.getTicsPerSlot());
}

// _____________________________________________________________________________
TEST(NetworkSetupTest, isComponent) {
  // Scenario: we gradually test two components becoming registered in a setup.
  // Why: two components as a minimal example where two different values can be
  //  returned for components.
  NetworkSetup setup(20);
  Component compA;
  Component compB;

  ASSERT_FALSE(setup.isComponent(compA));
  ASSERT_FALSE(setup.isComponent(compB));
  setup.registerComponent(&compA);
  ASSERT_TRUE(setup.isComponent(compA));
  ASSERT_FALSE(setup.isComponent(compB));
  setup.registerComponent(&compB);
  ASSERT_TRUE(setup.isComponent(compA));
  ASSERT_TRUE(setup.isComponent(compB));
}

// _____________________________________________________________________________
TEST(NetworkSetupTest, forEachComponent) {
  // Scenario: we check that forEachComponent does indeed call the callback once
  //  per component.
  // Why: two components as the minimal example that shows that the callback is
  //  not just called once but dependant on the component.
  NetworkSetup setup(20);
  Component compA;
  Component compB;
  setup.registerComponent(&compA);

  int j = 0;
  setup.forEachComponent([&j](const Component* p){ j++; });
  ASSERT_EQ(1, j);

  setup.registerComponent(&compB);

  int i = 0;
  setup.forEachComponent([&i](const Component* p){ i++; });
  ASSERT_EQ(2, i);
}

// _____________________________________________________________________________
TEST(NetworkSetupTest, registerComponent) {
  // Scenario: we check registering ten components.
  // Why: regular number of components for actual use-cases.
  NetworkSetup setup(20);
  Component comps[10];

  for (std::size_t i = 0; i < 10; i++) {
    ASSERT_FALSE(setup.isComponent(comps[i]));
    setup.registerComponent(&comps[i]);
    ASSERT_TRUE(setup.isComponent(comps[i]));
  }
}

// _____________________________________________________________________________
TEST(NetworkSetupTest, isMessage) {
  // See isComponent-test. This is analoguous, but with messages.
  NetworkSetup setup(20);
  Message msgA;
  Message msgB;

  ASSERT_FALSE(setup.isMessage(&msgA));
  ASSERT_FALSE(setup.isMessage(&msgB));
  setup.registerMessage(&msgA);
  ASSERT_TRUE(setup.isMessage(&msgA));
  ASSERT_FALSE(setup.isMessage(&msgB));
  setup.registerMessage(&msgB);
  ASSERT_TRUE(setup.isMessage(&msgA));
  ASSERT_TRUE(setup.isMessage(&msgB));
}

// _____________________________________________________________________________
TEST(NetworkSetupTest, registerMessage) {
  // See registerComponent-test. This is analoguous, but with messages.
  NetworkSetup setup(20);
  Message msgs[10];

  for (std::size_t i = 0; i < 10; i++) {
    ASSERT_FALSE(setup.isMessage(&msgs[i]));
    setup.registerMessage(&msgs[i]);
    ASSERT_TRUE(setup.isMessage(&msgs[i]));
  }
}

// _____________________________________________________________________________
TEST(NetworkSetupDeathTest, duplicateMessageRegistrationFails) {
  // Scenario: registering the same message twice fails.
  // Why: abnormal exit point in method. we add an extra message to ensure that
  //  there is not only memory of the previous message.
  NetworkSetup setup(20);
  Message msg1;
  Message msg2;

  setup.registerMessage(&msg1);
  setup.registerMessage(&msg2);
  ASSERT_DEATH(setup.registerMessage(&msg1), "duplicate");
}

// _____________________________________________________________________________
TEST(NetworkSetupDeathTest, duplicateComponentRegistrationFails) {
  // See duplicateMessageRegistrationFails-test. This is analoguous, but with
  //  components.
  NetworkSetup setup(20);
  Component comp1;
  Component comp2;

  setup.registerComponent(&comp1);
  setup.registerComponent(&comp2);
  ASSERT_DEATH(setup.registerComponent(&comp1), "duplicate");
}

// _____________________________________________________________________________
TEST(NetworkSetupDeathTest, canNotRegisterNullptrAsComponent) {
  // Scenario: registering a nullptr as a component fails.
  // Why: abnormal exit point in method.
  NetworkSetup setup(20);
  ASSERT_DEATH(setup.registerComponent(nullptr), "can not register nullptr");
}

// _____________________________________________________________________________
TEST(NetworkSetupDeathTest, canNotRegisterNullptrAsMessage) {
  // See canNotRegisterNullptrAsComponent, this is analoguous.
  NetworkSetup setup(20);
  Message msg1;

  setup.registerMessage(&msg1);
  ASSERT_DEATH(setup.registerMessage(nullptr), "can not register nullptr");
}

// _____________________________________________________________________________
TEST(NetworkStateDeathTest, canNotGetFromPartialState) {
  // Scenario: getting a component action from a partial state fails.
  // Why: abnormal exit point in method.
  NetworkSetup setup(20);
  NetworkState state(&setup);
  Component comp;

  ASSERT_DEATH(state.getTraitFor(&comp),
    "attempting to get trait for partial");
}

// _____________________________________________________________________________
TEST(NetworkStateDeathTest, canNotGetStringOfPartialState) {
  // Scenario: toString() on partial state fails.
  // Why: abnormal exit point in method.
  NetworkSetup setup(20);
  NetworkState state(&setup);

  ASSERT_DEATH(state.toString(), "attempting to get string for partial");
}

// _____________________________________________________________________________
TEST(NetworkStateDeathTest, canNotGetXMLOfPartialState) {
  // Scenario: toXML() on partial state fails.
  // Why: abnormal exit point in method.
  NetworkSetup setup(20);
  NetworkState state(&setup);

  ASSERT_DEATH(state.toXML(), "attempting to get XML for partial");
}

// _____________________________________________________________________________
TEST(NetworkStateDeathTest, canNotGetInvalidFromState) {
  // Scenario: getting a component action for an unregistered component fails.
  // Why: abnormal exit point in method.
  NetworkSetup setup(20);
  NetworkState state(&setup);
  Component comp1;
  Component comp2;

  setup.registerComponent(&comp1);
  state.setTraitFor(&comp1,
    ComponentAction(setup, ActionType::IDLE, 0, nullptr));

  ASSERT_DEATH(state.getTraitFor(&comp2), "not a valid component for");
}

// _____________________________________________________________________________
TEST(NetworkStateTest, isPartial) {
  // Scenario: we test the getter isPartial() before and after each assignment
  //  to the state.
  // Why: this lets us ensure the value changes at exactly the point it is
  //  supposed to.
  NetworkSetup setup(20);
  NetworkState state(&setup);
  Component comp1;
  Component comp2;
  ComponentAction act1(setup, ActionType::IDLE, 0, nullptr);
  ComponentAction act2(setup, ActionType::COLLISION, 0, nullptr);

  setup.registerComponent(&comp1);
  setup.registerComponent(&comp2);

  ASSERT_TRUE(state.isPartial());
  state.setTraitFor(&comp1, act1);
  ASSERT_TRUE(state.isPartial());
  state.setTraitFor(&comp2, act2);
  ASSERT_FALSE(state.isPartial());
}

// _____________________________________________________________________________
TEST(NetworkStateTest, getTraitFor) {
  // Scenario: we test that the correct component action is returned for both
  //  components.
  // Why: we chose two components as a minimal example where there are multiple
  //  component actions that could be returned by the network state.
  NetworkSetup setup(20);
  NetworkState state(&setup);
  Component comp1;
  Component comp2;
  ComponentAction act1(setup, ActionType::IDLE, 0, nullptr);
  ComponentAction act2(setup, ActionType::COLLISION, 0, nullptr);

  setup.registerComponent(&comp1);
  setup.registerComponent(&comp2);
  state.setTraitFor(&comp1, act1);
  state.setTraitFor(&comp2, act2);

  ASSERT_NE(act1, act2);
  ASSERT_EQ(act1, state.getTraitFor(&comp1));
  ASSERT_EQ(act2, state.getTraitFor(&comp2));
}

// _____________________________________________________________________________
TEST(NetworkStateTest, toString) {
  // Scenario: we test that the textual representation of a network state with
  //  two component actions is not empty.
  // Why: two components in order to have a "stronger" distinction between the
  //  representation of a component action and a network state.
  NetworkSetup setup(20);
  NetworkState state(&setup);
  Component comp1;
  Component comp2;
  ComponentAction act1(setup, ActionType::IDLE, 0, nullptr);
  ComponentAction act2(setup, ActionType::COLLISION, 0, nullptr);

  setup.registerComponent(&comp1);
  setup.registerComponent(&comp2);
  state.setTraitFor(&comp1, act1);
  state.setTraitFor(&comp2, act2);

  // Assert at least one char per action.
  ASSERT_GE(state.toString().size(), 2);
}

// _____________________________________________________________________________
TEST(NetworkStateTest, toXML) {
  // Scenario: we test the xml representation of a network state with two
  //  component actions.
  // Why: one component action without message and tic (COLLISION) and one
  //  component action with those (SENT) have one representative of both of
  //  these kinds. furthermore, we obtain a case with more than one component
  //  action.
  NetworkSetup setup(20);
  Message msg;
  Component comp1;
  Component comp2;
  setup.registerComponent(&comp1);
  setup.registerComponent(&comp2);
  setup.registerMessage(&msg);

  NetworkState state(&setup);
  ComponentAction act1(setup, ActionType::COLLISION, 0, nullptr);
  ComponentAction act2(setup, ActionType::SENT, 3, &msg);
  state.setTraitFor(&comp1, act1);
  state.setTraitFor(&comp2, act2);

  // Check the XML representation.
  std::vector<std::string> repr = state.toXML();
  ASSERT_EQ(15, repr.size());

  ASSERT_EQ("<entry>", repr[0]);
  ASSERT_EQ("  <for>default</for>", repr[1]);
  ASSERT_EQ("  <trait>", repr[2]);
  ASSERT_EQ("    <type>COL</type>", repr[3]);
  ASSERT_EQ("  </trait>", repr[4]);
  ASSERT_EQ("</entry>", repr[5]);
  ASSERT_EQ("<entry>", repr[6]);
  ASSERT_EQ("  <for>default</for>", repr[7]);
  ASSERT_EQ("  <trait>", repr[8]);
  ASSERT_EQ("    <type>SENT</type>", repr[9]);
  ASSERT_EQ("    <msg>", repr[10]);
  ASSERT_EQ("    </msg>", repr[11]);
  ASSERT_EQ("    <tic>3</tic>", repr[12]);
  ASSERT_EQ("  </trait>", repr[13]);
  ASSERT_EQ("</entry>", repr[14]);
}

// _____________________________________________________________________________
TEST(NetworkStateDeathTest, canNotSetInvalid) {
  // See canNotGetInvalidFromState-test, this is analoguous.
  NetworkSetup setup(20);
  NetworkState state(&setup);
  Component comp;
  ComponentAction act(setup, ActionType::IDLE, 0, nullptr);

  ASSERT_DEATH(state.setTraitFor(&comp, act), "not a valid component for");
}

// _____________________________________________________________________________
TEST(NetworkStateDeathTest, canNotOverride) {
  // Scenario: it is impossible to override an assigned component action by
  //  another.
  // Why: abnormal exit point from method.
  NetworkSetup setup(20);
  NetworkState state(&setup);
  Component comp;
  ComponentAction act1(setup, ActionType::COLLISION, 0, nullptr);
  ComponentAction act2(setup, ActionType::SILENCE, 0, nullptr);
  setup.registerComponent(&comp);

  state.setTraitFor(&comp, act1);
  ASSERT_DEATH(state.setTraitFor(&comp, act2), "can not override");
}

// _____________________________________________________________________________
TEST(NetworkStateTest, setTraitFor) {
  // Scenario: we set the component actions for two components.
  // Why: minimal example that allows distinct component actions to be assigned.
  NetworkSetup setup(20);
  NetworkState state(&setup);
  Component comp1;
  Component comp2;
  ComponentAction act1(setup, ActionType::COLLISION, 0, nullptr);
  ComponentAction act2(setup, ActionType::SILENCE, 0, nullptr);
  setup.registerComponent(&comp1);
  setup.registerComponent(&comp2);

  state.setTraitFor(&comp1, act1);
  state.setTraitFor(&comp2, act2);

  // These assertions not failing also implies that the state is no longer
  // partial.
  ASSERT_EQ(act1, state.getTraitFor(&comp1));
  ASSERT_EQ(act2, state.getTraitFor(&comp2));
}

// _____________________________________________________________________________
TEST(IntentionAssignmentDeathTest, canNotGetFromPartialState) {
  // See same test but for NetworkStates -- this is analoguous.
  NetworkSetup setup(20);
  IntentionAssignment assgn(&setup);
  Component comp;

  ASSERT_DEATH(assgn.getTraitFor(&comp),
    "attempting to get trait for partial");
}

// _____________________________________________________________________________
TEST(IntentionAssignmentDeathTest, canNotGetStringOfPartialState) {
  // See same test but for NetworkStates -- this is analoguous.
  NetworkSetup setup(20);
  IntentionAssignment assgn(&setup);

  ASSERT_DEATH(assgn.toString(), "attempting to get string for partial");
}

// _____________________________________________________________________________
TEST(IntentionAssignmentDeathTest, canNotGetXMLOfPartialState) {
  // See same test but for NetworkStates -- this is analoguous.
  NetworkSetup setup(20);
  IntentionAssignment assgn(&setup);

  ASSERT_DEATH(assgn.toXML(), "attempting to get XML for partial");
}

// _____________________________________________________________________________
TEST(IntentionAssignmentDeathTest, canNotGetInvalidFromState) {
  // See same test but for NetworkStates -- this is analoguous.
  NetworkSetup setup(20);
  IntentionAssignment assgn(&setup);
  Component comp1;
  Component comp2;

  setup.registerComponent(&comp1);
  assgn.setTraitFor(&comp1,
    ComponentIntention(setup, IntentionType::IDLE, 0, nullptr));

  ASSERT_DEATH(assgn.getTraitFor(&comp2), "not a valid component for");
}

// _____________________________________________________________________________
TEST(IntentionAssignmentTest, isPartial) {
  // See same test but for NetworkStates -- this is analoguous.
  NetworkSetup setup(20);
  IntentionAssignment assgn(&setup);
  Component comp1;
  Component comp2;
  ComponentIntention act1(setup, IntentionType::IDLE, 0, nullptr);
  ComponentIntention act2(setup, IntentionType::LISTEN, 0, nullptr);

  setup.registerComponent(&comp1);
  setup.registerComponent(&comp2);

  ASSERT_TRUE(assgn.isPartial());
  assgn.setTraitFor(&comp1, act1);
  ASSERT_TRUE(assgn.isPartial());
  assgn.setTraitFor(&comp2, act2);
  ASSERT_FALSE(assgn.isPartial());
}

// _____________________________________________________________________________
TEST(IntentionAssignmentTest, getTraitFor) {
  // See same test but for NetworkStates -- this is analoguous.
  NetworkSetup setup(20);
  IntentionAssignment assgn(&setup);
  Component comp1;
  Component comp2;
  ComponentIntention act1(setup, IntentionType::IDLE, 0, nullptr);
  ComponentIntention act2(setup, IntentionType::LISTEN, 0, nullptr);

  setup.registerComponent(&comp1);
  setup.registerComponent(&comp2);
  assgn.setTraitFor(&comp1, act1);
  assgn.setTraitFor(&comp2, act2);

  ASSERT_NE(act1, act2);
  ASSERT_EQ(act1, assgn.getTraitFor(&comp1));
  ASSERT_EQ(act2, assgn.getTraitFor(&comp2));
}

// _____________________________________________________________________________
TEST(IntentionAssignmentTest, toString) {
  // See same test but for NetworkStates -- this is analoguous.
  NetworkSetup setup(20);
  IntentionAssignment assgn(&setup);
  Component comp1;
  Component comp2;
  ComponentIntention act1(setup, IntentionType::IDLE, 0, nullptr);
  ComponentIntention act2(setup, IntentionType::LISTEN, 0, nullptr);

  setup.registerComponent(&comp1);
  setup.registerComponent(&comp2);
  assgn.setTraitFor(&comp1, act1);
  assgn.setTraitFor(&comp2, act2);

  // Assert at least 1 character per intention.
  ASSERT_GT(assgn.toString().size(), 1);
}

// _____________________________________________________________________________
TEST(IntentionAssignmentTest, toXML) {
  // See same test but for NetworkStates -- this is analoguous.
  NetworkSetup setup(20);
  Message msg;
  Component comp1;
  Component comp2;
  setup.registerComponent(&comp1);
  setup.registerComponent(&comp2);
  setup.registerMessage(&msg);

  IntentionAssignment assgn(&setup);
  ComponentIntention in1(setup, IntentionType::LISTEN, 0, nullptr);
  ComponentIntention in2(setup, IntentionType::SEND, 3, &msg);
  assgn.setTraitFor(&comp1, in1);
  assgn.setTraitFor(&comp2, in2);

  // Check the XML representation.
  std::vector<std::string> repr = assgn.toXML();
  ASSERT_EQ(15, repr.size());

  ASSERT_EQ("<entry>", repr[0]);
  ASSERT_EQ("  <for>default</for>", repr[1]);
  ASSERT_EQ("  <trait>", repr[2]);
  ASSERT_EQ("    <type>LST</type>", repr[3]);
  ASSERT_EQ("  </trait>", repr[4]);
  ASSERT_EQ("</entry>", repr[5]);
  ASSERT_EQ("<entry>", repr[6]);
  ASSERT_EQ("  <for>default</for>", repr[7]);
  ASSERT_EQ("  <trait>", repr[8]);
  ASSERT_EQ("    <type>SEND</type>", repr[9]);
  ASSERT_EQ("    <msg>", repr[10]);
  ASSERT_EQ("    </msg>", repr[11]);
  ASSERT_EQ("    <tic>3</tic>", repr[12]);
  ASSERT_EQ("  </trait>", repr[13]);
  ASSERT_EQ("</entry>", repr[14]);
}

// _____________________________________________________________________________
TEST(IntentionAssignmentDeathTest, canNotSetInvalid) {
  // See same test but for NetworkStates -- this is analoguous.
  NetworkSetup setup(20);
  IntentionAssignment assgn(&setup);
  Component comp;
  ComponentIntention act(setup, IntentionType::IDLE, 0, nullptr);

  ASSERT_DEATH(assgn.setTraitFor(&comp, act), "not a valid component for");
}

// _____________________________________________________________________________
TEST(IntentionAssignmentDeathTest, canNotOverride) {
  // See same test but for NetworkStates -- this is analoguous.
  NetworkSetup setup(20);
  IntentionAssignment assgn(&setup);
  Component comp;
  ComponentIntention act1(setup, IntentionType::IDLE, 0, nullptr);
  ComponentIntention act2(setup, IntentionType::LISTEN, 0, nullptr);
  setup.registerComponent(&comp);

  assgn.setTraitFor(&comp, act1);
  ASSERT_DEATH(assgn.setTraitFor(&comp, act2), "can not override");
}

// _____________________________________________________________________________
TEST(IntentionAssignmentTest, setTraitFor) {
  // See same test but for NetworkStates -- this is analoguous.
  NetworkSetup setup(20);
  IntentionAssignment assgn(&setup);
  Component comp1;
  Component comp2;
  ComponentIntention act1(setup, IntentionType::IDLE, 0, nullptr);
  ComponentIntention act2(setup, IntentionType::LISTEN, 0, nullptr);
  setup.registerComponent(&comp1);
  setup.registerComponent(&comp2);

  assgn.setTraitFor(&comp1, act1);
  assgn.setTraitFor(&comp2, act2);

  // These assertions not failing also implies that the assignment is no longer
  // partial.
  ASSERT_EQ(act1, assgn.getTraitFor(&comp1));
  ASSERT_EQ(act2, assgn.getTraitFor(&comp2));
}

// _____________________________________________________________________________
TEST(ANLViewDeathTest, getPreviousAction) {
  // Scenario: we test that getting the previous action fails if there is none
  //  and that getting it if there is one does not fail.
  // Why: fail: abnormal exit point in method. success: test of simple getter.
  NetworkSetup setup(20);
  Component comp;
  Message msg;
  setup.registerComponent(&comp);
  setup.registerMessage(&msg);

  ANLView av1(&setup, 0, &comp, nullptr);
  ASSERT_DEATH(av1.getPreviousAction(), "no previous");

  ComponentAction act(setup, ActionType::CANCELLED, 4, &msg);
  ANLView av2(&setup, 0, &comp, act, nullptr);
  ASSERT_EQ(act, av2.getPreviousAction());
}

// _____________________________________________________________________________
TEST(ANLViewTest, getSlotNumber) {
  // Scenario: we test that getting the slot number works.
  // Why: we chose 74 as a random value to test the getter getSlotNumber().
  NetworkSetup setup(20);
  Component comp;
  setup.registerComponent(&comp);

  ANLView av(&setup, 74, &comp, nullptr);
  ASSERT_EQ(74, av.getSlotNumber());
}

// _____________________________________________________________________________
TEST(ANLViewTest, idle) {
  // Scenario: we use the ANL view to let a component idle.
  // Why: one of the methods for the protocol designer to interact with the ANL.
  NetworkSetup setup(20);
  Component comp;
  setup.registerComponent(&comp);

  IntentionAssignment intent(&setup);
  ANLView av(&setup, 0, &comp, &intent);
  av.idle();
  ASSERT_EQ(IntentionType::IDLE, intent.getTraitFor(&comp).getType());
  ASSERT_EQ(0, intent.getTraitFor(&comp).getTic());
  ASSERT_EQ(nullptr, intent.getTraitFor(&comp).getMessage());
}

// _____________________________________________________________________________
TEST(ANLViewTest, listen) {
  // Scenario: we use the ANL view to let a component listen to the medium.
  // Why: one of the methods for the protocol designer to interact with the ANL.
  NetworkSetup setup(20);
  Component comp;
  setup.registerComponent(&comp);

  IntentionAssignment intent(&setup);
  ANLView av(&setup, 0, &comp, &intent);
  av.listen();
  ASSERT_EQ(IntentionType::LISTEN, intent.getTraitFor(&comp).getType());
  ASSERT_EQ(0, intent.getTraitFor(&comp).getTic());
  ASSERT_EQ(nullptr, intent.getTraitFor(&comp).getMessage());
}

// _____________________________________________________________________________
TEST(ANLViewTest, sendWithCS) {
  // Scenario: we use the ANL view to let a component send a message while using
  //  carrier sensing.
  // Why: we pick a message and a tic != 0 in order to ensure that the correct
  //  tic and message are stored in the intention.
  NetworkSetup setup(20);
  Component comp;
  Message msg;
  setup.registerComponent(&comp);
  setup.registerMessage(&msg);

  IntentionAssignment intent(&setup);
  ANLView av(&setup, 0, &comp, &intent);
  av.send(&msg, 4);
  ASSERT_EQ(IntentionType::SEND, intent.getTraitFor(&comp).getType());
  ASSERT_EQ(4, intent.getTraitFor(&comp).getTic());
  ASSERT_EQ(&msg, intent.getTraitFor(&comp).getMessage());
}

// _____________________________________________________________________________
TEST(ANLViewTest, sendWithoutCS) {
  // Scenario: we use the ANL view to let a component send a message while using
  //  carrier sensing.
  // Why: we pick a message and a tic != 0 in order to ensure that the correct
  //  tic and message are stored in the intention.
  NetworkSetup setup(20);
  Component comp;
  Message msg;
  setup.registerComponent(&comp);
  setup.registerMessage(&msg);

  IntentionAssignment intent(&setup);
  ANLView av(&setup, 0, &comp, &intent);
  av.send(&msg, 13, false);
  ASSERT_EQ(IntentionType::SEND_FORCE, intent.getTraitFor(&comp).getType());
  ASSERT_EQ(13, intent.getTraitFor(&comp).getTic());
  ASSERT_EQ(&msg, intent.getTraitFor(&comp).getMessage());
}

// _____________________________________________________________________________
TEST(ANLViewTest, hasActed) {
  // Scenario: we check the return value of the getter hasActed before and after
  //  using an intention method.
  // Why: we only use one intention method as the others are tested below for
  //  failing duplicate actions which uses the "has-acted" flag for which this
  //  method is a simple getter.
  NetworkSetup setup(20);
  Component comp;
  setup.registerComponent(&comp);

  IntentionAssignment intent(&setup);
  ANLView av(&setup, 0, &comp, &intent);

  ASSERT_FALSE(av.hasActed());
  av.listen();
  ASSERT_TRUE(av.hasActed());
}

// _____________________________________________________________________________
TEST(ANLViewDeathTest, duplicateActionFails) {
  // Scenario: we test another intention in the same slot, this fails.
  // Why: we chose every intention method as the first method in the failing
  //  sequence in order to ensure that failing does not depend on the first
  //  called intention method.
  NetworkSetup setup(20);
  Component comp;
  Message msg;
  setup.registerComponent(&comp);
  setup.registerMessage(&msg);

  IntentionAssignment intent1(&setup);
  ANLView av1(&setup, 0, &comp, &intent1);
  av1.idle();
  ASSERT_DEATH(av1.idle(), "already acted");

  IntentionAssignment intent2(&setup);
  ANLView av2(&setup, 0, &comp, &intent2);
  av2.idle();
  ASSERT_DEATH(av2.listen(), "already acted");

  IntentionAssignment intent3(&setup);
  ANLView av3(&setup, 0, &comp, &intent3);
  av3.idle();
  ASSERT_DEATH(av3.send(&msg, 7, false), "already acted");

  IntentionAssignment intent4(&setup);
  ANLView av4(&setup, 4, &comp, &intent4);
  av4.listen();
  ASSERT_DEATH(av4.idle(), "already acted");

  IntentionAssignment intent5(&setup);
  ANLView av5(&setup, 2, &comp, &intent5);
  av5.send(&msg, 3, false);
  ASSERT_DEATH(av5.listen(), "already acted");
}

// _____________________________________________________________________________
TEST(StateMachineComponentTest, initialState) {
  // Scenario: we supply an initial state which is correctly set as the state of
  //  the component.
  // Why: two different states to make coincidence less likely.
  StateMachineComponent<int> smc1(4);
  ASSERT_EQ(4, smc1.getState());

  StateMachineComponent<int> smc2(-34);
  ASSERT_EQ(-34, smc2.getState());
}

// _____________________________________________________________________________
TEST(StateMachineComponentTest, noopTransition) {
  // Scenario: we ensure that the default transition is a no-op.
  // Why: testing default method behavior.
  StateMachineComponent<int> smc(0);
  ASSERT_EQ(0, smc.getState());
  smc.onAct(nullptr);  // The view is not used.
  ASSERT_EQ(0, smc.getState());
}
