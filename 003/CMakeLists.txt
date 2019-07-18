cmake_minimum_required(VERSION 3.14)

get_filename_component(DIRECTORY_NAME "${CMAKE_CURRENT_LIST_DIR}" NAME)

add_executable(
	${DIRECTORY_NAME}
	main.cpp
	AudioEndpointDeviceNotificationClient.h
	AudioEndpointDeviceNotificationClient.cpp
)

target_compile_definitions(${DIRECTORY_NAME} PUBLIC TARGET_NAME="${DIRECTORY_NAME}")
