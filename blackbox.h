#ifndef _BLACKBOX_H_
#define _BLACKBOX_H_

#include <string>
#include <vector>

namespace quasi {

struct IncomingPort {
  // The name of this port.
  // REQUIRES: Port names, regardless of their type, are unique.
  std::string name;
  // Additional Metadata...
};

struct OutgoingPort {
  // The name of this port.
  // REQUIRES: Port names, regardless of their type, are unique.
  std::string name;
  // Known relationship with an Incoming Port in the BlackBox.
  // REQUIRES: This is the name of an IncomingPort in the BlackBox.
  std::vector<std::string> related_to;
  // Additional Metadata...
};

struct BlackBox {
  // Ports that can be used to inject information into the BlackBox.
  std::vector<IncomingPort> incoming_ports;
  // Ports that can be used to extract information into the BlackBox.
  std::vector<OutgoingPort> outgoing_ports;
  // Additional Metadata...
};

}

#endif