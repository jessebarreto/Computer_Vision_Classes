#file(COPY fruit.jpg wine.jpg DESTINATION .)
add_executable(Project6 main.cpp project6.cpp)
target_link_libraries(Project6 ${OpenCV_LIBS})
