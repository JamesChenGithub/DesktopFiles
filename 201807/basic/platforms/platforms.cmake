get_filename_component(PLATFORMS_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

if(WIN32)
	include(${PLATFORMS_CMAKE_DIR}/windows/windows.cmake)
elseif(UNIX)
	include(${PLATFORMS_CMAKE_DIR}/unix/unix.cmake)
	if(CMAKE_SYSTEM_NAME MATCHES "Android")
		include(${PLATFORMS_CMAKE_DIR}/android/android.cmake)
	endif()
endif()

set(PLATFORMS_INC
		${PLATFORMS_CMAKE_DIR}
		)

set(PLATFORMS_SRC
		)

if(WINDOWS_CMAKE_DIR)
	set(PLATFORMS_INC ${PLATFORMS_INC}
			${WINDOWS_INC}
			)
	set(PLATFORMS_SRC ${PLATFORMS_SRC}
			${WINDOWS_SRC}
			)
endif()

if(UNIX_CMAKE_DIR)
	set(PLATFORMS_INC ${PLATFORMS_INC}
			${UNIX_INC}
			)
	set(PLATFORMS_SRC ${PLATFORMS_SRC}
			${UNIX_SRC}
			)
endif()

if(ANDROID_CMAKE_DIR)
	set(PLATFORMS_INC ${PLATFORMS_INC}
			${ANDROID_INC}
			)
	set(PLATFORMS_SRC ${PLATFORMS_SRC}
			${ANDROID_SRC}
			)
endif()