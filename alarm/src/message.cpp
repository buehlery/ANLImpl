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
#include <unordered_map>
#include "./alarm.h"

// _____________________________________________________________________________
// Required for old implementations of g++ which don't handle enum hashing.
struct EnumHasher {
  template<class T>
  std::size_t operator()(T enumVal) const {
    // Use the integer constant of the enumeration value as the hash.
    return static_cast<std::size_t>(enumVal);
  }
};

// _____________________________________________________________________________
using MessageMap = std::unordered_map<MessageType,
    std::unordered_map<const Component*,
      std::unordered_map<const Component*,
        std::unordered_map<const Component*, const ProtocolMessage*>
      >
    >,
    EnumHasher
  >;

// _____________________________________________________________________________
static MessageMap gMessageMap;

// _____________________________________________________________________________
const ProtocolMessage* ProtocolMessage::getMessage(MessageType type,
    const Component* from, const Component* to, const Component* data) {
  std::size_t count = gMessageMap[type][from][to].count(data);
  if (count == 0) {
    gMessageMap[type][from][to][data] =
      new ProtocolMessage(type, from, to, data);
  }
  return gMessageMap[type][from][to][data];
}

// _____________________________________________________________________________
void ProtocolMessage::clean() {
  for (auto& x : gMessageMap) {
    for (auto& y : x.second) {
      for (auto& z : y.second) {
        for (auto& w : z.second) {
          delete w.second;
        }
      }
    }
  }
}

// _____________________________________________________________________________
ProtocolMessage::ProtocolMessage(MessageType type, const Component* from,
  const Component* to, const Component* data) : mType(type), mFrom(from),
    mTo(to), mData(data) {}

// _____________________________________________________________________________
std::string ProtocolMessage::doToString() const {
  std::stringstream sstr;
  sstr << (mType == MessageType::ALARM ? "ALARM" : "ACK");
  sstr << "[" << mFrom->getId() << ">" << mTo->getId() << ": " << mData->getId()
    << "]";
  return sstr.str();
}

// _____________________________________________________________________________
std::vector<std::string> ProtocolMessage::doToXML() const {
  std::vector<std::string> res;
  std::stringstream sstr;
  sstr << "<type>" << (mType == MessageType::ALARM ? "ALARM" : "ACK")
    << "</type>";
  res.push_back(sstr.str());
  sstr.str("");
  sstr << "<from>" << mFrom->getId() << "</from>";
  res.push_back(sstr.str());
  sstr.str("");
  sstr << "<to>" << mTo->getId() << "</to>";
  res.push_back(sstr.str());
  sstr.str("");
  sstr << "<data>" << mData->getId() << "</data>";
  res.push_back(sstr.str());
  sstr.str("");
  return res;
}
