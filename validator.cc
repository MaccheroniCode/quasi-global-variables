#include "validator.h"

#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>

#include "blackbox.h"

namespace quasi {
namespace {

bool IsValidBlackBox_SingleFunction(const BlackBox& box);
bool IsValidBlackBox_HelperFunctions(const BlackBox& box);
bool IsValidBlackBox_ValidatorClass(const BlackBox& box);

}

bool IsValidBlackBox(const BlackBox& box) {
  // We forward the call to 3 Possible Implementations.
  constexpr int impl = 1;
  switch (impl) {
    case 0: return IsValidBlackBox_SingleFunction(box);
    case 1: return IsValidBlackBox_HelperFunctions(box);
    case 2: return IsValidBlackBox_ValidatorClass(box);
    default: return false;
  }
}

namespace {
////////////////////////////////////////////////////////////
// Implementation Using a Single Function.
//
// This is a compact implementation of the validation logic.
// 
// NOTE: Assume that the "Extra Metadata Validation" blocks
// are non-trivial, and so you find yourself in the spot of
// trying to refactor it to keep complexity manageable.
//
// IMPORTANT: This is a toy example to prove a point.
// Don't consider it production-grade.
////////////////////////////////////////////////////////////

bool IsValidBlackBox_SingleFunction(const BlackBox& box) {
  // The names of all the ports seen so far.
  std::unordered_set<std::string_view> port_names;
  // Validate incoming ports.
  for (const IncomingPort& incoming_port : box.incoming_ports) {
    // Check for duplicate names.
    if (const auto [_, inserted] = port_names.insert(incoming_port.name);
        !inserted) {
      return false;
    }
    // ...
    // Extra Metadata Validation
    // ...
  }
  // Save the names of the incoming ports to validate related_to.
  std::unordered_set<std::string_view> incoming_port_names = port_names;
  // Validate outgoing ports.
  for (const OutgoingPort& outgoing_port : box.outgoing_ports) {
    // Check for duplicate names.
    if (const auto [_, inserted] = port_names.insert(outgoing_port.name);
        !inserted) {
      return false;
    }
    // Local check for duplicated related_to.
    std::unordered_set<std::string_view> related_to_names;
    // Validate the related_to.
    for (const std::string_view related_to : outgoing_port.related_to) {
      // Validate that this is an incoming port name.
      if (!incoming_port_names.contains(related_to)) {
        return false;
      }
      // Check for duplicates.
      if (const auto [_, inserted] = related_to_names.insert(related_to);
          !inserted) {
        return false;
      }
    }
    // ...
    // Extra Metadata Validation
    // ...
  }
  return true;
}

////////////////////////////////////////////////////////////
// Implementation Using a Few Helper Functions.
//
// This version only uses free functions to split the logic
// into small well-defined sub-tasks.
// 
// NOTE: The signatures of these helpers could get long,
// depending on how you decide to split the logic.
// The artisanship stands in finding relatively independent
// sub-tasks with little to no dependencies.
// The simpler these utilities are the more you are REDUCING
// COMPLEXITY.
// When you look at the implementation of each single
// function you just need to know the SIGNATURE of the
// others to understand their implications. You DON'T need
// to know how they are IMPLEMENTED.
//
// IMPORTANT: This is a toy example to prove a point.
// Don't consider it production-grade.
////////////////////////////////////////////////////////////

// Validates the incoming ports, and returns a set containing
// their names, or std::nullopt in case of error.
std::optional<std::unordered_set<std::string_view>>
  ValidateIncomingPortsAndReturnNames(
    std::span<const IncomingPort> ports) {
  // The names of all the ports seen so far.
  std::unordered_set<std::string_view> names;
  // Validate incoming ports.
  for (const IncomingPort& port : ports) {
    // Check for duplicate names.
    if (const auto [_, inserted] = names.insert(port.name);
        !inserted) {
      return std::nullopt;
    }
    // ...
    // Extra Metadata Validation
    // ...
  }
  return names;
}

bool ValidateRelatedTo(
  std::span<const std::string> related_to,
  const std::unordered_set<std::string_view>& incoming_port_names) {
  // Local check for duplicated related_to.
  std::unordered_set<std::string_view> names;
  // Validate the related_to.
  for (const std::string_view incoming_port_name : related_to) {
    // Validate that this is an incoming port name.
    if (!incoming_port_names.contains(incoming_port_name)) {
      return false;
    }
    // Check for duplicates.
    if (const auto [_, inserted] = names.insert(incoming_port_name);
      !inserted) {
      return false;
    }
  }
  return true;
}

bool ValidateOutgoingPorts(std::span<const OutgoingPort> ports,
  const std::unordered_set<std::string_view>& incoming_port_names) {
  // The names of all the ports seen so far.
  std::unordered_set<std::string_view> names = incoming_port_names;
  // Validate outgoing ports.
  for (const OutgoingPort& port : ports) {
    // Check for duplicate names.
    if (const auto [_, inserted] = names.insert(port.name);
        !inserted) {
      return false;
    }
    if (!ValidateRelatedTo(port.related_to, incoming_port_names)) {
      return false;
    }
    // ...
    // Extra Metadata Validation
    // ...
  }
  return true;
}

bool IsValidBlackBox_HelperFunctions(const BlackBox& box) {
  const std::optional<std::unordered_set<std::string_view>>
    incoming_port_names =
      ValidateIncomingPortsAndReturnNames(box.incoming_ports);
  if (!incoming_port_names.has_value()) {
    return false;
  }
  return ValidateOutgoingPorts(
    box.outgoing_ports, *incoming_port_names);
}

////////////////////////////////////////////////////////////
// Implementation Using a Validator Class.
//
// This version uses single-use BlackBoxValidator class.
// Sub-tasks are split into member functions in the class,
// while data is shared by storing it in member variables.
// 
// NOTE: The signatures of these member functions is short,
// but is hiding the real dependencies between them.
// While it is simple to add, remove, and propagate state
// around what is happening is that COMPLEXITY is being
// HIDDEN, not removed.
// When you look at the implementation of each single member
// function it is not enough to know the SIGNATURE of the
// others to understand their implications. You NEED to
// know how they are IMPLEMENTED.
// When you HIDE COMPLEXITY, a SIGNATURE tells you little
// to nothing without knowing all the HIDDEN dependencies.
//
// IMPORTANT: This is a toy example to prove a point.
// Don't consider it production-grade.
////////////////////////////////////////////////////////////

class BlackBoxValidator {
 public:
  explicit BlackBoxValidator(const BlackBox& box) : box_(box) {}

