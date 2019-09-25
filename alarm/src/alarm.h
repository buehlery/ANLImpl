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

#ifndef ALARM_H_
#define ALARM_H_

#include <anl/anlimpl.h>
#include <string>
#include <vector>

// The states for the ALARM protocol.
enum class AlarmState {
  // A, B, and AB states
  //  An A state is a state in which alarms are sent to supervisors. An A state
  //  may only transition to B and AB states.
  //  A B state is a state in which alarms are acknowledged by supervisors. A B
  //  state may only transition to A and AB states.
  //  An AB state is a state in which no communication may be performed. An AB
  //  state may only transition to AB states.
  // Immediate transitions are transitions in which the state is changed and
  //  another transition is taken without delay. Immediate transitions may only
  //  be performed between identical state types (A -> A, B -> B, AB -> AB).

  // The initial state for the central unit.
  INITIAL_CU,  // A state

  // The state for when the central unit is waiting for a message.
  WAITING_CU,  // B state

  // The initial state for a repeater.
  INITIAL_REP,  // A state

  // The state for when a repeater waits for a message.
  WAIT_FOR_ALARM_REP,  // B state

  // The state for when a repeater needs to forward alarms.
  FORWARD_ALARMS_REP,  // A state

  // The state for when a repeater attempted to forward an alarm.
  ATTEMPTED_REP,  // B state

  // The state for when a repeater is waiting for an acknowledgement.
  WAIT_FOR_ACK_REP,  // A state

  // The initial state for a sensor.
  INITIAL_SEN,  // A state

  // The state for when the sensor attempted to send.
  ATTEMPTED_SEN,  // B state

  // The state for when the sensor waits for an acknowledgement.
  WAIT_FOR_ACK_SEN,  // A state

  // The state for when the sensor is done with its protocol.
  DONE_SEN  // AB state
};

// The central unit.
class CentralUnit : public StateMachineComponent<AlarmState> {
 public:
  // Constructor.
  CentralUnit();

 private:
  // The protocol implementation.
  AlarmState doStateAct(ANLView* view, AlarmState state) override;

  // Getter for the ID.
  std::string doGetId() const override { return "CentralUnit"; }

  // XML conversion.
  std::vector<std::string> doToXML() const override;
};

// A repeater.
class Repeater : public StateMachineComponent<AlarmState> {
 public:
  // Constructor.
  explicit Repeater(std::size_t num);

 private:
  // The number of the repeater.
  std::size_t mNum;

  // The priority of the repeater.
  std::size_t mPriority;

  // The collision counter of the repeater.
  std::size_t mCollision;

  // The alarm storage.
  const Component* mAlarms[10];

  // The alarm counter.
  std::size_t mAlarmCount;

  // Callback for missing acknowledgements. May modify the priority.
  void failure();

  // Adds an alarm for the given component to the storage.
  void addAlarm(const Component* comp);

  // The protocol implementation.
  AlarmState doStateAct(ANLView* view, AlarmState state) override;

  // Getter for the ID.
  std::string doGetId() const override;

  // XML conversion.
  std::vector<std::string> doToXML() const override;
};

// A sensor.
class Sensor : public StateMachineComponent<AlarmState> {
 public:
  // Constructor.
  Sensor(std::size_t id, std::size_t num);

 private:
  // The internal ID of the sensor.
  std::size_t mId;

  // The number of the sensor.
  std::size_t mNum;

  // The priority of the sensor.
  std::size_t mPriority;

  // The collision counter of the sensor.
  std::size_t mCollision;

  // Callback for missing acknowledgements. May modify the priority.
  void failure();

  // The protocol implementation.
  AlarmState doStateAct(ANLView* view, AlarmState state) override;

  // Getter for the ID.
  std::string doGetId() const override;

  // XML conversion.
  std::vector<std::string> doToXML() const override;
};

// The different types of messages in the ALARM protocol.
enum class MessageType {
  // An ALARM message.
  ALARM,

  // An acknowledgement.
  ACK
};

// A message in the ALARM protocol.
class ProtocolMessage : public Message {
 public:
  // Constructor.
  ProtocolMessage(MessageType type, const Component* from,
    const Component* to, const Component* data);

  // Getters.
  MessageType getType() const { return mType; }
  const Component* getFrom() const { return mFrom; }
  const Component* getTo() const { return mTo; }
  const Component* getData() const { return mData; }

  // Get the message object with the given parameters.
  static const ProtocolMessage* getMessage(MessageType type,
    const Component* from, const Component* to, const Component* data);

  // Cleans up all messages.
  static void clean();

 private:
  // The type of the message.
  MessageType mType;

  // The component this message originates from.
  const Component* mFrom;

  // The component this message is directed to.
  const Component* mTo;

  // The component that is passed as payload of the message.
  const Component* mData;

  // Getter for the ID.
  std::string doToString() const override;

  // XML conversion.
  std::vector<std::string> doToXML() const override;
};

// The different components that exist.
extern Component** gRepeaters;
extern Component** gSensors;
extern Component*  gCentralUnit;

#endif  // ALARM_H_
