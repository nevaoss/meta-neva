# Copyright 2025 LG Electronics, Inc.

require chromium-stdlib-neva.inc

# Siso determines the root of the Chromium project based on the directory
# passed with the -C parameter. OUT_DIR is located outside the Chromium
# gclient branch when using externalsrc, so we define SISO_OUT_DIR.
SISO_OUT_DIR = "${S}/src/oe-workdir/${BP}/${BUILD_TYPE}"

do_configure() {
    export GYP_CHROMIUM_NO_ACTION=1
    export PATH="${HOSTTOOLS_DIR}:${DEPOT_TOOLS_DIR}:$PATH"

    OUT_DIR_IS_EMPTY=$([ -z "$(ls -A "${OUT_DIR}")" ] && echo "true" || echo "false")
    if [ "${OUT_DIR_IS_EMPTY}" = "false" ]; then
        configure_gn_clean "${OUT_DIR}"
    fi

    if [ "${USE_SISO}" != "true" ]; then
        rm -rf ${OUT_DIR}/build/
    fi

    cd "${S}/src"
    gn --root=${S}/src --dotfile="neva/libc++.gn" gen "${OUT_DIR}" --args="${GN_ARGS}"
}

do_compile() {
    if [ ! -f "${OUT_DIR}/build.ninja" ]; then
        do_configure
    fi

    PATH="${DEPOT_TOOLS_DIR}:$PATH"

    cd "${S}/src"
    set_build_tool_and_parallel_make
    ${BUILD_TOOL} "${ADJUSTED_PARALLEL_MAKE}" -C "${SISO_OUT_DIR}" "${TARGET}"
}

do_install() {
    offset_libcxx="src/third_party/libc++/src/include"
    offset_libcxxabi="src/third_party/libc++abi/src/include"
    install -d "${D}${includedir}/c++/v1"
    cp -R --no-dereference --preserve=mode,links -v \
        "${S}/${offset_libcxx}"/* "${D}${includedir}/c++/v1"
    cp -R --no-dereference --preserve=mode,links -v \
        "${S}/${offset_libcxxabi}"/* "${D}${includedir}/c++/v1"

    install -v -d "${D}/${LIBCBE_DIR}"
    install -v -m 644 "${OUT_DIR}/libc++.so" "${D}/${LIBCBE_DIR}"

    cp -R --no-dereference --preserve=mode,links -v \
        "${S}/src/buildtools/third_party/libc++/__config_site" "${D}${includedir}/c++/v1"
    cp -R --no-dereference --preserve=mode,links -v \
        "${S}/src/buildtools/third_party/libc++/__assertion_handler" "${D}${includedir}/c++/v1"
}

# Use do_gclient_sync() from webruntime-clang instead of own implemented to have
# an ability to use the same Chromium source from externalsrc for
# webruntime-clang, chromium-toolchain-native and chromium-stdlib. Otherwise,
# errors occurs because of do_gclient_sync() funcs running in parallel for
# different recipes.
do_configure[depends] += "webruntime-clang:do_gclient_sync"
