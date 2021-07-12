#pragma once

namespace gzz
{
	enum class NET_ERR : int
	{
		FAIL = -1,
		UNKNOWN = 0,
		SUCESS = 1,

		START_UP_FAIL,
		SOCKET_CREATE_FAIL,
		SOCKET_ALREADY_ASSIGNED,
		BIND_FAIL,
		LISTEN_FAIL,

		ACCEPT_FAIL,
		ACCEPT_PROCESS_FAIL,
		COMPLETIONPORT_BINDING_FAIL,

		CONNECT_FAIL,
		CONNECT_PROCESS_FAIL,

		WSA_SEND_FAIL,
		SEND_ERROR_MAX_DATA,
		SEND_ERROR_NEED_MORE_DATA,
		SEND_ERROR_MORE_DATA,
		SEND_PROCESS_FAIL,

		WSA_RECV_FAIL,
		RECV_PROCESS_MORE_DATA,
		RECV_PROCESS_FAIL_DATA,

		SET_SOCKET_CONDITIONAL_ACCEPT_FAIL,
		SET_SOCKET_RUSUEADDR_FAIL,
		SET_SOCKET_UPDATE_ACCEPT_CONETEXT_FAIL,
		SET_SOCKET_UPDATE_CONNECT_CONETEXT_FAIL,
		GET_ACCPEPT_EX_PTR_FAIL,

		SOCKET_CONNECT_YET,
		SOCKET_ESTABLISHED_YET,
		SOCKET_ESTABLISHED_DIV,

		SOCKET_EX_TYPE,
		DISCONNECT_EX_FAIL,
		DISCONNECT_ALLOC_FAIL,
	};

	enum class RECV_STATUS
	{
		OK = 0xfffd,
		MORE = 0xfffe,
		FAIL = 0xffff
	};

}