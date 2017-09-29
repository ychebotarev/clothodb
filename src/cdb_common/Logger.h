#pragma once

#include <string>
#include <memory>

namespace cdb{

enum log_event
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
    
	virtual void log(log_event event, const std::string& message)
	{};

	void log_debug(const std::string& message)
	{
        log(log_event::Debug, message);
	}

	void log_information(const std::string& message)
	{
		log(log_event::Information, message);
	}
	void log_warning(const std::string& message)
	{
        log(log_event::Warning, message);
	}
	void log_exception(const std::string& message)
	{
        log(log_event::Exception, message);
	}
	void log_alert(const std::string& message)
	{
        log(log_event::Alert, message);
	}

	static std::shared_ptr<ILogger> instance()
	{
		return ILogger::sLogger;
	}
};

std::shared_ptr<ILogger> ILogger::sLogger = std::make_shared<ILogger>();

}
