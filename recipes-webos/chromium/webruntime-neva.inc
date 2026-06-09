# Copyright 2025 LG Electronics, Inc.

require webruntime-common-neva.inc

# Add required DEPENDS which missed in parent meta
DEPENDS:append = " libva"

# Enable this in webos-local.conf or in recipe to build
DEPLOY_CHROMEDRIVER = "true"
DEPLOY_WAM_DEMO = "true"
DEPLOY_BROWSER = "false"
DEPLOY_UNITTESTS = "false"

BROWSERSHELL_APPLICATION = "browser-shell"
BROWSERSHELL_APPLICATION_DIR = "${bindir}/${BROWSERSHELL_APPLICATION}"

APPSHELL_BUILD_TARGET = "${@oe.utils.conditional('DEPLOY_BROWSERSHELL', 'true', '', 'app_shell', d)}"

UNITTESTS_TARGETS = "${@oe.utils.conditional('DEPLOY_UNITTESTS', 'true', 'media:media_unittests', '', d)}"
UNITTESTS_PACKAGE = "${@oe.utils.conditional('DEPLOY_UNITTESTS', 'true', 'neva-unittests', '', d)}"

PACKAGECONFIG[debug] = "symbol_level=2,symbol_level=0"

# TODO: remove it when RP submits http://gpro.lge.com/#/c/259070/1/meta-webos/recipes-webos/chromium/webruntime.inc
PACKAGECONFIG[debug-webcore] = ""
PACKAGECONFIG[debug-blink] = "blink_symbol_level=1,blink_symbol_level=0"

PACKAGECONFIG[v8_lite] = "v8_enable_lite_mode=true,v8_enable_lite_mode=false"

GN_ARGS:append:aarch64 = " arm_control_flow_integrity=\"none\""

# /-- Remove these no more existing Chromium build flags
GN_ARGS:remove = "closure_compile=false"
GN_ARGS:remove = "compress_debug_info=false"
GN_ARGS:remove = "cros_host_is_clang=false"
GN_ARGS:remove = "disable_ftp_support=true"
GN_ARGS:remove = "use_gnome_keyring=false"
GN_ARGS:remove = "use_local_storage_manager=true"
PACKAGECONFIG[jumbo] = ""
# \--

GN_ARGS:remove = "ozone_platform_wayland_external=true"
GN_ARGS:remove = "use_xkbcommon=true"
PACKAGECONFIG[google_ozone_wayland] = "ozone_platform_wayland=true"
PACKAGECONFIG += "google_ozone_wayland"
PACKAGECONFIG[intel_ozone_wayland] = ""
# IOW compilation is switched off by default
#PACKAGECONFIG += "intel_ozone_wayland"
# Below PACKAGECONFIG:remove required to cover
# PACKAGECONFIG += "intel_ozone_wayland"
# in the meta-lg-webos/meta-webos/recipes-webos/chromium/webruntime_%.bb
PACKAGECONFIG:remove = "intel_ozone_wayland"

PACKAGECONFIG[wayland_gbm] = "use_wayland_gbm=true,use_wayland_gbm=false"

PACKAGECONFIG += "av1-codec"

# neva-media feature and its related features are removed from Neva
PACKAGECONFIG:remove = "neva-media gstreamer umediaserver gav neva-webrtc"

# TODO: get rid of this when it will be removed on RP side from
# meta-lg-webos/meta-webos/recipes-webos/chromium/webruntime.inc
GN_ARGS:remove = "linux_use_bundled_binutils=false"

# TODO: get rid of this when (and if) we adopt GPU info collector patch
GN_ARGS:remove = "use_webos_gpu_info_collector=true"
# TODO: get rid of this when (and if) we adopt pmlog patch
GN_ARGS:remove = "use_pmlog=true"
# TODO: get rid of this when we adopt system debugger patch
GN_ARGS:remove = "use_system_debugger_abort=true"

GN_ARGS:remove = "${@oe.utils.conditional('DEPLOY_BROWSERSHELL', 'true', 'is_app_shell_cbe=true', '', d)}"

GN_ARGS:append = " \
  webos_sysbus_manifestsdir=\"${webos_sysbus_manifestsdir}\"\
  webos_sysbus_pubservicesdir=\"${webos_sysbus_pubservicesdir}\"\
  webos_sysbus_prvservicesdir=\"${webos_sysbus_prvservicesdir}\"\
  webos_sysbus_pubrolesdir=\"${webos_sysbus_pubrolesdir}\"\
  webos_sysbus_prvrolesdir=\"${webos_sysbus_prvrolesdir}\"\
  webos_sysbus_permissionsdir=\"${webos_sysbus_permissionsdir}\"\
  webos_sysbus_rolesdir=\"${webos_sysbus_rolesdir}\"\
  webos_sysbus_containersdir=\"${webos_sysbus_containersdir}\"\
"

