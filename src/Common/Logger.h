#pragma once

#include <string>
#include <memory>

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
private:
	static std::shared_ptr<ILogger> sLogger;
public:
	~ILogger()
	{}
    
	virtual void Log(LogEvent event, const std::string& message)
	{};

	void LogDebug(const std::string& message)
	{
		Log(LogEvent::Debug, message);
	}

	void LogInformation(const std::string& message)
	{
		Log(LogEvent::Information, message);
	}
	void LogWarning(const std::string& message)
	{
		Log(LogEvent::Warning, message);
	}
	void LogException(const std::string& message)
	{
		Log(LogEvent::Exception, message);
	}
	void LogAlert(const std::string& message)
	{
		Log(LogEvent::Alert, message);
	}

	static std::shared_ptr<ILogger> Instance()
	{
		return ILogger::sLogger;
	}
};

std::shared_ptr<ILogger> ILogger::sLogger = std::make_shared<ILogger>();

}}
