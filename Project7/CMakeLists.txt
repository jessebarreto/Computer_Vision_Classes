

file(COPY Data DESTINATION .)
add_executable( Project7 main.cpp)
target_link_libraries(Project7 ${OpenCV_LIBS})
