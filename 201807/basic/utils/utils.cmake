get_filename_component(UTILS_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

set(UTILS_INC
		${UTILS_CMAKE_DIR}
		)

set(UTILS_SRC
		${UTILS_CMAKE_DIR}/txg_assert.c
		${UTILS_CMAKE_DIR}/txg_log_base.c
		${UTILS_CMAKE_DIR}/txg_time_util.cpp
		${UTILS_CMAKE_DIR}/TXCTickCount.cpp
		)
