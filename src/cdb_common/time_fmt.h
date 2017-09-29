#pragma once

#include <sstream>
#include <iomanip>

namespace cdb {

class time_fmt
{
public:
    time_fmt(const char* format) : m_format(format)
	{}

    std::time_t from_string(const char* str)
	{
        std::istringstream ss(str);
        std::tm tm = {};
        ss >> std::get_time(&tm, m_format.c_str());
        if (ss.fail())
        {
            return -1;
        }
        std::time_t time = mktime(&tm);
		return time * 1000;
	}

private:
    time_fmt(const time_fmt& src)
    {
        operator=(src);
    };
    time_fmt& operator=(const time_fmt& src)
    {
        m_format = src.m_format;
        return *this;
    }

private:
    std::string m_format;
};

}