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
#include "./alarm.h"

// _____________________________________________________________________________
CentralUnit::CentralUnit()
  : StateMachineComponent(AlarmState::INITIAL_CU) {}

// _____________________________________________________________________________
AlarmState CentralUnit::doStateAct(ANLView* view, AlarmState state) {
  switch (state) {
    case AlarmState::INITIAL_CU:  // A
      // We are waiting for messages to register. As message for supervisors
      //  (i.e. a message for us to register) arrive in A states, we listen.
      view->listen();
      return AlarmState::WAITING_CU;  // B

    case AlarmState::WAITING_CU:  // B
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
            view->send(reply, 0, false);
            view->logProtocol("Alarm: " + msg->getData()->getId());
            return AlarmState::INITIAL_CU;  // A
          }  // Else: An alarm, but not for us.
        }  // Else: A message, but no alarm.
      }  // Else: Nothing understandable received at all.

      // Otherwise, if we did not get a message, we just wait for one to come.
      view->idle();
      return AlarmState::INITIAL_CU;  // A

    default:
      // Should not happen. As we select no intention, we provoke a failure.
      return state;
  }
}

// _____________________________________________________________________________
std::vector<std::string> CentralUnit::doToXML() const {
  std::vector<std::string> result;
  result.push_back("<layer>0</layer>");
  return result;
}
