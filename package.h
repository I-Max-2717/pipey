#pragma once
#include <string>
#include <vector>

struct Version {
    std::string number;
    std::string date; // Format: YYYY-MM-DD
    std::string url;
};

struct Package {
    std::string name;
    std::vector<Version> versions;
};
