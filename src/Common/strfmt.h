#pragma once

namespace clothodb {
namespace common {

template<int N>
class strfmt 
{
public:
	strfmt(const char* fmt)
		: m_fmt(fmt)
	{}

	template<typename ... TArgs>
	const char* fmt(TArgs... args) 
	{
		::snprintf(m_bufffer, N, m_fmt, args...);
		return m_bufffer;
	}

private:
	strfmt(const strfmt&) = delete;
	strfmt& operator=(const strfmt&) = delete;
private:
	char m_bufffer[N];
	const char* m_fmt;
};

typedef strfmt<100> sstrfmt;
typedef strfmt<300> lstrfmt;

}}
