# ====================================================================
# Copyright (c) 2015, Imperial College London
# Copyright (c) 2015, Ghislain Antony Vaillant
# All rights reserved.
#
# Distributed under the terms of the new BSD license.
#
# THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
# ====================================================================

#if(USE_NIFTI)
#    if(NiftiCLib_FOUND)
#        include_directories(${NiftiCLib_INCLUDES})
        add_oiio_plugin(niftiinput.cpp niftioutput.cpp
#                LINK_LIBRARIES ${NiftiCLib_LIBRARIES}
        )
#    else()
#        message(STATUS "Nifti not found: nifti plugin will not be built")
#    endif()
#endif()     
