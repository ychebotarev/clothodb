#pragma once

#include <cstdint>
namespace incolun {
namespace raft {

class RaftState {
public:
	RaftState()
	{}

private:
	RaftState(const RaftState&) = delete;
	RaftState& operator=(const RaftState&) = delete;

public:
	uint64_t GetTerm() const { return m_term; }
	uint64_t GetCommitIndex() const { return m_commitIndex; }
	int GetVotedFor() const { return m_votedFor; }

	void SetTerm(uint64_t term) { m_term = term; }
	void SetCommitIndex(uint64_t commitIndex)
	{
		if (commitIndex > m_commitIndex)
			m_commitIndex = commitIndex;
	}
	void SetVotedFor(int votedFor) { m_votedFor = votedFor; }
	
	void IncrementTerm() { m_term += 1; }
private:
	uint64_t m_term = 0;
	uint64_t m_commitIndex = 0;
	int m_votedFor = -1;
};

}}