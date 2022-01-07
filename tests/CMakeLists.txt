project("cbi_test")
add_executable( ${PROJECT_NAME}
    main.cpp 
    "test_cbi.cpp")
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE cbi)


string(REPLACE "/W3" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REPLACE "/O2" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REPLACE "/W3" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
string(REPLACE "/O2" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
string(REPLACE "/W3" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
string(REPLACE "/O2" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
string(REPLACE "/O2" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
string(REPLACE "/W3" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
string(REPLACE "/W3" "" CMAKE_CXX_FLAGS_MINSIZE "${CMAKE_CXX_FLAGS_MINSIZE}")
string(REPLACE "/O2" "" CMAKE_CXX_FLAGS_MINSIZE "${CMAKE_CXX_FLAGS_MINSIZE}")
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>: $<$<CXX_COMPILER_ID:MSVC>:           /std:c++latest /W4 /WX /Od /JMC >>
    $<$<CONFIG:Release>: $<$<CXX_COMPILER_ID:MSVC>:         /std:c++latest /W4 /WX /O2      >>
    $<$<CONFIG:RelWithDebInfo>: $<$<CXX_COMPILER_ID:MSVC>:  /std:c++latest /W4 /WX /O1 /JMC >>
    $<$<CONFIG:MinSizeRel>: $<$<CXX_COMPILER_ID:MSVC>:      /std:c++latest /W4 /WX /O2      >>

    $<$<CONFIG:Debug>:$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:              -std=c++20 -Wall -Wextra -Werror -pedantic -Og>>
    $<$<CONFIG:Release>:$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:            -std=c++20 -Wall -Wextra -Werror -pedantic -O3>>
    $<$<CONFIG:RelWithDebInfo>: $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:    -std=c++20 -Wall -Wextra -Werror -pedantic -O1>>
    $<$<CONFIG:MinSizeRel>: $$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:       -std=c++20 -Wall -Wextra -Werror -pedantic -Os>>)
