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
#include "anl/core/anl.h"
#include "anl/core/anl_algorithm.h"

// We do this here as a test can under no circumstance pollute the namespace as
// it is never included.
using namespace Core;  // NOLINT

// _____________________________________________________________________________
TEST(ANLFilterNothingTest, emptyStaysEmpty) {
  // Scenario: the canonical filter leaves empty vectors empty.
  // Why: filters should never add entries to the vector.
  NetworkSetup setup(20);
  std::vector<ComponentAction> actions;

  // Ensure the vector is empty before and after.
  ASSERT_EQ(0, actions.size());
  ANLFilterNothing(setup, &actions);
  ASSERT_EQ(0, actions.size());
}

// _____________________________________________________________________________
TEST(ANLFilterNothingTest, idleStays) {
  // Scenario: the canonical filter does not remove idling from the vector.
  // Why: the canonical filter leaves vectors untouched.
  NetworkSetup setup(20);

  std::vector<ComponentAction> actions;
  actions.emplace_back(setup, ActionType::IDLE, 0, nullptr);

  ANLFilterNothing(setup, &actions);

  // Ensure the filter leaves vectors untouched.
  ASSERT_EQ(1, actions.size());
  ASSERT_EQ(ActionType::IDLE, actions[0].getType());
  ASSERT_EQ(0, actions[0].getTic());
  ASSERT_EQ(nullptr, actions[0].getMessage());
}

// _____________________________________________________________________________
TEST(ANLFilterNothingTest, collisionStays) {
  // Scenario: the canonical filter does not remove collisions from the vector.
  // Why: the canonical filter leaves vectors untouched.
  NetworkSetup setup(20);

  std::vector<ComponentAction> actions;
  actions.emplace_back(setup, ActionType::COLLISION, 0, nullptr);

  ANLFilterNothing(setup, &actions);

  // Ensure the filter leaves vectors untouched.
  ASSERT_EQ(1, actions.size());
  ASSERT_EQ(ActionType::COLLISION, actions[0].getType());
  ASSERT_EQ(0, actions[0].getTic());
  ASSERT_EQ(nullptr, actions[0].getMessage());
}

// _____________________________________________________________________________
TEST(ANLFilterNothingTest, silenceStays) {
  // Scenario: the canonical filter does not remove silence from the vector.
  // Why: the canonical filter leaves vectors untouched.
  NetworkSetup setup(20);

  std::vector<ComponentAction> actions;
  actions.emplace_back(setup, ActionType::SILENCE, 0, nullptr);

  ANLFilterNothing(setup, &actions);

  // Ensure the filter leaves vectors untouched.
  ASSERT_EQ(1, actions.size());
  ASSERT_EQ(ActionType::SILENCE, actions[0].getType());
  ASSERT_EQ(0, actions[0].getTic());
  ASSERT_EQ(nullptr, actions[0].getMessage());
}

// _____________________________________________________________________________
TEST(ANLFilterNothingTest, sentStays) {
  // Scenario: the canonical filter does not remove sending from the vector.
  // Why: the canonical filter leaves vectors untouched.
  NetworkSetup setup(20);
  Message msg;
  setup.registerMessage(&msg);

  std::vector<ComponentAction> actions;
  actions.emplace_back(setup, ActionType::SENT, 5, &msg);

  ANLFilterNothing(setup, &actions);

  // Ensure the filter leaves vectors untouched.
  ASSERT_EQ(1, actions.size());
  ASSERT_EQ(ActionType::SENT, actions[0].getType());
  ASSERT_EQ(5, actions[0].getTic());
  ASSERT_EQ(&msg, actions[0].getMessage());
}

