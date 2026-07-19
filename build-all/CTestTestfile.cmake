# CMake generated Testfile for 
# Source directory: /home/ratana/RatanaOS
# Build directory: /home/ratana/RatanaOS/build-all
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[desktop-smoke]=] "/home/ratana/RatanaOS/tests/smoke_desktop.sh" "/home/ratana/RatanaOS")
set_tests_properties([=[desktop-smoke]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/ratana/RatanaOS/CMakeLists.txt;28;add_test;/home/ratana/RatanaOS/CMakeLists.txt;0;")
subdirs("apps")
subdirs("desktop")
subdirs("system/manager")
subdirs("system/update-daemon")
