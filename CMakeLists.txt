cmake_minimum_required(VERSION 3.0.0)
project(TRGoChessC VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()
set(CMAKE_C_FLAGS_RELEASE "-O3")

# set(CMAKE_C_FLAGS_DEBUG "-fsanitize=undefined,address,leak -fno-omit-frame-pointer")
set(CMAKE_C_FLAGS "-fopenmp")

include_directories(${INCLUDE_DIR} ${PROJECT_SOURCE_DIR})

add_executable(TRGoChessC main.c ChessBoard.c Stack.c Queue.c ACAutomaton.c Globals.c GameManager.c Player.c HumanPlayer.c AIPlayer.c ChessPot.c Action.c AIUtilities.c Train.c GA.c mt19937.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
