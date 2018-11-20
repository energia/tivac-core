#!/bin/bash 
#-ex 


# Copyright (c) 2018 StefanSch
# 
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
# 
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

# package GCC toolchain for Energia
#
# prerequisites
# - bash
# - wget, GNU patch, GNU make
# - things needed to build binutils, GCC and GDB
#

set -e

source ./extras/versions.sh
source ./extras/setup.bash

 
echo '--- do compiler packages'
echo 'prepare gcc'
echo "this needs to be already available online at: ${DSLITE_URL}"
m_download "${DSLITE_URL}/windows/gcc-arm-none-eabi-${GCC_VER}-windows.tar.bz2"
cp  extras/download/gcc-arm-none-eabi-${GCC_VER}-windows.tar.bz2 extras/build/windows/
m_download "${DSLITE_URL}/macosx/gcc-arm-none-eabi-${GCC_VER}-mac.tar.bz2"
cp  extras/download/gcc-arm-none-eabi-${GCC_VER}-mac.tar.bz2 extras/build/macos/
m_download "${DSLITE_URL}/linux64/gcc-arm-none-eabi-${GCC_VER}-x86_64-pc-linux-gnu.tar.bz2"
cp  extras/download/gcc-arm-none-eabi-${GCC_VER}-x86_64-pc-linux-gnu.tar.bz2 extras/build/linux64/
for filename in $(find extras/build/ -name 'gcc-arm-none-eabi-*.sha256' ); do
    rm "$filename" 
done 
for filename in $(find extras/build/ -name 'gcc-arm-none-eabi-*' ); do
    shasum -a 256 "$filename" >"$filename".sha256
done

