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

#ifndef ANL_CORE_ANL_H_
#define ANL_CORE_ANL_H_

#include <cstddef>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "anl/core/types.h"

// This file contains the ANL from the CORE module.
namespace Core {


// * Important notice: The classes here implement the ANL defined in the report.
// *  We opted for an implementation that is closer to the formal definition in
// *  order to be easier to understand for developers knowing the report.


// A network setup (see report).
class NetworkSetup {
 public:
  // Constructor. Sets the number of tics per slot. Must be greater than zero.
  explicit NetworkSetup(std::size_t ticsPerSlot);

  // Registers a message with the network setup. This should be done once per
  //  message directly after creating the network setup.
  void registerMessage(const Message* msg);

  // Registers a component with the network setup. This must be done once per
  //  component directly after creating the network setup.
  void registerComponent(Component* comp);

  // Checks whether or not a message is registered in this network setup.
  bool isMessage(const Message* msg) const;

  // Checks whether or not a component is registered in this network setup.
  bool isComponent(const Component& comp) const;

  // Executes the given function for all components in this setup. The order of
  //  the components is guaranteed to be the registration order.
  void forEachComponent(std::function<void(Component*)>) const;

  // Gets the number of components in the setup.
  std::size_t getComponentCount() const { return mComponents.size(); }

  // Gets the number of tics per slot.
  std::size_t getTicsPerSlot() const { return mTicsPerSlot; }

 private:
  // The number of tics per slot.
  std::size_t mTicsPerSlot;

  // The messages that are recognized by the network.
  std::unordered_set<const Message*> mMessages;

  // The components that the network consists of.
  std::vector<Component*> mComponents;
};


// The different kinds of component actions.
enum class ActionType {
  // Represents a component that idled.
  IDLE,

  // Represents a component that listened to a medium but received silence.
  SILENCE,

  // Represents a component that listened to a medium but received a collision.
  COLLISION,

  // Represents a component that listened to a medium and received a message.
  //  Requires a message and a tic.
  RECEIVED,

  // Represents a component that sent a message.
  //  Requires a message and a tic.
  SENT,

  // Represents a component that attempted to send a message but cancelled the
  // message due to carrier sensing.
  //  Requires a message and a tic.
  CANCELLED
};


// The different kinds of component intentions.
enum class IntentionType {
  // The component intends to idle.
  IDLE,

  // The component intends to listen to the medium.
  LISTEN,

  // The component intends to send a message using carrier sensing.
  //  Requires a message and a tic.
  SEND,

  // The component intends to send a message without carrier sensing.
  //  Requires a message and a tic.
  SEND_FORCE
};


// A trait of a component that is message- and tic-annotated.
// The first template parameter is an enum that specifies the valid subtypes of
//  the respective trait.
template<class T>
class ComponentTrait {
 public:
  // Constructor. All values must be valid for the given type and network setup.
  //  That is, only required data is given (0 / nullptr otherwise). Also, only
  //  tics in the correct range may be specified. Furthermore, invalid messages
  //  (that is, not added to the network setup) SHOULD not be passed.
  ComponentTrait(const NetworkSetup& setup, T type, std::size_t tic,
    const Message* message);

  // Creates a string that represents this component trait textually.
  std::string toString() const;

  // Creates an XML representation of this component trait.
  std::vector<std::string> toXML() const;

  // Getters.
  T getType() const { return mType; }
  std::size_t getTic() const { return mTic; }
  const Message* getMessage() const { return mMessage; }

  // Operators.
  bool operator==(const ComponentTrait<T>& other) const;
  bool operator!=(const ComponentTrait<T>& other) const
    { return !(*this == other); }

 private:
  // The type of this component trait.
  T mType;

  // The tic this trait occurred in. If there is no tic associated with the
  // type, this field contains the value 0.
  std::size_t mTic;

  // The message that is associated with this trait. If there is no message
  // associated with this trait, this field contains the value nullptr.
  const Message* mMessage;
};


// A trait mapping, mapping components to traits.
template<class T>
class TraitMapping {
 public:
  // Constructor.
  explicit TraitMapping(const NetworkSetup* setup);

  // Retrieves the trait for the given component in this mapping.
  //  The reference is valid for as long as the trait mapping is valid.
  const ComponentTrait<T>& getTraitFor(const Component* comp) const;

