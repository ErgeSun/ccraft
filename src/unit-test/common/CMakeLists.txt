aux_source_directory(. SRCS)

add_executable(common_test ${SRCS})
target_link_libraries(common_test common ccsys protobuf glog gflags gtest pthread)
