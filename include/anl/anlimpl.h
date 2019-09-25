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

#ifndef ANL_ANLIMPL_H_
#define ANL_ANLIMPL_H_

// Include everything that is relevant for use as a library.
#include "anl/core/anl.h"
#include "anl/core/entry_point.h"
#include "anl/core/simulator.h"
#include "anl/core/statemachine.h"
#include "anl/core/topologies.h"
#include "anl/core/types.h"
#include "anl/output/output.h"

// Import important names, if not disabled.
#ifndef ANL_NO_USING

using Core::ActionType;
using Core::ANLView;
using Core::Component;
using Core::ComponentAction;
using Core::ExplicitNetworkTopology;
using Core::IsolatedNetworkTopology;
using Core::Message;
using Core::NetworkTopology;
using Core::Simulator;
using Core::StateMachineComponent;
using Core::TrivialNetworkTopology;
using Output::StdOutOutputModule;
using Output::XMLOutputModule;

#endif  // ANL_NO_USING

#endif  // ANL_ANLIMPL_H_
