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
// Part of the ALARM example.

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include "./alarm.h"

// _____________________________________________________________________________
Repeater::Repeater(std::size_t num)
  : StateMachineComponent(AlarmState::INITIAL_REP), mNum(num), mPriority(0),
    mCollision(0), mAlarmCount(0) {}

// _____________________________________________________________________________
void Repeater::failure() {
  bool left = ((mNum & (128 >> mCollision)) == 0);
  if (left) {
    // Lower the priority.
    if (mPriority <= 1) {
      mPriority = 8;
    } else {
      mPriority--;
    }
  }

  // Count the collision.
  mCollision++;
  if (mCollision == 9) {
    mCollision = 0;
    mPriority = 8;
  }
}

// _____________________________________________________________________________
void Repeater::addAlarm(const Component* comp) {
  for (std::size_t i = 0; i < mAlarmCount; i++) {
    if (mAlarms[i] == comp) {
      return;
    }
  }
  if (mAlarmCount == 10) {
    std::fprintf(stderr, "Too many alarms.\n");
    std::exit(1);
  }
  mAlarms[mAlarmCount++] = comp;
}

// _____________________________________________________________________________
AlarmState Repeater::doStateAct(ANLView* view, AlarmState state) {
  switch (state) {
    case AlarmState::INITIAL_REP:  // A
      // We are waiting for messages to relay. As message for supervisors (i.e.
      //  a message for us to relay) arrive in A states, we listen.
      view->listen();
      return AlarmState::WAIT_FOR_ALARM_REP;  // B

    case AlarmState::WAIT_FOR_ALARM_REP:  // B
      // Now we react to the message. We do this as soon as our timer exceeds
      //  TXtoRX, which is the case after approx. 0.19 tics.
      if (view->getPreviousAction().getType() == ActionType::RECEIVED) {
        const ProtocolMessage* msg = dynamic_cast<const ProtocolMessage*>(
          view->getPreviousAction().getMessage());
        // We received a message, but now we need to check if it is the alarm
        //  message that we expect.
        if (msg != nullptr && msg->getType() == MessageType::ALARM) {
          if (this == msg->getTo()) {
            // Here: It's a message for us! We will acknowledge it here.
            //  As we only wait TXtoRX in the protocol (0.19 tics), we send it
            //  in tic 0 of the slot, without carrier sensing.
            const ProtocolMessage* reply = ProtocolMessage::getMessage(
              MessageType::ACK, this, msg->getFrom(), msg->getData());
            addAlarm(msg->getData());
            view->logProtocol(getId() + ": added alarm to stack: "
              + msg->getData()->getId());
            view->send(reply, 0, false);
            return AlarmState::FORWARD_ALARMS_REP;  // A
          }  // Else: An alarm, but not for us.
        }  // Else: A message, but no alarm.
      }  // Else: Nothing understandable received at all.

      // We now need to check whether or not we got more alarms.
      if (mAlarmCount > 0) {
        // We go to FORWARD_ALARMS_REP as we did not get an alarm but we have
        //  alarms left. This is the transition from the left side into
        //  LBT_START in the model.
        view->idle();
        return AlarmState::FORWARD_ALARMS_REP;  // A
      } else {
        // We return to INITIAL_REP as we did not get an alarm and have no
        //  alarm.
        view->idle();
        return AlarmState::INITIAL_REP;  // A
      }

    case AlarmState::FORWARD_ALARMS_REP:  // A
      // Missing in comparison to ALARM:
      //  LBT_START -> ALM: In this transition we attempt to receive an alarm
      //  but we want to forward one at the same time. However, with the ANL we
      //  need to decide on what to do beforehand.
      //  LBT_END -> ALM: Same reason as LBT_START -> ALM.
      //  CD -> ALM: Same reason as LBT_START -> ALM.

      // We assume tic * mPriority as the "base tic" denoted by 'base'. This
      //  value ranges from 0*tic to 8*tic.
      //  LBT (Carrier Sensing) starts at base + hfInit + LBTinit, which is
      //  approx. base + 0.23 * tic.
      //  LBT ends at base + hfInit + LBTinit + LBTLen, which is approx.
      //  base + 0.31 * tic.
      //  As sending for the base tic starts at
      //  base + hfInit + LBTinit + LBTLen + TXtoRX (~ base + 0.5 * tic), only
      //  transmissions from before the base tic can be noticed by LBT.
      //  We achieve this with the ANL by starting in tic "base tic".
      {
        // We now forward the alarm to our supervisor layer.
        const Component* target =
          (mNum == 1) ? gCentralUnit : gRepeaters[mNum - 2];
        const ProtocolMessage* fwd = ProtocolMessage::getMessage(
          MessageType::ALARM, this, target, mAlarms[mAlarmCount - 1]);
        view->send(fwd, mPriority);
        return AlarmState::ATTEMPTED_REP;  // B
      }

    case AlarmState::ATTEMPTED_REP:  // B
      // If the transmission was CANCELLED, we return to attempting to send it
      //  without recording a failure (CD -> LBT_START).
      if (view->getPreviousAction().getType() == ActionType::CANCELLED) {
        view->idle();
        return AlarmState::FORWARD_ALARMS_REP;  // A
      }

      // Otherwise, we listen for an acknowledgement.
      view->listen();
      return AlarmState::WAIT_FOR_ACK_REP;  // A

    case AlarmState::WAIT_FOR_ACK_REP:  // A
      // We now check whether or not we got an acknowledgement.
      if (view->getPreviousAction().getType() == ActionType::RECEIVED) {
        const ProtocolMessage* msg = dynamic_cast<const ProtocolMessage*>(
          view->getPreviousAction().getMessage());
        // We received a message, but now we need to check if it is the
        //  acknowledgement message that we expect.
        if (msg != nullptr && msg->getType() == MessageType::ACK) {
          if (this == msg->getTo()) {
            // Here: Acknowledgement received. We will now listen to obtain more
            //  alarms and remove the alarm from our stack.
            // ALARM only does this if there are no more alarms. But as we do
            //  not support the extra actions in FORWARD_ALARMS_REP, we do this
            //  here as we could only have one alarm at a time otherwise.
            view->listen();
            view->logProtocol(getId() + ": latest alarm marked as done");
            mAlarmCount--;
            mPriority = 0;
            mCollision = 0;
            return AlarmState::WAIT_FOR_ALARM_REP;  // B
          }  // Else: An acknowledgement, but not for us.
        }  // Else: A message, but no acknowledgement.
      }  // Else: Nothing understandable at all.

      // If the alarm was not acknowledged, we immediately execute forwarding
      //  again and record a failure.
      failure();
      return doStateAct(view, AlarmState::FORWARD_ALARMS_REP);  // Immediate: A

    default:
      // Should not happen. As we select no intention, we provoke a failure.
      return state;
  }
}

// _____________________________________________________________________________
std::string Repeater::doGetId() const {
  std::stringstream sstr;
  sstr << "Repeater" << mNum;
  return sstr.str();
}

// _____________________________________________________________________________
std::vector<std::string> Repeater::doToXML() const {
  std::vector<std::string> result;
  std::stringstream sstr;
  sstr << "<layer>" << mNum << "</layer>";
  result.push_back(sstr.str());
  return result;
}
