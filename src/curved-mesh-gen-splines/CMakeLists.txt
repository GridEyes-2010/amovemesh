
add_executable(amc curve3d.cpp)
target_link_libraries(amc amesh3d arbf alinalg ageometry3d amatrix)

add_executable(curveh curvedmeshgen2dh.cpp)
target_link_libraries(curveh arbf ageometryh amesh2dh adatastructures amatrix)

# add_executable(curvesr curvedmeshgen2dh_sr.cpp)
# target_link_libraries(curvesr arbfsr aboundaryinfluence ageometryh amesh2dh adatastructures amatrix)

add_executable(curveelast linelast-curvedmeshgen2d.cpp)
target_link_libraries(curveelast arbf ageometry adatastructures amatrix)