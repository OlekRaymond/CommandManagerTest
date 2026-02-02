
#include "Message.hpp"
#include <iostream>

// Windows has some odd backwards comaptibility issues
//  so prefers wide strings for Unicode but in linux a short string will
//  hold any ascii character just fine
// Some programers also dislike the widechar idea so
//  it's handy to have both options avaliable
// Though if you are doing this regularly it may be better to use a library
//  to make Unicode a bit easier and more ergonomic

// "short" API, prefered on linux
void ManageCommand(std::string_view message) {
    Message parsed{message};
    std::cout << parsed.ToString();
    parsed.AddToHistory();
}

// "wide" API, sometimes prefered on windows
void ManageCommand(std::wstring_view message) {
    Message parsed{message};
    std::wcout << parsed.ToString();
    parsed.AddToHistory();
}

