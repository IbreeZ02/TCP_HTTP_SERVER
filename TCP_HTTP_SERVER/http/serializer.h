#pragma once
#include "httpResponse.h"
#include <string>

class Serializer {
public:
    std::string serialize(const HttpResponse& res);
};