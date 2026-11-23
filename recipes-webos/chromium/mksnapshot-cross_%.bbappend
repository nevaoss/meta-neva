# Copyright 2021 LG Electronics, Inc.

require webruntime-common-neva.inc
PN = "mksnapshot-cross-${TRANSLATED_TARGET_ARCH}"

# Add crypto library for zlib for webOS.
DEPENDS:append = " nss-native"

# To avoid error "--dynamic-linker=: must take a non-empty argument"
BUILD_LDFLAGS:remove = "-Wl,--dynamic-linker="

TARGET = "${V8_SNAPSHOT_TOOLCHAIN}/mksnapshot"

do_configure() {
    PATH="${DEPOT_TOOLS_DIR}:$PATH"

    OUT_DIR_IS_EMPTY=$([ -z "$(ls -A "${OUT_DIR}")" ] && echo "true" || echo "false")
    if [ "${OUT_DIR_IS_EMPTY}" = "false" ]; then
        configure_gn_clean "${OUT_DIR}"
    fi

    rm -rf ${OUT_DIR}/build/

    cd ${S}/src
    gn --root=${S}/src --dotfile=neva/mksnapshot.gn gen ${OUT_DIR} --args="${GN_ARGS}"
}

do_compile() {
    export WEBOS_CHROMIUM_ROOT="${S}/src"

    if [ ! -f ${OUT_DIR}/build.ninja ]; then
        do_configure
    fi

    PATH="${DEPOT_TOOLS_DIR}:$PATH"

    cd ${S}/src
    set_build_tool_and_parallel_make
    ${BUILD_TOOL} ${ADJUSTED_PARALLEL_MAKE} -C ${OUT_DIR} ${TARGET}
}

do_install() {
    echo "Installing ${PN}"
    install -d ${D}${bindir}

    install ${OUT_DIR}/${TARGET} ${D}${bindir}/mksnapshot-cross-${TARGET_ARCH}
}
