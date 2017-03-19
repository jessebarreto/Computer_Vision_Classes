
# Add the begin source code
add_executable(begin begin.cpp)
target_link_libraries(begin ${OpenCV_LIBS})

# Add the webcam source code
add_executable(webcam_test webcam_test.cpp)
target_link_libraries(webcam_test ${OpenCV_LIBS})

# Add the video source code
add_executable(video_example begin_video.cpp)
target_link_libraries(video_example ${OpenCV_LIBS})