// _____________________________________________________________________________
TEST(ANLFilterNothingTest, cancelledStays) {
  // Scenario: the canonical filter does not remove cancelled messages from the
  //  vector.
  // Why: the canonical filter leaves vectors untouched.
  NetworkSetup setup(20);
  Message msg;
  setup.registerMessage(&msg);

  std::vector<ComponentAction> actions;
  actions.emplace_back(setup, ActionType::CANCELLED, 5, &msg);

  ANLFilterNothing(setup, &actions);

  // Ensure the filter leaves vectors untouched.
  ASSERT_EQ(1, actions.size());
  ASSERT_EQ(ActionType::CANCELLED, actions[0].getType());
  ASSERT_EQ(5, actions[0].getTic());
  ASSERT_EQ(&msg, actions[0].getMessage());
}

// _____________________________________________________________________________
TEST(ANLFilterNothingTest, singleReceivedStays) {
  // Scenario: the canonical filter does not remove a received message from the
  //  vector.
  // Why: the canonical filter leaves vectors untouched.
  NetworkSetup setup(20);
  Message msg;
  setup.registerMessage(&msg);

  std::vector<ComponentAction> actions;
  actions.emplace_back(setup, ActionType::RECEIVED, 5, &msg);

  ANLFilterNothing(setup, &actions);

  // Ensure the filter leaves vectors untouched.
  ASSERT_EQ(1, actions.size());
  ASSERT_EQ(ActionType::RECEIVED, actions[0].getType());
  ASSERT_EQ(5, actions[0].getTic());
  ASSERT_EQ(&msg, actions[0].getMessage());
}

// _____________________________________________________________________________
TEST(ANLFilterNothingTest, multiReceivedStays) {
  // Scenario: the canonical filter does not remove multiple received messages
  //  from the vector.
  // Why: the canonical filter leaves vectors untouched.
  NetworkSetup setup(20);
  Message msg1;
  Message msg2;
  setup.registerMessage(&msg1);
  setup.registerMessage(&msg2);

  std::vector<ComponentAction> actions;
  actions.emplace_back(setup, ActionType::RECEIVED, 5, &msg1);
  actions.emplace_back(setup, ActionType::RECEIVED, 3, &msg2);

  ANLFilterNothing(setup, &actions);

  // Ensure the filter leaves vectors untouched.
  ASSERT_EQ(2, actions.size());
  ASSERT_EQ(ActionType::RECEIVED, actions[0].getType());
  ASSERT_EQ(5, actions[0].getTic());
  ASSERT_EQ(&msg1, actions[0].getMessage());
  ASSERT_EQ(ActionType::RECEIVED, actions[1].getType());
  ASSERT_EQ(3, actions[1].getTic());
  ASSERT_EQ(&msg2, actions[1].getMessage());
}

// _____________________________________________________________________________
TEST(ANLFilterNothingTest, multiReceivedWithSilenceCollisionStays) {
  // Scenario: the canonical filter does not remove multiple received messages
  //  including a collision and silence from the vector.
  // Why: the canonical filter leaves vectors untouched.
  NetworkSetup setup(20);
  Message msg1;
  Message msg2;
  setup.registerMessage(&msg1);
  setup.registerMessage(&msg2);

  std::vector<ComponentAction> actions;
  actions.emplace_back(setup, ActionType::RECEIVED, 5, &msg1);
  actions.emplace_back(setup, ActionType::RECEIVED, 3, &msg2);
  actions.emplace_back(setup, ActionType::COLLISION, 0, nullptr);
  actions.emplace_back(setup, ActionType::SILENCE, 0, nullptr);

  ANLFilterNothing(setup, &actions);

  // Ensure the filter leaves vectors untouched.
  ASSERT_EQ(4, actions.size());
  ASSERT_EQ(ActionType::RECEIVED, actions[0].getType());
  ASSERT_EQ(5, actions[0].getTic());
  ASSERT_EQ(&msg1, actions[0].getMessage());
  ASSERT_EQ(ActionType::RECEIVED, actions[1].getType());
  ASSERT_EQ(3, actions[1].getTic());
  ASSERT_EQ(&msg2, actions[1].getMessage());
  ASSERT_EQ(ActionType::COLLISION, actions[2].getType());
  ASSERT_EQ(0, actions[2].getTic());
  ASSERT_EQ(nullptr, actions[2].getMessage());
  ASSERT_EQ(ActionType::SILENCE, actions[3].getType());
  ASSERT_EQ(0, actions[3].getTic());
  ASSERT_EQ(nullptr, actions[3].getMessage());
}

