cmake_minimum_required(VERSION 3.6)

project(amp_i2c_test)

aux_source_directory(.  SRC_FILE)


set(CMAKE_SKIP_RPATH TRUE)


add_executable(amp_i2c_test ${SRC_FILE})

target_link_libraries(amp_i2c_test m)
