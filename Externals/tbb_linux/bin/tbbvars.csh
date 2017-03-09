#!/bin/csh
#
# Copyright (c) 2005-2017 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#
#
#


# Parsing script arguments
# Arg1 represents target architecture. Its possible values are 'ia32' or 'intel64',
# default value equals to the value of $COMPILERVARS_ARCHITECTURE environment variable.
if ("$1" == "ia32" || "$1" == "intel64") then
    set TBB_TARGET_ARCH="$1"
else
    if ($?COMPILERVARS_ARCHITECTURE) then
        if ("$COMPILERVARS_ARCHITECTURE" == "ia32" || "$COMPILERVARS_ARCHITECTURE" == "intel64") then
            set TBB_TARGET_ARCH=$COMPILERVARS_ARCHITECTURE
        else
            echo "ERROR: Unknown switch '$COMPILERVARS_ARCHITECTURE'. Accepted values: ia32, intel64"
            exit 1
        endif
    else
        echo "ERROR: Unknown switch '$1'. Accepted values: ia32, intel64"
        exit 1
    endif
endif

# Arg2 represents target platform. Its possible values are 'android' or 'linux'.
# If $COMPILERVARS_PLATFORM environment variable is defined,
# the default value of $TBB_TARGET_PLATFORM equals to its value.
# Otherwise it equals to 'linux'.
if ("$2" == "linux" || "$2" == "android") then
    set TBB_TARGET_PLATFORM=$2
else
    if ($?COMPILERVARS_PLATFORM) then
        if ("$COMPILERVARS_PLATFORM" == "linux" || "$COMPILERVARS_PLATFORM" == "android") then
            set TBB_TARGET_PLATFORM=$COMPILERVARS_PLATFORM
        else
            set TBB_TARGET_PLATFORM="linux"
        endif
    else
        set TBB_TARGET_PLATFORM="linux"
    endif
endif

# Arg3 represents TBBROOT detection method. Its possible value is 'auto_tbbroot'. In which case
# the environment variable TBBROOT is detected automatically by using the script directory path.
if ("$3" == "auto_tbbroot") then
    set sourced=($_)
    if ("$sourced" != '') then # if the script was sourced
        set script_name=`readlink -f $sourced[2]`
    else # if the script was run => "$_" is empty
        set script_name=`readlink -f $0`
    endif
    set script_dir=`dirname $script_name`
    setenv TBBROOT "$script_dir/.."
else
    setenv TBBROOT "SUBSTITUTE_INSTALL_DIR_HERE"
endif

if ("$TBB_TARGET_PLATFORM" != "android") then
    which gcc >/dev/null
    if ($status == 0) then
        set gcc_version_full=`gcc --version | grep "gcc"| egrep -o " [0-9]+\.[0-9]+\.[0-9]+.*" | sed -e "s/^\ //"`
        if ($status == 0) then
            set gcc_version=`echo "$gcc_version_full" | egrep -o "^[0-9]+\.[0-9]+\.[0-9]+"`
        endif
        switch ("$gcc_version")
        case 4.[7-9]*:
        case [5-9]*:
            set library_directory="${TBB_TARGET_ARCH}/gcc4.7"
            breaksw
        case 4.[4-6]*:
            set library_directory="${TBB_TARGET_ARCH}/gcc4.4"
            breaksw
        default:
            set library_directory="${TBB_TARGET_ARCH}/gcc4.1"
            breaksw
        endsw
    else
        echo "WARNING: 'gcc' was not found"
        set library_directory="${TBB_TARGET_ARCH}/gcc4.1"
    endif
else
    if ($TBB_TARGET_PLATFORM == "android") then
        if ($TBB_TARGET_ARCH == "ia32") then
            set library_directory=$TBB_TARGET_PLATFORM
        else
            if ($TBB_TARGET_ARCH == "intel64") then
                set library_directory="${TBB_TARGET_PLATFORM}/x86_64"
            endif
        endif
    else
        set library_directory=""
    endif
endif

if ( -e "${TBBROOT}/lib/mic/libtbb.so" ) then
    if (! $?MIC_LD_LIBRARY_PATH) then
        setenv MIC_LD_LIBRARY_PATH "${TBBROOT}/lib/mic"
    else
        setenv MIC_LD_LIBRARY_PATH "${TBBROOT}/lib/mic:$MIC_LD_LIBRARY_PATH"
    endif

    if (! $?MIC_LIBRARY_PATH) then
        setenv MIC_LIBRARY_PATH "${TBBROOT}/lib/mic"
    else
        setenv MIC_LIBRARY_PATH "${TBBROOT}/lib/mic:$MIC_LIBRARY_PATH"
    endif
endif

if (! $?LD_LIBRARY_PATH) then
    setenv LD_LIBRARY_PATH "${TBBROOT}/lib/${library_directory}"
else
    setenv LD_LIBRARY_PATH "${TBBROOT}/lib/${library_directory}:$LD_LIBRARY_PATH"
endif

if (! $?LIBRARY_PATH) then
    setenv LIBRARY_PATH "${TBBROOT}/lib/${library_directory}"
else
    setenv LIBRARY_PATH "${TBBROOT}/lib/${library_directory}:$LIBRARY_PATH"
endif

if (! $?CPATH) then
    setenv CPATH "${TBBROOT}/include"
else
    setenv CPATH "${TBBROOT}/include:$CPATH"
endif
