
add_executable(convertformat2d convertformat.cpp)
target_link_libraries(convertformat2d aoutput amesh2dh)

add_executable(convertformat3d convertformat3d.cpp)
target_link_libraries(convertformat3d amesh3d)

add_executable(convertGmshToDomn3D convertformatGmshDomn3d.cpp)
target_link_libraries(convertGmshToDomn3D amesh3d)

add_executable(quadratize3d quadratize3d.cpp)
target_link_libraries(quadratize3d amesh3d)

add_executable(shapemetric2d shapemetric2d.cpp)
target_link_libraries(shapemetric2d amesh2dh aoutput)

add_executable(avg-minj avg-minj.cpp)
