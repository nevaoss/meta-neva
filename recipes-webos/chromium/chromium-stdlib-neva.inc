# Copyright 2025 LG Electronics, Inc.

inherit clang_libc
inherit pkgconfig

require ${META_WEBOS_LAYER}/recipes-webos/chromium/webruntime-common.inc
require webruntime-common-neva.inc

LICENSE = "Apache-2.0-with-LLVM-exception"

DEPENDS += "glib-2.0"

GN_ARGS += "libcxx_is_shared=true"
GN_ARGS += "is_clang=true"
GN_ARGS += "use_custom_libcxx=true"
GN_ARGS += "target_sysroot=\"${STAGING_DIR_TARGET}\""
GN_ARGS:remove = "use_gold=true"

TARGET = "libc++"

LIBCBE_DIR = "${libdir}/cbe"

FILES:${PN} = " \
    ${LIBCBE_DIR}/libc++.so \
"

FILES:${PN}-dev = " \
    ${includedir} \
"
