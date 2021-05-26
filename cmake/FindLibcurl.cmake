# - Try to find Libcurl includes dirs and libraries
#
# Usage of this module as follows:
#
#     find_package(Libcurl)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
# Variables defined by this module:
#
#  Libcurl_FOUND            System has Libcurl, include and lib dirs found
#  Libcurl_INCLUDE_DIR      The Libcurl includes directories.
#  Libcurl_LIBRARY          The Libcurl library.

find_path(Libcurl_INCLUDE_DIR NAMES curl/curl.h PATHS /usr/local/include)
find_library(Libcurl_LIBRARY NAMES  libcurl curl PATHS /usr/local/lib)

if(Libcurl_INCLUDE_DIR AND Libcurl_LIBRARY)
    set(Libcurl_FOUND TRUE)
    mark_as_advanced(
        Libcurl_INCLUDE_DIR
        Libcurl_LIBRARY
    )
endif()

if(NOT Libcurl_FOUND)
    message(FATAL_ERROR "Libcurl doesn't exist")
endif()