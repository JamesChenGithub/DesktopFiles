get_filename_component(FILE_SYSTEM_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

set(FILE_SYSTEM_INC
		${FILE_SYSTEM_CMAKE_DIR}
		)

set(FILE_SYSTEM_SRC
		${FILE_SYSTEM_CMAKE_DIR}/txg_mmap_util.cpp
		${FILE_SYSTEM_CMAKE_DIR}/TXCMMapFile.cpp
		${FILE_SYSTEM_CMAKE_DIR}/TXCPath.cpp
		${FILE_SYSTEM_CMAKE_DIR}/TXCPathIterator.cpp
		)