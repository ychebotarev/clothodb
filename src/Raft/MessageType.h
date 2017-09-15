#pragma once

namespace incolun {
namespace raft {

	enum MessageType {
		request_vote_request = 0x1,
		request_vote_response,
		append_entries_request,
		append_entries_response,
		client_request,
		add_server_request,
		add_server_response,
		remove_server_request,
		remove_server_response,
		sync_log_request,
		sync_log_response,
		join_cluster_request,
		join_cluster_response,
		leave_cluster_request,
		leave_cluster_response,
		install_snapshot_request,
		install_snapshot_response
	};
}}