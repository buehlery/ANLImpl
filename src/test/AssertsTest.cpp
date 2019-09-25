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
#include "anl/misc/asserts.h"

// _____________________________________________________________________________
TEST(AssertsDeathTest, failedAssertExitsProgram) {
  // Scenario: failed assertions exit the program.
  // Why: intended behavior of assertions.
  ASSERT_DEATH(Misc::Asserts::require(false, "fails!"), "Assertion failed*");
}

// _____________________________________________________________________________
TEST(AssertTest, successfulAssertDoesNotExitProgram) {
  // Scenario: successful assertions do not exit the program.
  // Why: intended behavior of assertions.
  Misc::Asserts::require(true, "runs!");
  // We have no assertion for ensuring the program still works...
  // We need to rely on the developer running the tests to see that the test
  // program has terminated abnormally.
}

// _____________________________________________________________________________
TEST(ExpectTest, failedExpectationDoesNotExitProgram) {
  // Scenario: unsuccessful expectations do not exit the program.
  // Why: intended behavior of expectations.
  Misc::Asserts::expect(false, "expected! ;-)");
  // (Same argument as above)
}

// _____________________________________________________________________________
TEST(ExpectTest, successfulExpectationDoesNotExitProgram) {
  // Scenario: successful expectations do not exit the program.
  // Why: intended behavior of expectations.
  Misc::Asserts::expect(true, "runs!");
  // (Same argument as above)
}
