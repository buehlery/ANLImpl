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

#include "anl/core/anl.h"
#include <cstdio>
#include <sstream>
#include "anl/core/anl_algorithm.h"
#include "anl/misc/asserts.h"

using std::size_t;

// This file contains the ANL from the CORE module.
namespace Core {


// _____________________________________________________________________________
static std::string getSymbolForType(ActionType type) {
  switch (type) {
    case ActionType::IDLE: return "IDL";
    case ActionType::SILENCE: return "SIL";
    case ActionType::COLLISION: return "COL";
    case ActionType::RECEIVED: return "RCVD";
    case ActionType::SENT: return "SENT";
    case ActionType::CANCELLED: return "CCLD";
    default: Misc::Asserts::require(false, "invalid enum value"); break;
  }
  return "";
}

// _____________________________________________________________________________
static std::string getSymbolForType(IntentionType type) {
  switch (type) {
    case IntentionType::IDLE: return "IDL";
    case IntentionType::LISTEN: return "LST";
    case IntentionType::SEND: return "SEND";
    case IntentionType::SEND_FORCE: return "SEND!";
    default: Misc::Asserts::require(false, "invalid enum value"); break;
  }
  return "";
}

// _____________________________________________________________________________
NetworkSetup::NetworkSetup(size_t ticsPerSlot) : mTicsPerSlot(ticsPerSlot) {
  Misc::Asserts::require(ticsPerSlot > 0,
    "at least one tic per slot required!");
}

// _____________________________________________________________________________
void NetworkSetup::registerMessage(const Message* msg) {
  Misc::Asserts::require(!isMessage(msg), "duplicate message registered");
  Misc::Asserts::require(msg != nullptr, "can not register nullptr as message");
  mMessages.insert(msg);
}

// _____________________________________________________________________________
void NetworkSetup::registerComponent(Component* comp) {
  Misc::Asserts::require(comp != nullptr, "can not register nullptr as "
    "component");
  Misc::Asserts::require(!isComponent(*comp), "duplicate component registered");
  mComponents.push_back(comp);
}

// _____________________________________________________________________________
bool NetworkSetup::isMessage(const Message* msg) const {
  return mMessages.find(msg) != mMessages.end();
}

// _____________________________________________________________________________
bool NetworkSetup::isComponent(const Component& comp) const {
  for (const Component* elem : mComponents) {
    if (elem == &comp) {
      return true;
    }
  }
  return false;
}

// _____________________________________________________________________________
void NetworkSetup::forEachComponent(std::function<void(Component*)> cb)
    const {
  for (Component* comp : mComponents) {
    cb(comp);
  }
}

// _____________________________________________________________________________
template<class T>
ComponentTrait<T>::ComponentTrait(const NetworkSetup& setup, T type,
    std::size_t tic, const Message* message) : mType(type), mTic(tic),
      mMessage(message) {
  // Assert tic number < maxTic.
  Misc::Asserts::require(setup.getTicsPerSlot() > tic,
    "invalid tic number: too big");

  // We expect that existing messages are registered. But we allow them not to
  // be.
  if (message != nullptr) {
    Misc::Asserts::expect(setup.isMessage(message),
      "message not registered with the network setup!");
  }
}

// _____________________________________________________________________________
template<class T>
std::string ComponentTrait<T>::toString() const {
  if (mMessage == nullptr) {
    // Only show the symbol for the type.
    return getSymbolForType(mType);
  } else {
    // Show the symbol together with message and tic.
    std::stringstream sstr;
    sstr << getSymbolForType(mType);
    sstr << "[";
    sstr << mMessage->toString();
    sstr << ", ";
    sstr << mTic;
    sstr << "]";
    return sstr.str();
  }
}

// _____________________________________________________________________________
template<class T>
std::vector<std::string> ComponentTrait<T>::toXML() const {
  std::vector<std::string> res;
  std::stringstream sstr;

  res.push_back("<trait>");

  sstr << "  <type>" << getSymbolForType(mType) << "</type>";
  res.push_back(sstr.str());
  sstr.str("");

  if (mMessage != nullptr) {
    res.push_back("  <msg>");
    std::vector<std::string> xmlRepr = mMessage->toXML();
    for (const std::string& rpr : xmlRepr) {
      sstr << "    " << rpr;
      res.push_back(sstr.str());
      sstr.str("");
    }
    res.push_back("  </msg>");

    sstr << "  <tic>" << mTic << "</tic>";
    res.push_back(sstr.str());
    sstr.str("");
  }

  res.push_back("</trait>");

  return res;
}

// _____________________________________________________________________________
template<class T>
bool ComponentTrait<T>::operator==(const ComponentTrait<T>& other) const {
  if (mMessage == nullptr || other.mMessage == nullptr) {
    return mType == other.mType && mTic == other.mTic
      && mMessage == other.mMessage;
  }
  return mType == other.mType && mTic == other.mTic
    && *mMessage == *(other.mMessage);
}

// _____________________________________________________________________________
template<class T>
TraitMapping<T>::TraitMapping(const NetworkSetup* setup) : mSetup(setup),
  mPartial(true) {}

// _____________________________________________________________________________
template<class T>
const ComponentTrait<T>& TraitMapping<T>::getTraitFor(const Component* comp)
    const {
  Misc::Asserts::require(!mPartial,
    "attempting to get trait for partial trait mapping");
  Misc::Asserts::require(mSetup->isComponent(*comp),
    "not a valid component for associated network setup");

  // An invariant of trait mappings is that if mPartial == false, then every
  // valid component is mapped. Thus this can not throw.
  return mMapping.at(comp);
}

// _____________________________________________________________________________
template<class T>
void TraitMapping<T>::setTraitFor(const Component* comp,
    const ComponentTrait<T>& action) {
  Misc::Asserts::require(mSetup->isComponent(*comp),
    "not a valid component for associated network setup");

  // Add the entry, if it is no duplicate.
  auto entry = mMapping.find(comp);
  Misc::Asserts::require(entry == mMapping.end(), "can not override component "
    "trait for component");
  mMapping.emplace(comp, action);

  // Check whether or not the trait mapping is still partial.
  if (mMapping.size() == mSetup->getComponentCount()) {
    // Every component has a value as only components can be in mMapping.
    mPartial = false;
  }
}

// _____________________________________________________________________________
template<class T>
std::string TraitMapping<T>::toString() const {
  Misc::Asserts::require(!mPartial,
    "attempting to get string for partial trait mapping");
  std::string result("(");

  std::size_t nCompsDone = 0;
  mSetup->forEachComponent([this, &result, &nCompsDone](const Component* comp) {
    result += mMapping.at(comp).toString();

    if (++nCompsDone != mMapping.size()) {
      // There are more to come.
      result += ", ";
    }
  });

  result += ")";
  return result;
}

// _____________________________________________________________________________
template<class T>
std::vector<std::string> TraitMapping<T>::toXML() const {
  Misc::Asserts::require(!mPartial,
    "attempting to get XML for partial trait mapping");
  std::vector<std::string> res;

  mSetup->forEachComponent([this, &res](const Component* comp) {
    res.push_back("<entry>");
    std::stringstream sstr;

    sstr << "  <for>" << comp->getId() << "</for>";
    res.push_back(sstr.str());
    sstr.str("");

    std::vector<std::string> xmlRepr = mMapping.at(comp).toXML();
    for (const std::string& rpr : xmlRepr) {
      sstr << "  " << rpr;
      res.push_back(sstr.str());
      sstr.str("");
    }

    res.push_back("</entry>");
  });

  return res;
}

// _____________________________________________________________________________
ANL::ANL(const NetworkSetup* setup, ANLSemantics semantics) : mSetup(setup),
    mSemantics(semantics) {}

// _____________________________________________________________________________
std::vector<NetworkState> ANL::transition(const NetworkTopology* topo,
    const IntentionAssignment* intent) const {
  // Everything is contained in anl_algorithm.h -- nothing here in order to
  //  seperate algorithm interface and algorithm implementation.
  FilterFunction filter;
  if (mSemantics == ANLSemantics::CANONICAL) {
    filter = ANLFilterNothing;
  } else if (mSemantics == ANLSemantics::NAIVE) {
    filter = ANLFilterNaive;
  } else {
    Misc::Asserts::require(false, "unknown semantics");
  }

  ANLComputer anlComputer(mSetup, topo, intent, filter);
  return anlComputer.transition();
}

// _____________________________________________________________________________
void ANL::runSlot(std::size_t slot, const NetworkState* prevState,
    IntentionAssignment* targetIntent) {
  mSetup->forEachComponent(
    [this, slot, prevState, targetIntent](Component* comp) {
      // Determine the previous action of the component.
      if (prevState != nullptr) {
        // Create the view with the previous action.
        ANLView view(mSetup, slot, comp, prevState->getTraitFor(comp),
          targetIntent);
        comp->onAct(&view);

        Misc::Asserts::require(view.hasActed(), "component did not choose "
          "component intent for slot");
      } else {
        // Create the view without a previous action.
        ANLView view(mSetup, slot, comp, targetIntent);
        comp->onAct(&view);

        Misc::Asserts::require(view.hasActed(), "component did not choose "
          "component intent for slot");
      }
  });
}

// _____________________________________________________________________________
ANLView::ANLView(const NetworkSetup* setup, std::size_t slot,
    const Component* comp, const ComponentAction& prev,
    IntentionAssignment* targetIntent) : mSetup(setup), mSlot(slot),
      mComponent(comp), mPreviousAction(prev), mHasPreviousAction(true),
      mTargetIntent(targetIntent), mActed(false) {
  Misc::Asserts::require(mSetup->isComponent(*mComponent), "component unknown "
    "to setup!");
}

// _____________________________________________________________________________
ANLView::ANLView(const NetworkSetup* setup, std::size_t slot,
    const Component* comp, IntentionAssignment* targetIntent) : mSetup(setup),
      mSlot(slot), mComponent(comp),
      mPreviousAction(*setup, ActionType::IDLE, 0, nullptr),
      mHasPreviousAction(false), mTargetIntent(targetIntent), mActed(false) {
  Misc::Asserts::require(mSetup->isComponent(*mComponent), "component unknown "
    "to setup!");
}

// _____________________________________________________________________________
void ANLView::idle() {
  Misc::Asserts::require(!mActed, "already acted in slot");
  mTargetIntent->setTraitFor(mComponent, ComponentIntention(*mSetup,
    IntentionType::IDLE, 0, nullptr));
  mActed = true;
}

// _____________________________________________________________________________
void ANLView::send(const Message* msg, std::size_t tic, bool carrierSensing) {
  Misc::Asserts::require(!mActed, "already acted in slot");
  IntentionType type =
    carrierSensing ? IntentionType::SEND : IntentionType::SEND_FORCE;
  mTargetIntent->setTraitFor(mComponent, ComponentIntention(*mSetup, type, tic,
    msg));
  mActed = true;
}

// _____________________________________________________________________________
void ANLView::listen() {
  Misc::Asserts::require(!mActed, "already acted in slot");
  mTargetIntent->setTraitFor(mComponent, ComponentIntention(*mSetup,
    IntentionType::LISTEN, 0, nullptr));
  mActed = true;
}

// _____________________________________________________________________________
ComponentAction ANLView::getPreviousAction() const {
  Misc::Asserts::require(mHasPreviousAction, "no previous action!");
  return mPreviousAction;
}

// _____________________________________________________________________________
void ANLView::logProtocol(const std::string& msg) const {
  std::fprintf(stderr, "[ PROT ] Log: %s\n", msg.c_str());
}

// _____________________________________________________________________________
// Explicit template instantiations.
template class ComponentTrait<ActionType>;
template class ComponentTrait<IntentionType>;
template class TraitMapping<ActionType>;
template class TraitMapping<IntentionType>;

}  // namespace Core
