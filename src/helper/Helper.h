#ifndef IMAGE_SERVICE_HELPER_H
#define IMAGE_SERVICE_HELPER_H

#include <iostream>
#include <sstream>
#include <vector>

class Helper {
public:
    static std::string toString(const char *buffer);

    static std::vector<std::string> split(const std::string &s, char delimiter);

    static std::vector<std::string> split(const std::string &s, const std::string &delimiter);
};


#endif //IMAGE_SERVICE_HELPER_H
