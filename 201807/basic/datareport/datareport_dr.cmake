get_filename_component(DATA_REPORT_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

set(DATA_REPORT_INC
		${DATA_REPORT_CMAKE_DIR}
		${DATA_REPORT_CMAKE_DIR}/include
		)

set(DATA_REPORT_SRC
		${DATA_REPORT_CMAKE_DIR}/tx_pb_codec.c
		${DATA_REPORT_CMAKE_DIR}/tx_dr_codec.c
		${DATA_REPORT_CMAKE_DIR}/tx_dr_api.cpp
		${DATA_REPORT_CMAKE_DIR}/tx_dr_base.cpp
		${DATA_REPORT_CMAKE_DIR}/tx_dr_thread.cpp
		${DATA_REPORT_CMAKE_DIR}/tx_dr_util.cpp
		)