  // Sets the trait for a component. It is illegal to overwrite traits of
  //  components.
  void setTraitFor(const Component* comp, const ComponentTrait<T>& trait);

  // Creates a string that represents this component trait mapping textually.
  std::string toString() const;

  // Creates an XML representation of this component trait mapping.
  std::vector<std::string> toXML() const;

  // Checks whether this mapping is partial or not.
  bool isPartial() const { return mPartial; }

 private:
  // The underlying network setup.
  const NetworkSetup* mSetup;

  // A mapping Component -> ComponentTrait that is the 'actual' trait mapping.
  std::unordered_map<const Component*, ComponentTrait<T>> mMapping;

  // Whether or not the mapping is partial. Used for checking invariants.
  bool mPartial;
};


// Aliases for component actions, component intentions, network states, and
//  intention assignments.
using ComponentAction = ComponentTrait<ActionType>;
using ComponentIntention = ComponentTrait<IntentionType>;
using NetworkState = TraitMapping<ActionType>;
using IntentionAssignment = TraitMapping<IntentionType>;


// Different types of ANL semantics. All semantics produce subsets of the
//  network states produced by the CANONICAL semantic, which is the semantic
//  described in the report.
enum class ANLSemantics {
  // The canonical semantics from the report.
  CANONICAL,

  // The naive semantics is deterministic. Non-determinism (from LISTEN) is
  //  resolved using the following rules:
  //  1. If no neighbor is sending, receive SILENCE.
  //  2. If exactly one neighbor is sending, receive the message.
  //  3. If more than one neighbor is sending, receive COLLISION.
  NAIVE
};


// The class that provides the ANL -- the transition function \psi.
class ANL {
 public:
  // Constructor.
  explicit ANL(const NetworkSetup* setup, ANLSemantics semantics);

  // This method provides \psi.
  std::vector<NetworkState> transition(const NetworkTopology* topo,
    const IntentionAssignment* intent) const;

  // This method simulates the protocol execution of one slot in the ANL. The
  //  intentions of the components are stored in the given IntentionAssignment.
  //  The previous state is used to inform components of their previous
  //  component actions. If there is no previous state, nullptr must be passed.
  void runSlot(std::size_t slot, const NetworkState* prevState,
    IntentionAssignment* targetIntent);

 private:
  // The underlying network setup.
  const NetworkSetup* mSetup;

  // The ANL semantics that are used.
  const ANLSemantics mSemantics;
};


// The time-annotated component-centric view of the ANL that is exposed to the
//  protocol designer.
class ANLView {
 public:
  // Constructors.
  ANLView(const NetworkSetup* setup, std::size_t slot, const Component* comp,
    IntentionAssignment* targetIntent);
  ANLView(const NetworkSetup* setup, std::size_t slot, const Component* comp,
    const ComponentAction& prev, IntentionAssignment* targetIntent);

  // This causes the component to idle in the associated slot.
  void idle();

  // This causes the component to attempt to send the given message in the given
  //  tic of the associated slot. If carrier sensing is turned on, carrier
  //  sensing will be performed prior to sending.
  void send(const Message* msg, std::size_t tic, bool carrierSensing = true);

  // This causes the component to listen to the medium in the associated slot.
  void listen();

  // This retrieves the previous action of the associated component as a
  //  component action. May not be called if there is no such action.
  ComponentAction getPreviousAction() const;

  // Checks whether or not there is a previous actioon for the associated
  //  component.
  bool hasPreviousAction() const { return mHasPreviousAction; }

  // This retrieves the number of the associated slot.
  std::size_t getSlotNumber() const { return mSlot; }

  // Checks whether or not the component has already acted.
  bool hasActed() const { return mActed; }

  // Adds a message to the protocol log (not included in the output, but printed
  //  on stderr).
  void logProtocol(const std::string& msg) const;

 private:
  // The underlying network setup.
  const NetworkSetup* mSetup;

  // The slot this view takes place in.
  std::size_t mSlot;

  // The component this view is centered on.
  const Component* mComponent;

  // The component action of this component in the previous slot.
  ComponentAction mPreviousAction;

  // Whether or not there is a component action from the previous slot.
  bool mHasPreviousAction;

  // The intention assignment that is modified through this view.
  IntentionAssignment* mTargetIntent;

  // Whether this component has already acted in the associated slot.
  bool mActed;
};


}  // namespace Core

#endif  // ANL_CORE_ANL_H_
