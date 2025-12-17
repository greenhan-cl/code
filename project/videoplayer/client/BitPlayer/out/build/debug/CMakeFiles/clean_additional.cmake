# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "BitPlayer_autogen"
  "CMakeFiles\\BitPlayer_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\BitPlayer_autogen.dir\\ParseCache.txt"
  )
endif()
