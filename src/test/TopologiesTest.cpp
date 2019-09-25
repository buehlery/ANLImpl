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
#include "anl/core/topologies.h"

// We do this here as a test can under no circumstance pollute the namespace as
// it is never included.
using namespace Core;  // NOLINT

// _____________________________________________________________________________
TEST(TrivialNetworkTopologyTest, canReach) {
  // Scenario: each component can reach each other component in the trivial
  //  network topology. we test this with 10 components.
  // Why: 10 is a common amount of components.
  TrivialNetworkTopology tnt;

  // Test that every component can reach every other component.
  Component comps[10];
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      ASSERT_TRUE(tnt.canReach(&comps[i], &comps[j]));
    }
  }
}

// _____________________________________________________________________________
TEST(IsolatedNetworkTopologyTest, canReach) {
  // Scenario: no component can reach any other component in the isolated
  //  network topology. we test this with 10 components.
  // Why: 10 is a common amount of components.
  IsolatedNetworkTopology into;

  // Test that no component can reach any other component.
  Component comps[10];
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      ASSERT_FALSE(into.canReach(&comps[i], &comps[j]));
    }
  }
}

// _____________________________________________________________________________
TEST(ExplicitNetworkTopology, canReach) {
  // Scenario: we have two components and connect them in every possible way.
  //  we test each such connection configuration.
  // Why: non-trivial (more than 1) case.
  Component comps[2];
  for (int i = 0; i < 16; i++) {
    // Determine the connections: bit=1: connected, bit=0: unconnected.
    bool c00 = (i & 0x1) == 0x1;
    bool c01 = (i & 0x2) == 0x2;
    bool c10 = (i & 0x4) == 0x4;
    bool c11 = (i & 0x8) == 0x8;

    ExplicitNetworkTopology ent;
    if (c00) {
      ent.addEdge(&comps[0], &comps[0]);
    }
    if (c01) {
      ent.addEdge(&comps[0], &comps[1]);
    }
    if (c10) {
      ent.addEdge(&comps[1], &comps[0]);
    }
    if (c11) {
      ent.addEdge(&comps[1], &comps[1]);
    }

    ASSERT_EQ(c00, ent.canReach(&comps[0], &comps[0]));
    ASSERT_EQ(c01, ent.canReach(&comps[0], &comps[1]));
    ASSERT_EQ(c10, ent.canReach(&comps[1], &comps[0]));
    ASSERT_EQ(c11, ent.canReach(&comps[1], &comps[1]));
  }
}
