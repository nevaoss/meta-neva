# Copyright 2021 LG Electronics, Inc.

# v8 snapshot toolchain settings, webOS/OSE specifics.
#
# v8 snapshot toolchain settings should be set for both webruntime and
# mksnapshot targets, hence it is placed in the common place here.
#
# See NEVA-5683 comments for rationale.
# We don't have the cros:v8_snapshot_clang_${TARGET_CPU} which RP/Blink has, so
# we utilized the standard cros:v8_snapshot with a cros_v8_snapshot_sysroot="/"
# trick, to 'cross' (x64 -> x86) compile v8 snapshot tools in order to run them
# on host.
# /-- v8_snapshot_toolchain settings begin
V8_SNAPSHOT_TOOLCHAIN:qemux86 = "v8_snapshot"
GN_ARGS:append:qemux86 = " v8_snapshot_toolchain=\"//build/toolchain/cros:${V8_SNAPSHOT_TOOLCHAIN}\" cros_v8_snapshot_sysroot=\"/\""

V8_SNAPSHOT_TOOLCHAIN:qemux86-64 = "v8_snapshot"
GN_ARGS:append:qemux86-64 = " v8_snapshot_toolchain=\"//build/toolchain/cros:${V8_SNAPSHOT_TOOLCHAIN}\" cros_v8_snapshot_sysroot=\"/\""
# \-- v8_snapshot_toolchain settings end

# It is needed to fix webOS/OSE QEMUx86 build error,
# see http://jira.lge.com/issue/browse/NEVA-10680.
GN_ARGS:append:qemux86-64 = " clang_base_path=\"${S}/src/third_party/llvm-build/Release+Asserts/\" "
