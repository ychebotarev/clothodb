#pragma once

#include <string>

namespace incolun {
namespace clothodb {

enum LogEvent
{
    Debug,
    Information,
    Warning,
    Exception,
    Alert,
    Custom
};

class ILogger
{
public:
    virtual void Log(LogEvent event, const std::string& message) = 0;
};

}}
