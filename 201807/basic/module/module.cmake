get_filename_component(MODULE_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

set(MODULE_INC
		${MODULE_CMAKE_DIR}
		)

set(MODULE_SRC
		${MODULE_CMAKE_DIR}/TXCStatusRecorder.cpp
		)
