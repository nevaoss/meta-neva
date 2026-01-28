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

    if [ "${USE_SISO}" = "true" ]; then
        # Copy Chromium's Siso config into OUT_DIR to make Siso treat OUT_DIR
        # as part of exec_root. By default, Siso determines exec_root by locating
        # build/config/siso, which normally exists under chromium/src. Without this
        # copy, OUT_DIR outside chromium/src is rejected as "out of exec root".
        mkdir -p ${OUT_DIR}/build/config
        cp -r ${S}/src/build/config/siso ${OUT_DIR}/build/config/
    else
        rm -rf ${OUT_DIR}/build/
    fi

    cd ${S}/src
    gn --root=${S}/src --dotfile=neva/mksnapshot.gn gen ${OUT_DIR} --args="${GN_ARGS}"
}

do_compile() {
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
