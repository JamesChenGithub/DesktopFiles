get_filename_component(LOG_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

set(LOG_INC
		${LOG_CMAKE_DIR}
		)

set(LOG_SRC
		${LOG_CMAKE_DIR}/txg_log_util.c
		${LOG_CMAKE_DIR}/txg_log.cpp
		${LOG_CMAKE_DIR}/txg_formater.cpp
		${LOG_CMAKE_DIR}/txg_appender.cpp
		${LOG_CMAKE_DIR}/TXCAutoBuffer.cpp
		${LOG_CMAKE_DIR}/TXCPtrBuffer.cpp
		${LOG_CMAKE_DIR}/TXCLogBuffer.cpp
		${LOG_CMAKE_DIR}/TXCLogCrypt.cpp
		)