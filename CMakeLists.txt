cmake_minimum_required(VERSION 3.0.0)
project(TRGoChessC VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()
set(CMAKE_C_FLAGS_RELEASE "-O3")
set(CMAKE_C_FLAGS_RELEASE "-O3")
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/Inc)

include_directories(${INCLUDE_DIR} ${PROJECT_SOURCE_DIR})

add_executable(TRGoChessC main.c chessboard.c chessmanual.c surroundmap.c LinkedList.c Stack.c Queue.c ACAutomaton.c globals.c GameManager.c Player.c HumanPlayer.c AIPlayer.c ChessPot.c Action.c AIUtilities.c Train.c GA.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
