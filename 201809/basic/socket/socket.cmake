get_filename_component(SOCKET_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

set(SOCKET_INC
		${SOCKET_CMAKE_DIR}
		)

set(SOCKET_SRC
		${SOCKET_CMAKE_DIR}/getdnssvraddrs.cc
		${SOCKET_CMAKE_DIR}/getgateway.c
		${SOCKET_CMAKE_DIR}/getifaddrs.cc
		${SOCKET_CMAKE_DIR}/local_ipstack.cc
		${SOCKET_CMAKE_DIR}/nat64_prefix_util.cc
		${SOCKET_CMAKE_DIR}/socket_address.cc
		${SOCKET_CMAKE_DIR}/unix_socket.cc
		${SOCKET_CMAKE_DIR}/jni/ifaddrs.c
		)
