
#include "Message.hpp"
#include <iostream>

void ManageCommand(std::string_view message) {
    Message parsed{message};
    std::cout << parsed.ToString();
    parsed.AddToHistory();
}

void ManageCommand(std::wstring_view message) {
    Message parsed{message};
    std::wcout << parsed.ToString();
    parsed.AddToHistory();
}
