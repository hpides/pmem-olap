#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libpqxx::pqxx_shared" for configuration "Debug"
set_property(TARGET libpqxx::pqxx_shared APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(libpqxx::pqxx_shared PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libpqxx.so"
  IMPORTED_SONAME_DEBUG "libpqxx.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS libpqxx::pqxx_shared )
list(APPEND _IMPORT_CHECK_FILES_FOR_libpqxx::pqxx_shared "${_IMPORT_PREFIX}/lib/libpqxx.so" )

# Import target "libpqxx::pqxx_static" for configuration "Debug"
set_property(TARGET libpqxx::pqxx_static APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(libpqxx::pqxx_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libpqxx.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS libpqxx::pqxx_static )
list(APPEND _IMPORT_CHECK_FILES_FOR_libpqxx::pqxx_static "${_IMPORT_PREFIX}/lib/libpqxx.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
