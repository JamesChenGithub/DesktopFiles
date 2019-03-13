get_filename_component(THREAD_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

set(THREAD_INC
		${THREAD_CMAKE_DIR}
		)

set(THREAD_SRC
		${THREAD_CMAKE_DIR}/TXCAbstractThread.cpp
		${THREAD_CMAKE_DIR}/TXCMutex.cpp
		${THREAD_CMAKE_DIR}/TXCCondition.cpp
		${THREAD_CMAKE_DIR}/TXCThread.cpp
		${THREAD_CMAKE_DIR}/TXCThreadLocalStore.cpp
		)
