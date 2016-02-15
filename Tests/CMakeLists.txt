add_executable (StickTests StickTests.cpp)
target_link_libraries(StickTests Stick ${STICKDEPS})
add_custom_command(TARGET StickTests
                   POST_BUILD
                   COMMAND ./StickTests)
