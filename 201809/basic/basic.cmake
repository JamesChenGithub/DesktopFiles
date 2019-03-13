get_filename_component(BASIC_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

add_definitions(-D__CMAKE__)

include(${BASIC_CMAKE_DIR}/structs/structs.cmake)
include(${BASIC_CMAKE_DIR}/enums/enums.cmake)
include(${BASIC_CMAKE_DIR}/platforms/platforms.cmake)
include(${BASIC_CMAKE_DIR}/networks/networks.cmake)
include(${BASIC_CMAKE_DIR}/thread/thread.cmake)
include(${BASIC_CMAKE_DIR}/filesystem/filesystem.cmake)
include(${BASIC_CMAKE_DIR}/utils/utils.cmake)
include(${BASIC_CMAKE_DIR}/datareport/datareport.cmake)
include(${BASIC_CMAKE_DIR}/log/log.cmake)
include(${BASIC_CMAKE_DIR}/module/module.cmake)
include(${BASIC_CMAKE_DIR}/socket/socket.cmake)

set(BASIC_INC
		${BASIC_CMAKE_DIR}
		${STRUCTS_INC}
		${ENUMS_INC}
		${PLATFORMS_INC}
		${NETWORKS_INC}
		${THREAD_INC}
		${FILE_SYSTEM_INC}
		${UTILS_INC}
		${DATA_REPORT_INC}
		${LOG_INC}
		${MODULE_INC}
		${SOCKET_INC}
		)

set(BASIC_SRC
		${STRUCTS_SRC}
		${ENUMS_SRC}
		${PLATFORMS_SRC}
		${NETWORKS_SRC}
		${THREAD_SRC}
		${FILE_SYSTEM_SRC}
		${UTILS_SRC}
		${DATA_REPORT_SRC}
		${LOG_SRC}
		${MODULE_SRC}
		${SOCKET_SRC}
		${BASIC_CMAKE_DIR}/txg_verinfo.c
		)
