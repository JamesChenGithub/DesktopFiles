get_filename_component(BASIC_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

add_definitions(-D__CMAKE__ -DDONT_USES_LOG)

include(${BASIC_CMAKE_DIR}/platforms/platforms.cmake)
include(${BASIC_CMAKE_DIR}/networks/networks.cmake)
include(${BASIC_CMAKE_DIR}/thread/thread.cmake)
include(${BASIC_CMAKE_DIR}/datareport/datareport_dr.cmake)

set(BASIC_INC
		${PLATFORMS_INC}
		${THREAD_INC}
		${NETWORKS_INC}
		${DATA_REPORT_INC}
		${BASIC_CMAKE_DIR}/log
		)

set(BASIC_SRC
		${PLATFORMS_SRC}
		${THREAD_SRC}
		${NETWORKS_SRC}
		${DATA_REPORT_SRC}
		${BASIC_CMAKE_DIR}/txg_verinfo.c
		)
