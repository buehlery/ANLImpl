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

#include <sstream>
#include "./alarm.h"

// _____________________________________________________________________________
Sensor::Sensor(std::size_t id, std::size_t num)
  : StateMachineComponent(AlarmState::INITIAL_SEN), mId(id), mNum(num),
    mPriority(8), mCollision(0) {}

// _____________________________________________________________________________
void Sensor::failure() {
  bool left = ((mId & (128 >> mCollision)) == 0);
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
AlarmState Sensor::doStateAct(ANLView* view, AlarmState state) {
  switch (state) {
    case AlarmState::INITIAL_SEN:  // A
      // We assume tic * mPriority as the "base tic" denoted by 'base'. This
      //  value ranges from 1*tic to 8*tic.
      //  LBT (Carrier Sensing) starts at base + hfInit + LBTinit, which is
      //  approx. base + 0.23 * tic.
      //  LBT ends at base + hfInit + LBTinit + LBTLen, which is approx.
      //  base + 0.31 * tic.
      //  As sending for the base tic starts at
      //  base + hfInit + LBTinit + LBTLen + TXtoRX (~ base + 0.5 * tic), only
      //  transmissions from before the base tic can be noticed by LBT.
      //  We achieve this with the ANL by starting in tic "base tic".
      {
        const Message* msg = ProtocolMessage::getMessage(MessageType::ALARM,
          this, gRepeaters[4], this);
        view->send(msg, mPriority);
        return AlarmState::ATTEMPTED_SEN;  // B
      }

    case AlarmState::ATTEMPTED_SEN:  // B
      // If the transmission was cancelled, we fall back into INITIAL_SEN. As we
      //  are currently in a B state, we enter an A state in which we can send.
      if (view->getPreviousAction().getType() == ActionType::CANCELLED) {
        // Re-send in the next slot again.
        view->idle();
        return AlarmState::INITIAL_SEN;  // A
      }

      // The ALARM protocol usually expects acknowledgements in the same slot.
      //  We use B states for this. At this point we expect an
      //  acknowledgement.
      view->listen();
      return AlarmState::WAIT_FOR_ACK_SEN;  // A

    case AlarmState::WAIT_FOR_ACK_SEN:  // A
      // At this point we expect an acknowledgement. Here we check for it.
      if (view->getPreviousAction().getType() == ActionType::RECEIVED) {
        const ProtocolMessage* msg = dynamic_cast<const ProtocolMessage*>(
          view->getPreviousAction().getMessage());
        // We received a message, but now we need to check if it is the
        //  acknowledgement that we expect.
        if (msg != nullptr && msg->getType() == MessageType::ACK) {
          if (this == msg->getTo()) {
            // Here: Our message was acknowledged, we're done.
            view->idle();
            return AlarmState::DONE_SEN;  // AB
          }  // Else: An acknowledgement, but not for us.
        }  // Else: A message, but no acknowledgement.
      }  // Else: Nothing understandable received at all.

      // From here, we take the path SENT .. second state in the model (failure
      //  path). As we already waited one slot and want to re-send the alarm in
      // An A state, we take an immediate transition to INITIAL_SEN in order to
      // resend the alarm.

      // We record a failure and immediately execute the initial state (alarm
      //  sending) again.
      failure();
      return doStateAct(view, AlarmState::INITIAL_SEN);  // Immediate: A

    case AlarmState::DONE_SEN:  // AB
      // The component is in its done-loop (the ALARM might have not reached the
      //  central unit though). This represents the TWO states in the model
      //  (not only DONE, but also the waiting state).
      view->idle();
      return state;  // AB

    default:
      // Should not happen. As we select no intention, we provoke a failure.
      return state;
  }
}

// _____________________________________________________________________________
std::string Sensor::doGetId() const {
  std::stringstream sstr;
  sstr << "Sensor" << mNum;
  return sstr.str();
}

// _____________________________________________________________________________
std::vector<std::string> Sensor::doToXML() const {
  std::vector<std::string> result;
  std::stringstream sstr;
  sstr << "<internalId>" << mId << "</internalId>";
  result.push_back(sstr.str());

  // All sensors are on layer 6.
  result.push_back("<layer>6</layer>");
  return result;
}
