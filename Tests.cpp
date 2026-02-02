
#include <cassert>
#include <iostream>
#include "Message.hpp"

int main() {
    using namespace std::string_view_literals;
    {
        constexpr Message b{"AAAAAAAAAA0#"sv};
        assert(b.ToString().size() == 0);
    }
    {
        constexpr Message<char> c{"RUN_NO____123#"sv};
        assert(c.ToString() == "Run number: 123" );
    }
    {
        constexpr Message d{"POLAR_NO__2#"sv};
        assert(d.ToString() == "Polar number: 2" );
    }
    {
        constexpr Message d{"USR_MSG___Start Tunnel#"sv};
        assert(d.ToString() == "Start Tunnel" );
    }
    {
        constexpr Message d{"USR_MSG___Start Tunnel🙏#"sv};
        assert(d.ToString() == "Start Tunnel🙏" );
    }
    {
        auto pairs = SplitIntoPairs("Parameter1,0.004947,"sv);
        auto pair = pairs.at(0);
        assert(pair.find("Parameter1") != std::string_view::npos);
        assert(pair.find(",") != std::string_view::npos);
        assert(pair.find("0.004947") != std::string_view::npos);
    }
    {
        auto pairs = SplitIntoPairs("Parameter1,0.004947"sv);
        auto pair = pairs.at(0);
        assert(pair.find("Parameter1") != std::string_view::npos);
        assert(pair.find(",") != std::string_view::npos);
        assert(pair.find("0.004947") != std::string_view::npos);
    }
    {
        auto pairs = SplitIntoPairs("Parameter1,0.004947,Parameter2,0.203044,"sv);
        {
            auto pair = pairs.at(0);
            assert(pair.find("Parameter1") != std::string_view::npos);
            assert(pair.find(",") != std::string_view::npos);
            assert(pair.find("0.004947") != std::string_view::npos);
        }
        {
            auto pair = pairs.at(1);
            assert(pair.find("Parameter2") != std::string_view::npos);
            assert(pair.find(",") != std::string_view::npos);
            assert(pair.find("0.203044") != std::string_view::npos);
        }
    }
    {
        auto kvFeilds = ParseKeyValueFields("Parameter1,0.004947,"sv);
        assert(kvFeilds == "Parameter1 = 0.004947");
    }
    {
        auto kvFeilds = ParseKeyValueFields("Parameter1,0.004947,Parameter2,0.203044,"sv);
        assert(kvFeilds == "Parameter1 = 0.004947\nParameter2 = 0.203044");
    }
    {
        constexpr Message d{
            "D_USR_FLD_Parameter1,0.004947,Parameter2,0.203044,#"
        };
        assert(d.ToString() == "Parameter1 = 0.004947\nParameter2 = 0.203044");
    }
    {
        constexpr Message d{
            L"D_USR_FLD_Parameter1️⃣,0.004947,Parameter2️⃣,0.203044,#"
        };
        assert(d.ToString() == L"Parameter1️⃣ = 0.004947\nParameter2️⃣ = 0.203044");
    }
    {
        constexpr Message d{ "HISTORY___#" };
        d.AddToHistory(); d.AddToHistory(); d.AddToHistory(); d.AddToHistory();
        assert(d.ToString() == "HISTORY___\nHISTORY___\nHISTORY___\nHISTORY___");
    }
    {
        std::string h = "HISTORY___#"; 
        Message d{ h };
        assert(d.ToString() == "HISTORY___\nHISTORY___\nHISTORY___\nHISTORY___");
    }
    
}
