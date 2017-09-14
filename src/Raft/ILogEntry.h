#pragma once

namespace incolun {
namespace raft {

class ILogEntry
{
public:
	ILogEntry():ILogEntry(0){}
	ILogEntry(long term) :m_term(term) {}
	virtual ~ILogEntry() {}
private:
	long m_term;

};

}}
