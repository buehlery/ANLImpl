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

#ifndef ANL_CORE_TOPOLOGIES_H_
#define ANL_CORE_TOPOLOGIES_H_

#include <unordered_map>
#include <unordered_set>
#include "anl/core/types.h"

// This file contains the default topologies in the CORE module.
namespace Core {


// A network topology in which everyone can reach everyone else.
class TrivialNetworkTopology final : public NetworkTopology {
 private:
  // Overriding the internal canReach mechanism.
  bool doCanReach(const Component* sndr, const Component* rcvr) const override
    { return true; }
};


// A network topology in which no component can reach anyone else.
class IsolatedNetworkTopology final : public NetworkTopology {
 private:
  // Overriding the internal canReach mechanism.
  bool doCanReach(const Component* sndr, const Component* rcvr) const override
    { return false; }
};


// A network topology that can be easily modified programmatically.
class ExplicitNetworkTopology : public NetworkTopology {
 public:
  // Adds an edge from the first component to the second.
  void addEdge(const Component* from, const Component* to);

 protected:
  // Overriding the internal canReach mechanism.
  bool doCanReach(const Component* sndr, const Component* rcvr) const override;

 private:
  // The storage in which the edges of the network topology are kept.
  std::unordered_map<const Component*,
    std::unordered_set<const Component*>> mStorage;
};


}  // namespace Core

#endif  // ANL_CORE_TOPOLOGIES_H_
