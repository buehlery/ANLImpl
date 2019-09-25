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

#include "./alarm.h"

// Protocol and simulation constants.
// (Number of repeaters)
#define SIM_NUM_REP 5
// (Number of sensors)
#define SIM_NUM_SENSOR 10
// (Number of components overall (includes central unit))
#define SIM_NUM_COMPS (1 + SIM_NUM_REP + SIM_NUM_SENSOR)
// (The sensor ID offset)
#define SIM_SENSOR_ID_OFFSET 128

// Regarding FREQUENCIES.
// As the ALARM protocol only uses one frequency at a time in the example from
// the UPPAAL models (TR2Channel), we only use a single frequency (i.e. one
// ANL).

// _____________________________________________________________________________
Component** gRepeaters;
Component** gSensors;
Component*  gCentralUnit;

// _____________________________________________________________________________
// Entry point.
ANLIMPL_MAIN(int argc, char** argv) {
  // Here we create the components. We create the central unit, 5 repeaters, and
  //  10 sensors. We create the repeaters with IDs 1 to 5. We create the sensors
  //  with log_id (used for e.g. display) 0 to 9 and with IDs (used for e.g.
  //  priorities) 128 to 137.
  gCentralUnit = new CentralUnit();
  gRepeaters = new Component*[SIM_NUM_REP];
  for (std::size_t i = 0; i < SIM_NUM_REP; i++) {
    gRepeaters[i] = new Repeater(i + 1);
  }
  gSensors = new Component*[SIM_NUM_SENSOR];
  for (std::size_t i = 0; i < SIM_NUM_SENSOR; i++) {
    gSensors[i] = new Sensor(SIM_SENSOR_ID_OFFSET + i, i);
  }

  // Here we just add the different components to a single array, as ANL-Impl
  //  expects the components to be registered in a batch.
  Component* comps[SIM_NUM_COMPS];
  comps[0] = gCentralUnit;
  for (std::size_t i = 0; i < SIM_NUM_REP; i++) {
    comps[1 + i] = gRepeaters[i];
  }
  for (std::size_t i = 0; i < SIM_NUM_SENSOR; i++) {
    comps[1 + SIM_NUM_REP + i] = gSensors[i];
  }

  // Here we create the topology. The network topology is the following:
  //  (C is the central unit, Ri is repeater i, Sj is sensor j)
  //                                       +-> S0
  //                                      /    |
  // C <-> R1 <-> R2 <-> R3 <-> R4 <-> R5    ...   (Sensors can reach R5 and
  //                                      \    |    each other)
  //                                       +-> S9
  ExplicitNetworkTopology ent;
  ent.addEdge(gCentralUnit, gRepeaters[0]);
  ent.addEdge(gRepeaters[0], gCentralUnit);  // C <-> R1
  for (std::size_t i = 0; i < SIM_NUM_REP - 1; i++) {
    // This loop creates the repeater-"stem".
    ent.addEdge(gRepeaters[i], gRepeaters[i + 1]);
    ent.addEdge(gRepeaters[i + 1], gRepeaters[i]);
  }
  for (std::size_t i = 0; i < SIM_NUM_SENSOR; i++) {
    // This loop connects R5 and the sensors, and the sensors with one other.
    ent.addEdge(gSensors[i], gRepeaters[SIM_NUM_REP - 1]);
    ent.addEdge(gRepeaters[SIM_NUM_REP - 1], gSensors[i]);
    for (std::size_t j = 0; j < SIM_NUM_SENSOR; j++) {
      ent.addEdge(gSensors[i], gSensors[j]);
      ent.addEdge(gSensors[j], gSensors[i]);
    }
  }

  // Here we create the simulator. The constructor argument 25 is the number of
  //  tics per slot (taken from the UPPAAL model). We also assign the topology
  //  and register the components.
  Simulator sim(25);
  sim.useTopology(&ent);
  sim.useComponents(comps, SIM_NUM_COMPS);

  // Create the messages. Two is the number of distinct message types
  //  (ALARM, ACK).
  const Message** msgs = new const Message*[2 * SIM_NUM_COMPS * SIM_NUM_COMPS
    * SIM_NUM_COMPS];
  std::size_t ptr = 0;
  for (std::size_t i = 0; i < SIM_NUM_COMPS; i++) {
    for (std::size_t j = 0; j < SIM_NUM_COMPS; j++) {
      for (std::size_t k = 0; k < SIM_NUM_COMPS; k++) {
        msgs[ptr++] = ProtocolMessage::getMessage(MessageType::ALARM, comps[i],
          comps[j], comps[k]);
        msgs[ptr++] = ProtocolMessage::getMessage(MessageType::ACK, comps[i],
          comps[j], comps[k]);
      }
    }
  }
  sim.useMessages(msgs, 2 * SIM_NUM_COMPS * SIM_NUM_COMPS * SIM_NUM_COMPS);
  delete[] msgs;  // We no longer need the array of messages created for
                  // registering them.

  // Run the simulation for 120 slots (we determined this amount to be enough by
  //  trying different values: after ~115 slots, nothing interesting happens
  //  anymore).
  sim.run(120);

  // Cleanup.
  delete gCentralUnit;
  for (std::size_t i = 0; i < SIM_NUM_REP; i++) {
    delete gRepeaters[i];
  }
  delete[] gRepeaters;
  for (std::size_t i = 0; i < SIM_NUM_SENSOR; i++) {
    delete gSensors[i];
  }
  delete[] gSensors;
  ProtocolMessage::clean();

  return 0;
}
