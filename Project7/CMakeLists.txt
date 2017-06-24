

file(COPY Data DESTINATION .)
add_executable( Project7 main.cpp project7.h)
target_link_libraries(Project7 ${OpenCV_LIBS})
