add_executable(iarrc_circuit_race_controller iarrc_circuit_race_controller.cpp)
target_link_libraries(iarrc_circuit_race_controller ${catkin_LIBRARIES})
add_dependencies(iarrc_circuit_race_controller ${iarrc_msgs_EXPORTED_TARGETS})
