#include <iostream>

#include "blackbox.h"
#include "validator.h"

namespace {
  using quasi::BlackBox;
}

int main() {
  // Setup the BlackBox.
  const BlackBox box = {
    .incoming_ports = {
      {.name = "In0"},
      {.name = "In1"},
      {.name = "In2"}
    },
    .outgoing_ports = {
      {.name = "Out0", .related_to = {"In0"}},
      {.name = "Out1"},
      {.name = "Out2", .related_to = {"In0", "In2"}}
    }
  };
  // Setup the BlackBox.
  std::cout << "Validating the BlackBox\n";
  if (!IsValidBlackBox(box)) {
    std::cout << "Invalid BlackBox\n";
    return 1;
  }
  // Use the BlackBox.
  std::cout << "Using the BlackBox\n";
  return 0;
}