addtask do_gclient_sync before do_configure

# It's needed to use MACHINE in do_gclient_sync()
# and apply different patches for different MACHINEs.
export MACHINE

do_gclient_sync() {
    export GYP_CHROMIUM_NO_ACTION=1
    export PATH="${HOSTTOOLS_DIR}:${DEPOT_TOOLS_DIR}:$PATH"

    cd ${S}
    GCLIENT_CACHE="${GCLIENT_CACHE}"
    gclient config ${GCLIENT_CACHE:+--cache-dir=$GCLIENT_CACHE} \
            --custom-var=checkout_x86=False \
            --custom-var=checkout_x64=False \
            --custom-var=checkout_configuration=small \
            --name=src --unmanaged ${NEVA_GIT_REPO_PREFIX}/chromium
    gclient sync --with_tags --with_branch_heads
}

do_configure() {
    export PATH="${HOSTTOOLS_DIR}:${DEPOT_TOOLS_DIR}:$PATH"

    OUT_DIR_IS_EMPTY=$([ -z "$(ls -A "${OUT_DIR}")" ] && echo "true" || echo "false")
    if [ "${OUT_DIR_IS_EMPTY}" = "false" ]; then
        configure_gn_clean "${OUT_DIR}"
    fi

    echo GN_ARGS is "${GN_ARGS}"
    echo BUILD_TARGETS are ${TARGET} ${UNITTESTS_TARGETS}

    if [ "${USE_SISO}" = "true" ]; then
        # Make a link to Chromium's Siso config inside OUT_DIR to make Siso treat OUT_DIR
        # as part of exec_root. By default, Siso determines exec_root by locating
        # build/config/siso, which normally exists under chromium/src. Without this link
        # OUT_DIR outside chromium/src is rejected as "out of exec root".
        mkdir -p ${OUT_DIR}/build
        ln -sfn ${S}/src/build/config ${OUT_DIR}/build/config
    else
        rm -rf ${OUT_DIR}/build/
    fi

    cd ${S}/src
    gn gen ${OUT_DIR} --args="${GN_ARGS}"
}

# TODO: commit a patch to RP meta to use DEPOT_TOOLS_DIR ?= "${S}/depot_tools"
# TODO: commit a patch to RP meta to use ${PARALLEL_MAKE}
do_compile() {
    if [ ! -f ${OUT_DIR}/build.ninja ]; then
         do_configure
    fi

    PATH="${DEPOT_TOOLS_DIR}:$PATH"
    cd ${S}/src
    for target in ${TARGET} ${UNITTESTS_TARGETS}
    do
        set_build_tool_and_parallel_make
        ${BUILD_TOOL} ${ADJUSTED_PARALLEL_MAKE} -C ${OUT_DIR} ${target}
    done

    exit 0 # ugly hack! to prevent any :append from being executed
}

D = "${OUT_DIR}/image"

install_ls2_roles_acg() {
  :
}

install_ls2_roles() {
  :
}

do_install() {
    PATH="${DEPOT_TOOLS_DIR}:$PATH"
    set_build_tool_and_parallel_make

    cd ${S}/src
    ${BUILD_TOOL} ${ADJUSTED_PARALLEL_MAKE} -C ${OUT_DIR} webos/install/weboswebruntime

    if ${DEPLOY_BROWSER}; then
        ${BUILD_TOOL} ${ADJUSTED_PARALLEL_MAKE} -C ${OUT_DIR} webos/install/default_browser
    fi

    if ${DEPLOY_CHROMEDRIVER}; then
        ${BUILD_TOOL} ${ADJUSTED_PARALLEL_MAKE} -C ${OUT_DIR} webos/install/chromedriver
    fi

    if ${DEPLOY_WAM_DEMO}; then
        ${BUILD_TOOL} ${ADJUSTED_PARALLEL_MAKE} -C ${OUT_DIR} webos/install/wam_demo

        W_DIR=${D}${WAM_DEMO_APPLICATION_DIR}
        chmod -v 755 ${W_DIR}/run_wam_demo
    fi

    if ${DEPLOY_BROWSERSHELL}; then
        ${BUILD_TOOL} ${ADJUSTED_PARALLEL_MAKE} -C ${OUT_DIR} webos/install/browser_shell
        BS_DIR=${D}${BROWSERSHELL_APPLICATION_DIR}
        chmod -v 755 ${BS_DIR}/browser_shell
        chmod -v 755 ${BS_DIR}/run_browser_shell
    fi

    if ${DEPLOY_UNITTESTS}; then
        ${BUILD_TOOL} ${ADJUSTED_PARALLEL_MAKE} -C ${OUT_DIR} webos/install/unit_tests
    fi

    if ${DEPLOY_BROWSER}; then
        D_DIR=${D}${BROWSER_APPLICATION_DIR}
        chmod -v 755 ${D_DIR}/chrome
        chmod -v 755 ${D_DIR}/chrome_crashpad_handler
        chmod -v 755 ${D_DIR}/kill_webbrowser
        chmod -v 755 ${D_DIR}/run_webbrowser
    fi
}

