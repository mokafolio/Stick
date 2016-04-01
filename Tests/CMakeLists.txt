add_executable (StickTests EXCLUDE_FROM_ALL StickTests.cpp)
target_link_libraries(StickTests Stick ${STICKDEPS})
add_custom_target(check COMMAND StickTests)