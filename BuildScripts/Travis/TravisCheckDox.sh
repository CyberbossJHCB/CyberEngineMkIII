#!/bin/bash

fail=0

#Check all 3 platforms for dox errors
mkdir TestDocs
((cat 'BuildScripts/Doxygen/CyberEngineMkIIIDoxy.cfg' ; echo 'EXPAND_AS_DEFINED = DEBUG TARGET_OS_WINDOWS' ; echo 'OUTPUT_DIRECTORY = TestDocs'; echo 'EXCLUDE_PATTERNS = CYBPosix* CYBLinux* CYBOSX*  *cotire*') | doxygen - 2>&1) | tee doxygen.log

if grep -q -i "warning" "doxygen.log"; then
	echo 'Warnings detected in Windows documentation. Failing build!'
	fail=1
 fi
 
((cat 'BuildScripts/Doxygen/CyberEngineMkIIIDoxy.cfg' ; echo 'EXPAND_AS_DEFINED = DEBUG TARGET_OS_MAC' ; echo 'OUTPUT_DIRECTORY = TestDocs'; echo 'EXCLUDE_PATTERNS = CYBWin32* CYBOSX* *cotire*') | doxygen - 2>&1) | tee doxygen.log

if grep -q -i "warning" "doxygen.log"; then
	echo 'Warnings detected in OSX documentation. Failing build!'
	fail=1
 fi
 
((cat 'BuildScripts/Doxygen/CyberEngineMkIIIDoxy.cfg' ; echo 'EXPAND_AS_DEFINED = DEBUG TARGET_OS_LINUX' ; echo 'OUTPUT_DIRECTORY = TestDocs'; echo 'EXCLUDE_PATTERNS = CYBLinux* CYBWin32* *cotire*') | doxygen - 2>&1) | tee doxygen.log

if grep -q -i "warning" "doxygen.log"; then
	echo 'Warnings detected in Linux documentation. Failing build!'
	fail=1
 fi

 if [ "$fail" -eq "1" ]; then
    exit 1;
fi