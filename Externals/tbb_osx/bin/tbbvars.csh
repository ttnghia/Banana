#!/bin/csh
#
# Copyright (c) 2005-2016 Intel Corporation
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


setenv TBBROOT "SUBSTITUTE_INSTALL_DIR_HERE" 
if (! $?LIBRARY_PATH) then 
    setenv LIBRARY_PATH "${TBBROOT}/lib" 
else
    setenv LIBRARY_PATH "${TBBROOT}/lib:$LIBRARY_PATH" 
endif
if (! $?DYLD_LIBRARY_PATH) then 
    setenv DYLD_LIBRARY_PATH "${TBBROOT}/lib" 
else
    setenv DYLD_LIBRARY_PATH "${TBBROOT}/lib:$DYLD_LIBRARY_PATH" 
endif
if (! $?CPATH) then 
    setenv CPATH "${TBBROOT}/include" 
else 
    setenv CPATH "${TBBROOT}/include:$CPATH" 
endif 
