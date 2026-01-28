# Copyright 2025 LG Electronics, Inc.

require webruntime-common-neva.inc

LICENSE = "Apache-2.0-with-LLVM-exception"

# These are prebuilt binaries, doesn't make sense to package them as target
# package.
inherit native

OFFSET = "src/third_party/llvm-build/Release+Asserts"