INSANE_SKIP:${PN}-dbg += "libdir"

RDEPENDS:${PN}:remove = "${@oe.utils.conditional('DEPLOY_BROWSERSHELL', 'true', \
                         '${APP_SHELL_RUNTIME}', '', d)}"
RDEPENDS:${PN}:append = "${@oe.utils.conditional('DEPLOY_BROWSERSHELL', 'true', \
                         ' ${BROWSERSHELL_APPLICATION}', '', d)}"

RDEPENDS:${PN}:append = "${@oe.utils.conditional('DEPLOY_CHROMEDRIVER', 'true', \
                         ' ${CHROMEDRIVER_RUNTIME}', '', d)}"

RDEPENDS:${PN}:append = "${@oe.utils.conditional('DEPLOY_WAM_DEMO', 'true', \
                         ' ${WAM_DEMO_APPLICATION}', '', d)}"

PACKAGES += "${UNITTESTS_PACKAGE}"

FILES:${PN}:remove = "${webos_sysbus_manifestsdir}"
FILES:${PN}:remove = "${webos_sysbus_permissionsdir}"
FILES:${PN}:remove = "${webos_sysbus_rolesdir}"

FILES:${BROWSER_APPLICATION} += "${BROWSER_APPLICATION_DIR}/chrome_crashpad_handler \
                                 ${webos_sysbus_manifestsdir}/${BROWSER_APPLICATION}.manifest.json \
                                 ${webos_sysbus_rolesdir}/${BROWSER_APPLICATION}.role.json \
                                "

FILES:${WAM_DEMO_APPLICATION} += "${webos_sysbus_manifestsdir}/${WAM_DEMO_APPLICATION}.manifest.json \
                                  ${webos_sysbus_manifestsdir}/${WAM_DEMO_MANUAL_WEB_APPLICATION}.manifest.json \
                                  ${webos_sysbus_containersdir}/${WAM_DEMO_APPLICATION}.container.json \
                                  ${webos_sysbus_permissionsdir}/${WAM_DEMO_APPLICATION}.perm.json \
                                  ${webos_sysbus_permissionsdir}/${WAM_DEMO_MANUAL_WEB_APPLICATION}.perm.json \
                                  ${webos_sysbus_rolesdir}/${WAM_DEMO_APPLICATION}.role.json \
                                  ${webos_sysbus_rolesdir}/${WAM_DEMO_MANUAL_WEB_APPLICATION}.role.json \
                                 "

FILES:${UNITTESTS_PACKAGE} += "/usr/bin/neva_unittests"
FILES:${BROWSERSHELL_APPLICATION} += "${BROWSERSHELL_APPLICATION_DIR} \
                                      ${webos_sysbus_permissionsdir}/${BROWSERSHELL_APPLICATION}.perm.json \
                                      ${webos_sysbus_rolesdir}/${BROWSERSHELL_APPLICATION}.role.json \
                                      ${webos_sysbus_manifestsdir}/${BROWSERSHELL_APPLICATION}.manifest.json \
                                      ${webos_sysbus_containersdir}/${BROWSERSHELL_APPLICATION}.container.json \
                                    "

# Allow installing libminigbm.so to unittests directory instead of global location
INSANE_SKIP:${MLPREFIX}${UNITTESTS_PACKAGE} += "libdir textrel"

# Fix for QA error: "No GNU_HASH in the elf binary: '<file>' [ldflags]"
# This indicates that binaries produced when building the recipe have not been
# linked with the LDFLAGS options provided by the build system.
TARGET_CC_ARCH += "${LDFLAGS}"
