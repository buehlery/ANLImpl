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

#ifndef ANL_CORE_TYPES_H_
#define ANL_CORE_TYPES_H_

#include <string>
#include <vector>

// This file contains the types for the CORE module.
namespace Core {


// See anl.h for full declaration.
class ANLView;


// A component in a network.
class Component {
 public:
  virtual ~Component() {}

  // Lets the component act by invoking its protocol callback. The given view of
  //  the ANL is centered around this component and may be used to express
  //  component intentions.
  void onAct(ANLView* view);

  // Converts the component into a representation of XML tags. Each element of
  //  the vector will be one line of the XML output.
  std::vector<std::string> toXML() const { return doToXML(); }

  // Fetches an ID of the component. Must be unique if proper XML support is
  //  desired.
  std::string getId() const { return doGetId(); }

  // Operators.
  bool operator==(const Component& other) const { return equals(other); }

 private:
  // Compares two components for equality. Derived classes may opt to not use
  // the value returned by the default implementation.
  virtual bool equals(const Component& other) const;

  // The protocol callback of the component.
  virtual void doAct(ANLView* view);

  // Converts the component into a representation of XML tags.
  virtual std::vector<std::string> doToXML() const
    { return std::vector<std::string>(); }

  // Fetches an ID of the component. Must be unique if proper XML support is
  //  desired.
  virtual std::string doGetId() const { return "default"; }
};


// A message that can be exchanged between components.
class Message {
 public:
  virtual ~Message() {}

  // Converts the message into a textual representation.
  std::string toString() const { return doToString(); }

  // Converts the message into a representation of XML tags. Each element of
  //  the vector will be one line of the XML output.
  std::vector<std::string> toXML() const { return doToXML(); }

  // Operators.
  bool operator==(const Message& other) const { return equals(other); }

 private:
  // Compares two messages for equality. Derived classes may opt to not use the
  // value returned by the default implementation.
  virtual bool equals(const Message& other) const;

  // Converts the message into a textual representation.
  virtual std::string doToString() const { return "Message"; }

  // Converts the message into a representation of XML tags.
  virtual std::vector<std::string> doToXML() const
    { return std::vector<std::string>(); }
};


// A network topology.
class NetworkTopology {
 public:
  virtual ~NetworkTopology() {}

  // Test for whether a component can reach another component.
  bool canReach(const Component* sndr, const Component* rcvr) const;

 private:
  // Virtual delegate for checking whether a component can reach another
  // component.
  virtual bool doCanReach(const Component* sndr, const Component* rcvr)
    const = 0;
};


}  // namespace Core

#endif  // ANL_CORE_TYPES_H_
