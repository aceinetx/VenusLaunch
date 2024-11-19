cmake_minimum_required(VERSION 3.10)
project(VenusLaunch)

set(CMAKE_CXX_STANDARD 20)

include_directories("include")

find_package(libzip CONFIG REQUIRED)
find_package(CURL REQUIRED)

set(SOURCES "src/main.cpp" "src/utils.cpp" "src/launch.cpp")
set(PDCURSES_SOURCES "src/pdcurses/addch.c" "src/pdcurses/addchstr.c" "src/pdcurses/addstr.c" "src/pdcurses/attr.c" "src/pdcurses/beep.c" "src/pdcurses/bkgd.c" "src/pdcurses/border.c" "src/pdcurses/clear.c" "src/pdcurses/color.c" "src/pdcurses/debug.c" "src/pdcurses/delch.c" "src/pdcurses/deleteln.c" "src/pdcurses/getch.c" "src/pdcurses/getstr.c" "src/pdcurses/getyx.c" "src/pdcurses/inch.c" "src/pdcurses/inchstr.c" "src/pdcurses/initscr.c" "src/pdcurses/inopts.c" "src/pdcurses/insch.c" "src/pdcurses/insstr.c" "src/pdcurses/instr.c" "src/pdcurses/kernel.c" "src/pdcurses/keyname.c" "src/pdcurses/mouse.c" "src/pdcurses/move.c" "src/pdcurses/outopts.c" "src/pdcurses/overlay.c" "src/pdcurses/pad.c" "src/pdcurses/panel.c" "src/pdcurses/pdcclip.c" "src/pdcurses/pdcdisp.c" "src/pdcurses/pdcgetsc.c" "src/pdcurses/pdckbd.c" "src/pdcurses/pdcscrn.c" "src/pdcurses/pdcsetsc.c" "src/pdcurses/pdcutil.c" "src/pdcurses/printw.c" "src/pdcurses/refresh.c" "src/pdcurses/scanw.c" "src/pdcurses/scroll.c" "src/pdcurses/scr_dump.c" "src/pdcurses/slk.c" "src/pdcurses/termattr.c" "src/pdcurses/touch.c" "src/pdcurses/util.c" "src/pdcurses/window.c")

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
add_executable("VenusLaunch" ${SOURCES} ${PDCURSES_SOURCES})
target_link_libraries("VenusLaunch" PRIVATE libzip::zip CURL::libcurl)