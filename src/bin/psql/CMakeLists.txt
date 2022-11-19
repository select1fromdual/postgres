cmake_minimum_required(VERSION 3.16)
project(psql)

add_subdirectory(_deps/spdlog)

list(APPEND srcs
  startup.cpp
  command.cpp
  common.cpp
  copy.cpp
  crosstabview.cpp
  describe.cpp
  help.cpp
  psqlscanslash.cpp
  stringutils.cpp
  sql_help.cpp
  input.cpp
  large_obj.cpp
  mainloop.cpp
  prompt.cpp
  tab-complete.cpp
  variables.cpp
  logger.cpp
)

add_executable(psql ${srcs})
target_link_libraries(psql pq)
target_link_directories(psql PUBLIC /home/esoye/postgres/__build/lib)
target_include_directories(psql PUBLIC 
  /home/esoye/postgres/__build/include
  /home/esoye/postgres/__build/include/lib
  /home/esoye/postgres/__build/include/server
  /home/esoye/postgres/__build/include/internal
  /home/esoye/postgres/__build/making/src/include
  /home/esoye/postgres/__build/making/src/interfaces/libpq
)
target_include_directories(psql PUBLIC _deps/spdlog/include)

target_link_libraries(psql readline)
target_compile_definitions(psql PUBLIC HAVE_READLINE_READLINE_H)

target_link_libraries(psql 
/home/esoye/postgres/__build/making/src/common/exec.o
/home/esoye/postgres/__build/making/src/common/string.o
/home/esoye/postgres/__build/making/src/common/keywords.o
/home/esoye/postgres/__build/making/src/common/kwlookup.o
/home/esoye/postgres/__build/making/src/common/pg_get_line.o
/home/esoye/postgres/__build/making/src/common/stringinfo.o
/home/esoye/postgres/__build/making/src/common/fe_memutils.o
/home/esoye/postgres/__build/making/src/port/path.o
/home/esoye/postgres/__build/making/src/port/thread.o
/home/esoye/postgres/__build/making/src/port/snprintf.o
/home/esoye/postgres/__build/making/src/port/strlcpy.o
/home/esoye/postgres/__build/making/src/port/pgstrcasecmp.o
/home/esoye/postgres/__build/making/src/port/pgstrsignal.o
/home/esoye/postgres/__build/making/src/port/strerror.o
/home/esoye/postgres/__build/making/src/fe_utils/print.o
/home/esoye/postgres/__build/making/src/fe_utils/cancel.o
/home/esoye/postgres/__build/making/src/fe_utils/mbprint.o
/home/esoye/postgres/__build/making/src/fe_utils/psqlscan.o
/home/esoye/postgres/__build/making/src/fe_utils/string_utils.o
/home/esoye/postgres/__build/making/src/common/sprompt.o
/home/esoye/postgres/__build/making/src/common/psprintf.o
/home/esoye/postgres/__build/making/src/common/wait_error.o
/home/esoye/postgres/__build/making/src/common/username.o
/home/esoye/postgres/__build/making/src/fe_utils/psqlscan.o
/home/esoye/postgres/__build/making/src/fe_utils/conditional.o
)

