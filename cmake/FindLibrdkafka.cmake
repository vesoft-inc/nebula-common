# - Try to find Librdkafka includes dirs and libraries
#
# Usage of this module as follows:
#
#     find_package(Librdkafka)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
# Variables defined by this module:
#
#  Librdkafka_FOUND            System has Librdkafka, include and lib dirs found
#  Librdkafka_INCLUDE_DIR      The Librdkafka includes directories.
#  Librdkafka_LIBRARY          The Librdkafka library.

find_path(Librdkafka_INCLUDE_DIR NAMES librdkafka/rdkafkacpp.h)
find_library(Librdkafka_LIBRARY NAMES librdkafka++ librdkafka)

if(Librdkafka_INCLUDE_DIR AND Librdkafka_LIBRARY)
    set(Librdkafka_FOUND TRUE)
    mark_as_advanced(
        Librdkafka_INCLUDE_DIR
        Librdkafka_LIBRARY
    )
endif()

if(NOT Librdkafka_FOUND)
    message(FATAL_ERROR "Librdkafka doesn't exist")
endif()