// _____________________________________________________________________________
TEST(ANLFilterNaiveTest, emptyStaysEmpty) {
  // Scenario: the naive filter does not add entries to the empty vector.
  // Why: filters are not supposed to add new entries to vectors.
  NetworkSetup setup(20);
  std::vector<ComponentAction> actions;

  // Check the size before and after filtering.
  ASSERT_EQ(0, actions.size());
  ANLFilterNaive(setup, &actions);
  ASSERT_EQ(0, actions.size());
}

// _____________________________________________________________________________
TEST(ANLFilterNaiveTest, idleStays) {
  // Scenario: the naive filter does not remove idling from the vector.
  // Why: idling only has one possible result.
  NetworkSetup setup(20);

  std::vector<ComponentAction> actions;
  actions.emplace_back(setup, ActionType::IDLE, 0, nullptr);

  ANLFilterNaive(setup, &actions);

  // Ensure that the filter does not remove idling from the vector.
  ASSERT_EQ(1, actions.size());
  ASSERT_EQ(ActionType::IDLE, actions[0].getType());
  ASSERT_EQ(0, actions[0].getTic());
  ASSERT_EQ(nullptr, actions[0].getMessage());
}

// _____________________________________________________________________________
TEST(ANLFilterNaiveTest, sentStays) {
  // Scenario: the naive filter does not remove sending from the vector if the
  //  sender is in the sender set.
  // Why: sending only has one possible result in the case that the sender is
  //  in the sender set.
  NetworkSetup setup(20);
  Message msg;
  setup.registerMessage(&msg);

  std::vector<ComponentAction> actions;
  actions.emplace_back(setup, ActionType::SENT, 5, &msg);

  ANLFilterNaive(setup, &actions);

  // Ensure the filter does not remove sending from the vector.
  ASSERT_EQ(1, actions.size());
  ASSERT_EQ(ActionType::SENT, actions[0].getType());
  ASSERT_EQ(5, actions[0].getTic());
  ASSERT_EQ(&msg, actions[0].getMessage());
}

// _____________________________________________________________________________
TEST(ANLFilterNaiveTest, cancelledStays) {
  // Scenario: the naive filter does not remove cancelling transmissions from
  //  the vector if the sender is not in the sender set.
  // Why: sending only has one possible result in the case that the sender is
  //  not in the sender set.
  NetworkSetup setup(20);
  Message msg;
  setup.registerMessage(&msg);

  std::vector<ComponentAction> actions;
  actions.emplace_back(setup, ActionType::CANCELLED, 5, &msg);

  ANLFilterNaive(setup, &actions);

  // Ensure the filter does not remove the cancelled transmission from the
  //  vector.
  ASSERT_EQ(1, actions.size());
  ASSERT_EQ(ActionType::CANCELLED, actions[0].getType());
  ASSERT_EQ(5, actions[0].getTic());
  ASSERT_EQ(&msg, actions[0].getMessage());
}

// _____________________________________________________________________________
TEST(ANLFilterNaiveTest, noSenderResultsInSilence) {
  // Scenario: the naive filter only allows silence when no component is sending
  //  but a component is listening.
  // Why: this is how the naive semantics work.
  NetworkSetup setup(20);

  std::vector<ComponentAction> actions;
  actions.emplace_back(setup, ActionType::SILENCE, 0, nullptr);

  ANLFilterNaive(setup, &actions);

  // Ensure that only silence is possible.
  ASSERT_EQ(1, actions.size());
  ASSERT_EQ(ActionType::SILENCE, actions[0].getType());
  ASSERT_EQ(0, actions[0].getTic());
  ASSERT_EQ(nullptr, actions[0].getMessage());
}

