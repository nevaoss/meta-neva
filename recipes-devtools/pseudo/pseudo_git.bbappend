# Copyright 2026 LG Electronics, Inc.

EXTENDPRAUTO:append = "neva1"

# Use the newer pseudo revision (1.9.8, 2026-06-03) which supports openat2().
SRCREV = "823895ba708c63f6ae4dcbfc266210f26c02c698"

# These patches are already integrated or no longer applicable to the newer
# pseudo revision.
SRC_URI:remove = " \
    file://0001-configure-Prune-PIE-flags.patch \
    file://glibc238.patch \
"
SRC_URI:remove:class-native = " \
    file://older-glibc-symbols.patch \
"
SRC_URI:remove:class-nativesdk = " \
    file://older-glibc-symbols.patch \
"
