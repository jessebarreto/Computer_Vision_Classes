

file(COPY fruits.jpg DESTINATION .)
add_executable( Project5_Req1 project5_req1.cpp project5.cpp)
target_link_libraries(Project5_Req1 ${OpenCV_LIBS})