  bool Validate() {
    if (!ValidateIncomingPorts()) {
      return false;
    }
    return ValidateOutgoingPorts();
  }
 private:
  bool ValidateIncomingPorts() {
    // Validate incoming ports.
    for (const IncomingPort& port : box_.incoming_ports) {
      // Check for duplicate names.
      if (const auto [_, inserted] = port_names_.insert(port.name);
          !inserted) {
        return false;
      }
      // ...
      // Extra Metadata Validation
      // ...
    }
    incoming_port_names_ = port_names_;
    return true;
  }

  bool ValidateOutgoingPorts() {
    // Validate outgoing ports.
    for (const OutgoingPort& port : box_.outgoing_ports) {
      // Check for duplicate names.
      if (const auto [_, inserted] = port_names_.insert(port.name);
          !inserted) {
        return false;
      }
      if (!ValidateRelatedTo(port.related_to)) {
        return false;
      }
      // ...
      // Extra Metadata Validation
      // ...
    }
    return true;
  }

  bool ValidateRelatedTo(std::span<const std::string> related_to) {
    // Local check for duplicated related_to.
    std::unordered_set<std::string_view> names;
    // Validate the related_to.
    for (const std::string_view incoming_port_name : related_to) {
      // Validate that this is an incoming port name.
      if (!incoming_port_names_.contains(incoming_port_name)) {
        return false;
      }
      // Check for duplicates.
      if (const auto [_, inserted] = names.insert(incoming_port_name);
        !inserted) {
        return false;
      }
    }
    return true;
  }

  const BlackBox& box_;
  std::unordered_set<std::string_view> port_names_;
  std::unordered_set<std::string_view> incoming_port_names_;
};

bool IsValidBlackBox_ValidatorClass(const BlackBox& box) {
  BlackBoxValidator validator(box);
  return validator.Validate();
}

}
}