// _____________________________________________________________________________
TEST(ANLFilterNaiveTest, multiSenderResultsInCollision) {
  // Scenario: the naive filter only allows collisions when multiple components
  //  are sending while a component is listening.
  // Why: this is how the naive semantics work.
  NetworkSetup setup(20);
  Message msg1;
  Message msg2;
  setup.registerMessage(&msg1);
  setup.registerMessage(&msg2);

  std::vector<ComponentAction> actions;
  actions.emplace_back(setup, ActionType::COLLISION, 0, nullptr);
  actions.emplace_back(setup, ActionType::RECEIVED, 6, &msg1);
  actions.emplace_back(setup, ActionType::RECEIVED, 2, &msg2);

  ANLFilterNaive(setup, &actions);

  // Ensure that only a collision is possible.
  ASSERT_EQ(1, actions.size());
  ASSERT_EQ(ActionType::COLLISION, actions[0].getType());
  ASSERT_EQ(0, actions[0].getTic());
  ASSERT_EQ(nullptr, actions[0].getMessage());

  // We will not exhaustively test all orders, but we will test three random
  //  ones.
  actions.clear();
  actions.emplace_back(setup, ActionType::RECEIVED, 6, &msg1);
  actions.emplace_back(setup, ActionType::COLLISION, 0, nullptr);
  actions.emplace_back(setup, ActionType::RECEIVED, 2, &msg2);

  ANLFilterNaive(setup, &actions);

  // Ensure that only a collision is possible.
  ASSERT_EQ(1, actions.size());
  ASSERT_EQ(ActionType::COLLISION, actions[0].getType());
  ASSERT_EQ(0, actions[0].getTic());
  ASSERT_EQ(nullptr, actions[0].getMessage());

  actions.clear();
  actions.emplace_back(setup, ActionType::RECEIVED, 2, &msg2);
  actions.emplace_back(setup, ActionType::RECEIVED, 6, &msg1);
  actions.emplace_back(setup, ActionType::COLLISION, 0, nullptr);

  ANLFilterNaive(setup, &actions);

  // Ensure that only a collision is possible.
  ASSERT_EQ(1, actions.size());
  ASSERT_EQ(ActionType::COLLISION, actions[0].getType());
  ASSERT_EQ(0, actions[0].getTic());
  ASSERT_EQ(nullptr, actions[0].getMessage());
}

// _____________________________________________________________________________
TEST(ANLFilterNaiveTest, oneSenderResultsInSuccess) {
  // Scenario: one component is sending, one component is listening. this leads
  //  to a successful transmission using the naive filter function.
  // Why: this is how naive semantics work.
  NetworkSetup setup(20);
  Message msg;
  setup.registerMessage(&msg);

  std::vector<ComponentAction> actions;
  actions.emplace_back(setup, ActionType::COLLISION, 0, nullptr);
  actions.emplace_back(setup, ActionType::RECEIVED, 4, &msg);

  ANLFilterNaive(setup, &actions);

  // Ensure that the transmission is successful.
  ASSERT_EQ(1, actions.size());
  ASSERT_EQ(ActionType::RECEIVED, actions[0].getType());
  ASSERT_EQ(4, actions[0].getTic());
  ASSERT_EQ(&msg, actions[0].getMessage());

  // Next order.
  actions.clear();
  actions.emplace_back(setup, ActionType::RECEIVED, 4, &msg);
  actions.emplace_back(setup, ActionType::COLLISION, 0, nullptr);

  ANLFilterNaive(setup, &actions);

  // Ensure that the transmission is successful.
  ASSERT_EQ(1, actions.size());
  ASSERT_EQ(ActionType::RECEIVED, actions[0].getType());
  ASSERT_EQ(4, actions[0].getTic());
  ASSERT_EQ(&msg, actions[0].getMessage());
}
