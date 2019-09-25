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
#include "anl/core/errortrace.h"

// We do this here as a test can under no circumstance pollute the namespace as
//  it is never included.
using namespace Core;  // NOLINT

// _____________________________________________________________________________
TEST(ErrorTracerDeathTest, invalidSection) {
  // Scenario: entering a section identified by an empty name fails.
  // Why: abnormal exit point from method.
  ErrorTracer et;

  ASSERT_DEATH(et.enter(""), "empty section name");
}

// _____________________________________________________________________________
TEST(ErrorTracerDeathTest, invalidMessage) {
  // Scenario: requiring an expression with an invalid message fails for both
  //  possible expression values (true/false).
  // Why: abnormal exit point from method.
  ErrorTracer et;

  ASSERT_DEATH(et.require(false, nullptr), "invalid message");

  ASSERT_DEATH(et.require(true, nullptr), "invalid message");
}

// _____________________________________________________________________________
TEST(ErrorTracerDeathTest, leaveEmptyStackFails) {
  // Scenario: "leave()"-ing an empty section stack fails.
  // Why: abnormal exit point from method.
  ErrorTracer et;

  ASSERT_DEATH(et.leave(), "empty section stack");
}

// _____________________________________________________________________________
TEST(ErrorTracerDeathTest, requireFailOnEmptyStack) {
  // Scenario: failing a required expression on the empty stack does work.
  // Why: corner case for section stacks. message is unique to avoid having it
  //  occur by chance.
  ErrorTracer et;

  ASSERT_DEATH(et.require(false, "test_123"), "test_123");
}

// _____________________________________________________________________________
TEST(ErrorTracerDeathTest, requireFailOnSingleStack) {
  // Scenario: failing a required expression on a section stack with only one
  //  element works.
  // Why: corner case for section stacks. message is unique to avoid having it
  //  occur by chance.
  ErrorTracer et;

  // Check both failure message and section name.
  et.enter("abc_789");
  ASSERT_DEATH(et.require(false, "test_456"), "test_456");
  ASSERT_DEATH(et.require(false, "test_456"), "abc_789");

  // Now do the same but leave the section and enter another one.
  et.leave();
  et.enter("xyz_42");
  ASSERT_DEATH(et.require(false, "test_396"), "test_396");
  ASSERT_DEATH(et.require(false, "test_396"), "xyz_42");
}

// _____________________________________________________________________________
TEST(ErrorTracerDeathTest, requireFailOnMultiStack) {
  // Scenario: failing a required expression on a section stack with multiple
  //  elements works.
  // Why: regular case for section stacks. message is unique to avoid having it
  //  occur by chance.
  ErrorTracer et;

  // First situation: Section X.Y (direct entry).
  et.enter("x_25");
  et.enter("y_03");
  ASSERT_DEATH(et.require(false, "z_123"), "z_123");
  ASSERT_DEATH(et.require(false, "z_123"), "x_25");
  ASSERT_DEATH(et.require(false, "z_123"), "y_03");

  // Second situation: Section X.Z (section X still on stack).
  et.leave();
  et.enter("z_2207");
  ASSERT_DEATH(et.require(false, "q_08"), "q_08");
  ASSERT_DEATH(et.require(false, "q_08"), "x_25");
  ASSERT_DEATH(et.require(false, "q_08"), "z_2207");

  // Third situation: Section U.W (going over empty stack).
  et.leave();
  et.leave();
  et.enter("u_23");
  et.enter("w_24");
  ASSERT_DEATH(et.require(false, "a_06"), "a_06");
  ASSERT_DEATH(et.require(false, "a_06"), "u_23");
  ASSERT_DEATH(et.require(false, "a_06"), "w_24");
}
