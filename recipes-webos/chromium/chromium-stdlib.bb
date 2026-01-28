# Copyright 2025 LG Electronics, Inc.

require chromium-stdlib-neva.inc

do_configure() {
    export GYP_CHROMIUM_NO_ACTION=1
    export PATH="${HOSTTOOLS_DIR}:${DEPOT_TOOLS_DIR}:$PATH"

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
    ${BUILD_TOOL} "${ADJUSTED_PARALLEL_MAKE}" -C "${OUT_DIR}" "${TARGET}"
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
