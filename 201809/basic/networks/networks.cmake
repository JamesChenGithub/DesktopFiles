get_filename_component(NETWORKS_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

set(NETWORKS_INC
		${NETWORKS_CMAKE_DIR}
		)

if(ENABLE_QUIC)
set(NETWORKS_SRC
		${NETWORKS_CMAKE_DIR}/txg_connect_util.c
		${NETWORKS_CMAKE_DIR}/TXCByteQueue.cpp
		${NETWORKS_CMAKE_DIR}/AsyncNetClientWrapper.cpp
		${NETWORKS_CMAKE_DIR}/SyncNetClientWrapper.cpp
		${NETWORKS_CMAKE_DIR}/NetClientContextWrapper.cpp
		)
else()
set(NETWORKS_SRC
		${NETWORKS_CMAKE_DIR}/txg_connect_util.c
		${NETWORKS_CMAKE_DIR}/TXCByteQueue.cpp
		${NETWORKS_CMAKE_DIR}/SyncNetClientWrapperNoQuic.cpp
		)
endif()

