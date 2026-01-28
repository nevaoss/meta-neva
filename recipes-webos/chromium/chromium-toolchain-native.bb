# Copyright 2025 LG Electronics, Inc.

require chromium-toolchain-native-neva.inc

do_install() {
    CLANG_VERSION="$(ls ${S}/${OFFSET}/lib/clang)"

    install -d ${D}${bindir}
    cp -R --no-dereference --preserve=mode,links -v ${S}/${OFFSET}/bin/* ${D}${bindir}

    install -d ${D}${libdir}/clang/${CLANG_VERSION}/
    cp -R --no-dereference --preserve=mode,links -v \
        ${S}/${OFFSET}/lib/clang/${CLANG_VERSION}/include ${D}${libdir}/clang/${CLANG_VERSION}
}

# Use do_gclient_sync() from webruntime-clang instead of own implemented to have
# an ability to use the same Chromium source from externalsrc for
# webruntime-clang, chromium-toolchain-native and chromium-stdlib. Otherwise,
# errors occurs because of do_gclient_sync() funcs running in parallel for
# different recipes.
do_install[depends] += "webruntime-clang:do_gclient_sync"
