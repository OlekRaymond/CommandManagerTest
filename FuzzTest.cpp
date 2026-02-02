#include <cstdint>
#include "Message.hpp"

// fuzz_target.cc
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  std::string_view view{(const char*)data, size};
  try {
    Message message{view};
    [[maybe_unused]] auto a = message.ToString();
  } catch (...) {
    // ignore errors, common in fuzz testing
  }
  return 0;  // Values other than 0 and -1 are reserved for future use.
}
