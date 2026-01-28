# Copyright 2025 LG Electronics, Inc.

# NOTE: this file is almost a full copy of
# //meta-lg-webos/meta-webos/classes/clang_cmake.bbclass, but without
# DEPENDS:append = " clang-native"
# because we use chromium-toolchain-native instead.

inherit webos_cmake
inherit clang_libc

OECMAKE_C_COMPILER = "clang"
OECMAKE_CXX_COMPILER = "clang++"

LIBCBE_DIR = "${libdir}/cbe"

CLANG_DEPENDENCY_SUFFIX = "-clang"

TOOLCHAIN_OPTIONS = "\
    --sysroot=${STAGING_DIR_TARGET} \
    --target=${TARGET_SYS} \
    -stdlib=libc++ \
    -fuse-ld=${STAGING_BINDIR_NATIVE}/ld.lld \
    -nostdinc++ \
    -isystem ${STAGING_INCDIR}/c++/v1/ \
    -Wl,-L${STAGING_DIR_TARGET}/${LIBCBE_DIR} \
    -Wl,-rpath,${LIBCBE_DIR} \
    -Wno-unused-command-line-argument \
    -D_LIBCPP_HAS_NO_VENDOR_AVAILABILITY_ANNOTATIONS \
    -D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_NONE \
"

TOOLCHAIN_OPTIONS:append = " ${@bb.utils.contains('USE_WEBRUNTIME_LIBCXX', '1', '-D_LIBCPP_ABI_UNSTABLE', '', d)}"

# Pass dyld-prefix with usrmerge otherwise the default loader from clang++ will be non-existent (on target)
# /lib64/ld-linux-x86-64.so.2 instead of expected /usr/lib/ld-linux-x86-64.so.2 for qemux86-64.
TUNE_CCARGS:append = "${@bb.utils.contains("DISTRO_FEATURES", "usrmerge", " --dyld-prefix=/usr", "", d)}"

# The same as in
# https://github.com/kraj/meta-clang/commit/4cf1e9e0baf30568851c6646510d18bad50c613b
# Without it building of some recipes fails.
DEBUG_PREFIX_MAP:remove = "-fcanon-prefix-map"
