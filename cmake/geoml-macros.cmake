#
# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2018 German Aerospace Center (DLR)
#

# gets the list of all subdirectories relative to curdir
# example use:
#   SUBDIRLIST(SUBDIRS ${MY_CURRENT_DIR})
macro(SUBDIRLIST result curdir)
  file(GLOB children RELATIVE ${curdir} ${curdir}/*)
  set(dirlist "")
  foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
        set(dirlist ${dirlist} ${child})
    endif()
  endforeach()
  set(${result} ${dirlist})
endmacro()
