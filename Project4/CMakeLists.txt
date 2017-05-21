

file(COPY edges-images DESTINATION .)
add_executable( Project4_Reqs1234 project4_req1234.cpp project4.h)
target_link_libraries( Project4_Reqs1234 ${OpenCV_LIBS} )
