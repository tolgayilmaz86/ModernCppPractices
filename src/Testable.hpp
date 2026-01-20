#pragma once

#include <string>

class Testable {
public:
    virtual ~Testable() = default;
    virtual void run() = 0;
    virtual std::string name() const = 0;
};