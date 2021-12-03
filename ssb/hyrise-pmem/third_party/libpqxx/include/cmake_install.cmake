# Install script for directory: /hpi/fs00/home/lars.bollmeier/hyrise/third_party/libpqxx/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES
    "/hpi/fs00/home/lars.bollmeier/hyrise/third_party/libpqxx/include/pqxx"
    "/hpi/fs00/home/lars.bollmeier/hyrise/third_party/libpqxx/include/pqxx"
    FILES_MATCHING REGEX "/array\\.hxx$" REGEX "/array$" REGEX "/binarystring\\.hxx$" REGEX "/binarystring$" REGEX "/compiler\\-internal\\-post\\.hxx$" REGEX "/compiler\\-internal\\-post$" REGEX "/compiler\\-internal\\-pre\\.hxx$" REGEX "/compiler\\-internal\\-pre$" REGEX "/compiler\\-internal\\.hxx$" REGEX "/compiler\\-internal$" REGEX "/compiler\\-public\\.hxx$" REGEX "/compiler\\-public$" REGEX "/connection\\.hxx$" REGEX "/connection$" REGEX "/cursor\\.hxx$" REGEX "/cursor$" REGEX "/dbtransaction\\.hxx$" REGEX "/dbtransaction$" REGEX "/errorhandler\\.hxx$" REGEX "/errorhandler$" REGEX "/except\\.hxx$" REGEX "/except$" REGEX "/field\\.hxx$" REGEX "/field$" REGEX "/isolation\\.hxx$" REGEX "/isolation$" REGEX "/largeobject\\.hxx$" REGEX "/largeobject$" REGEX "/nontransaction\\.hxx$" REGEX "/nontransaction$" REGEX "/notification\\.hxx$" REGEX "/notification$" REGEX "/pipeline\\.hxx$" REGEX "/pipeline$" REGEX "/prepared\\_statement\\.hxx$" REGEX "/prepared\\_statement$" REGEX "/result\\.hxx$" REGEX "/result$" REGEX "/result\\_iterator\\.hxx$" REGEX "/result\\_iterator$" REGEX "/robusttransaction\\.hxx$" REGEX "/robusttransaction$" REGEX "/row\\.hxx$" REGEX "/row$" REGEX "/strconv\\.hxx$" REGEX "/strconv$" REGEX "/stream\\_from\\.hxx$" REGEX "/stream\\_from$" REGEX "/stream\\_to\\.hxx$" REGEX "/stream\\_to$" REGEX "/subtransaction\\.hxx$" REGEX "/subtransaction$" REGEX "/transaction\\.hxx$" REGEX "/transaction$" REGEX "/transaction\\_base\\.hxx$" REGEX "/transaction\\_base$" REGEX "/transactor\\.hxx$" REGEX "/transactor$" REGEX "/types\\.hxx$" REGEX "/types$" REGEX "/util\\.hxx$" REGEX "/util$" REGEX "/version\\.hxx$" REGEX "/version$" REGEX "/internal\\/callgate\\.hxx$" REGEX "/internal\\/conversions\\.hxx$" REGEX "/internal\\/encoding\\_group\\.hxx$" REGEX "/internal\\/encodings\\.hxx$" REGEX "/internal\\/ignore\\-deprecated\\-post\\.hxx$" REGEX "/internal\\/ignore\\-deprecated\\-pre\\.hxx$" REGEX "/internal\\/libpq\\-forward\\.hxx$" REGEX "/internal\\/sql\\_cursor\\.hxx$" REGEX "/internal\\/statement\\_parameters\\.hxx$" REGEX "/internal\\/gates\\/connection\\-errorhandler\\.hxx$" REGEX "/internal\\/gates\\/connection\\-largeobject\\.hxx$" REGEX "/internal\\/gates\\/connection\\-notification\\_receiver\\.hxx$" REGEX "/internal\\/gates\\/connection\\-pipeline\\.hxx$" REGEX "/internal\\/gates\\/connection\\-sql\\_cursor\\.hxx$" REGEX "/internal\\/gates\\/connection\\-transaction\\.hxx$" REGEX "/internal\\/gates\\/errorhandler\\-connection\\.hxx$" REGEX "/internal\\/gates\\/icursor\\_iterator\\-icursorstream\\.hxx$" REGEX "/internal\\/gates\\/icursorstream\\-icursor\\_iterator\\.hxx$" REGEX "/internal\\/gates\\/result\\-connection\\.hxx$" REGEX "/internal\\/gates\\/result\\-creation\\.hxx$" REGEX "/internal\\/gates\\/result\\-row\\.hxx$" REGEX "/internal\\/gates\\/result\\-sql\\_cursor\\.hxx$" REGEX "/internal\\/gates\\/transaction\\-sql\\_cursor\\.hxx$" REGEX "/internal\\/gates\\/transaction\\-stream\\_from\\.hxx$" REGEX "/internal\\/gates\\/transaction\\-stream\\_to\\.hxx$" REGEX "/internal\\/gates\\/transaction\\-transactionfocus\\.hxx$" REGEX "/config\\-public\\-compiler\\.h$" REGEX "/pqxx$")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/libpqxx" TYPE DIRECTORY FILES "/hpi/fs00/home/lars.bollmeier/hyrise/third_party/libpqxx/include/pqxx/doc/" FILES_MATCHING REGEX "/README\\.md$" REGEX "/accessing\\-results\\.md$" REGEX "/escaping\\.md$" REGEX "/getting\\-started\\.md$" REGEX "/mainpage\\.md$" REGEX "/performance\\.md$" REGEX "/prepared\\-statement\\.md$" REGEX "/streams\\.md$" REGEX "/thread\\-safety\\.md$")
endif()

