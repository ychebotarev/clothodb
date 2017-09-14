#pragma once

namespace cornerstone {
namespace clothodb {

	template<int N>
	class strfmt 
	{
	public:
		strfmt(const char* fmt)
			: fmt_(fmt) 
		{}

		template<typename ... TArgs>
		const char* fmt(TArgs... args) 
		{
			::snprintf(buf_, N, fmt_, args...);
			return buf_;
		}

	private:
		strfmt(const strfmt&) = delete;
		strfmt& operator=(const strfmt&) = delete;
	private:
		char buf_[N];
		const char* fmt_;
	};

	typedef strfmt<100> sstrfmt;
	typedef strfmt<300> lstrfmt;

}}
