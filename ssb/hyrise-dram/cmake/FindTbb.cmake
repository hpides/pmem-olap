# Find the Intel Thread Building Blocks library.
# Output variables:
#  TBB_INCLUDE_DIR : e.g., /usr/include/.
#  TBB_LIBRARY     : Library path of Intel Threading Building Blocks library
#  TBB_FOUND       : True if found.
FIND_PATH(TBB_INCLUDE_DIR NAME tbb/tbb.h
    HINTS /opt/intel/oneapi/tbb/latest/include/)

FIND_LIBRARY(TBB_LIBRARY NAME tbb
    HINTS /opt/intel/oneapi/tbb/latest/lib/intel64/gcc4.8/)

IF (TBB_INCLUDE_DIR AND TBB_LIBRARY)
    SET(TBB_FOUND TRUE)
    MESSAGE(STATUS "Found tbb library: inc=${TBB_INCLUDE_DIR}, lib=${TBB_LIBRARY}")
ELSE ()
    SET(TBB_FOUND FALSE)
    MESSAGE(STATUS "WARNING: tbb library not found.")
ENDIF ()
