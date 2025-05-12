#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "sfml-system" for configuration "Debug"
set_property(TARGET sfml-system APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(sfml-system PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/libsfml-system-d.a"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/sfml-system-d-2.dll"
  )

list(APPEND _cmake_import_check_targets sfml-system )
list(APPEND _cmake_import_check_files_for_sfml-system "${_IMPORT_PREFIX}/lib/libsfml-system-d.a" "${_IMPORT_PREFIX}/bin/sfml-system-d-2.dll" )

# Import target "sfml-main" for configuration "Debug"
set_property(TARGET sfml-main APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(sfml-main PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libsfml-main-d.a"
  )

list(APPEND _cmake_import_check_targets sfml-main )
list(APPEND _cmake_import_check_files_for_sfml-main "${_IMPORT_PREFIX}/lib/libsfml-main-d.a" )

# Import target "sfml-window" for configuration "Debug"
set_property(TARGET sfml-window APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(sfml-window PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/libsfml-window-d.a"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/sfml-window-d-2.dll"
  )

list(APPEND _cmake_import_check_targets sfml-window )
list(APPEND _cmake_import_check_files_for_sfml-window "${_IMPORT_PREFIX}/lib/libsfml-window-d.a" "${_IMPORT_PREFIX}/bin/sfml-window-d-2.dll" )

# Import target "sfml-graphics" for configuration "Debug"
set_property(TARGET sfml-graphics APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(sfml-graphics PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/libsfml-graphics-d.a"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/sfml-graphics-d-2.dll"
  )

list(APPEND _cmake_import_check_targets sfml-graphics )
list(APPEND _cmake_import_check_files_for_sfml-graphics "${_IMPORT_PREFIX}/lib/libsfml-graphics-d.a" "${_IMPORT_PREFIX}/bin/sfml-graphics-d-2.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
