
if(NOT "C:/Src/SecondTry/out/build/x64-Debug/_deps/bzip2-subbuild/bzip2-populate-prefix/src/bzip2-populate-stamp/bzip2-populate-gitinfo.txt" IS_NEWER_THAN "C:/Src/SecondTry/out/build/x64-Debug/_deps/bzip2-subbuild/bzip2-populate-prefix/src/bzip2-populate-stamp/bzip2-populate-gitclone-lastrun.txt")
  message(STATUS "Avoiding repeated git clone, stamp file is up to date: 'C:/Src/SecondTry/out/build/x64-Debug/_deps/bzip2-subbuild/bzip2-populate-prefix/src/bzip2-populate-stamp/bzip2-populate-gitclone-lastrun.txt'")
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E rm -rf "C:/Src/SecondTry/out/build/x64-Debug/src/3rdParty/bzip2-build"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: 'C:/Src/SecondTry/out/build/x64-Debug/src/3rdParty/bzip2-build'")
endif()

# try the clone 3 times in case there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "C:/Program Files/Git/cmd/git.exe"  clone --no-checkout --config "advice.detachedHead=false" "https://gitlab.com/bzip2/bzip2.git" "bzip2-build"
    WORKING_DIRECTORY "C:/Src/SecondTry/out/build/x64-Debug/src/3rdParty"
    RESULT_VARIABLE error_code
    )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once:
          ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://gitlab.com/bzip2/bzip2.git'")
endif()

execute_process(
  COMMAND "C:/Program Files/Git/cmd/git.exe"  checkout 2d8393924b9f3e014000c7420c7da7c3ddb74e2c --
  WORKING_DIRECTORY "C:/Src/SecondTry/out/build/x64-Debug/src/3rdParty/bzip2-build"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: '2d8393924b9f3e014000c7420c7da7c3ddb74e2c'")
endif()

set(init_submodules TRUE)
if(init_submodules)
  execute_process(
    COMMAND "C:/Program Files/Git/cmd/git.exe"  submodule update --recursive --init 
    WORKING_DIRECTORY "C:/Src/SecondTry/out/build/x64-Debug/src/3rdParty/bzip2-build"
    RESULT_VARIABLE error_code
    )
endif()
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: 'C:/Src/SecondTry/out/build/x64-Debug/src/3rdParty/bzip2-build'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy
    "C:/Src/SecondTry/out/build/x64-Debug/_deps/bzip2-subbuild/bzip2-populate-prefix/src/bzip2-populate-stamp/bzip2-populate-gitinfo.txt"
    "C:/Src/SecondTry/out/build/x64-Debug/_deps/bzip2-subbuild/bzip2-populate-prefix/src/bzip2-populate-stamp/bzip2-populate-gitclone-lastrun.txt"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: 'C:/Src/SecondTry/out/build/x64-Debug/_deps/bzip2-subbuild/bzip2-populate-prefix/src/bzip2-populate-stamp/bzip2-populate-gitclone-lastrun.txt'")
endif()
