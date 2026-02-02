#include <cstdint>
#include "Message.hpp"

// fuzz_target.cc
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    try {
        // Ascii only
        std::string_view view{(const char*)data, size};
        Message message{view};
        [[maybe_unused]] auto a = message.ToString();
    } catch (...) {
        // ignore errors, common in fuzz testing
    }
    try {
        // UTF-8
        std::wstring_view view{(const wchar_t*)data, size / sizeof(wchar_t)};
        Message<wchar_t> message{view};
        [[maybe_unused]] auto b = message.ToString();
    } catch (...) {
        // ignore errors, common in fuzz testing
    }   
    return 0;  // Values other than 0 and -1 are reserved for future use.
}
