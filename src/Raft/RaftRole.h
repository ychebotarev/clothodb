#pragma once

namespace incolun {
namespace raft {

enum RaftRole {
	follower = 0x1,
	candidate,
	leader
};

}}

