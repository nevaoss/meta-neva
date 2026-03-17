# Copyright 2021 LG Electronics, Inc.

LIC_FILES_CHKSUM = ""
ERROR_QA:remove = "license-checksum"
WARN_QA:append = " license-checksum"

WEBOS_VERSION = "147.0.7727.0_00000000000000000000000000000000"
ERROR_QA:remove = "webos-enh-sub-autorev-error"
WARN_QA:append = " webos-enh-sub-autorev-error"
EXTENDPRAUTO:append = "neva1"

SRCREV_chromium = "${AUTOREV}"
SRCREV_depot_tools = "${AUTOREV}"

# This task doesn`t needed after the replacement unified  manifest to target-specified
do_configure_manifest[noexec] = "1"

NEVA_GIT_REPO_PREFIX = "https://repo.lge.com/neva"
CHROMIUM_GIT_REPO_PREFIX = "https://chromium.googlesource.com/chromium"
WAM_DEMO_MANUAL_WEB_APPLICATION = "com.webos.app.neva.wam.demo.manual"

SRC_URI = " \
    ${NEVA_GIT_REPO_PREFIX}/chromium;destsuffix=git/src;branch=neva-master;name=chromium;protocol=ssh \
    ${CHROMIUM_GIT_REPO_PREFIX}/tools/depot_tools.git;branch=main;destsuffix=git/depot_tools;name=depot_tools;protocol=https \
"
DEPOT_TOOLS_DIR ?= "${S}/depot_tools"

# v8 snapshot toolchain settings.
#
# v8 snapshot toolchain settings should be set for both webruntime and
# mksnapshot targets, hence it is placed in the common place here.
#
# We don't have the cros:v8_snapshot_clang_${TARGET_CPU} which RP/Blink has.
#
# See also webos-only/recipes-webos/chromium/webruntime-common-ose.inc for the
# webOS/OSE specifics.
# /-- v8_snapshot_toolchain settings begin
GN_ARGS:remove = "v8_snapshot_toolchain=\"//build/toolchain/cros:v8_snapshot_clang_${TARGET_CPU}\""

V8_SNAPSHOT_TOOLCHAIN:arm = "clang_x86_v8_arm"
GN_ARGS:append:arm = " v8_snapshot_toolchain=\"//build/toolchain/linux:${V8_SNAPSHOT_TOOLCHAIN}\""

V8_SNAPSHOT_TOOLCHAIN:aarch64 = "clang_x64_v8_arm64"
GN_ARGS:append:aarch64 = " v8_snapshot_toolchain=\"//build/toolchain/linux:${V8_SNAPSHOT_TOOLCHAIN}\""
# \-- v8_snapshot_toolchain settings end

GN_ARGS:append = " host_pkg_config=\"pkg-config-native\""
GN_ARGS:append = " use_dummy_lastchange=false"

# Use lld linker for Clang builds instead of gold
GN_ARGS:append = " use_gold=${@oe.utils.conditional('WEBRUNTIME_TOOLCHAIN', '-clang', 'false', 'true', d)}"

# Add usage of Siso instead of Ninja
USE_SISO ?= "true"
GN_ARGS:append = " use_siso=${USE_SISO}"

python neva_magic_hack() {
  """Mangles S variable to point exactly to the *parent* of chromium src dir (RP git repo root) as RP recipe expects.
  The point is that externalsrc.bblass sets S to be equal to EXTERNALSRC and it expects (sic!) it to be root
  of git repo otherwise it starts calculating checksum of all files in the EXTERNALSRC/S dir.
  Being squeezed between those different expectations, we point EXTERNALSRC to *the* src dir itself (our git repo root)
  and then mangle this back to 'normal'
  """
  import os

  if d.getVar('EXTERNALSRC', True):
    d.setVar('S', os.path.dirname(d.getVar('EXTERNALSRC', True).rstrip(os.path.sep)))
}

do_populate_lic[prefuncs] += "neva_magic_hack"
do_gclient_sync[prefuncs] += "neva_magic_hack"
do_configure[prefuncs] += "neva_magic_hack"
do_compile[prefuncs] += "neva_magic_hack"
do_install[prefuncs] += "neva_magic_hack"

do_gclient_sync[network] = "1"
do_configure[network] = "1"
do_compile[network] = "1"

# We can't afford the file checksumming functions from externalsrc.bbclass
# especially after M118 with a pile of git submodules
python() {
    d.setVarFlag('do_compile', 'file-checksums', '')
    d.setVarFlag('do_configure', 'file-checksums', '')
}

# If you want webruntime and mksnapshot-cross recipes to be fully invalidated and rebuilt,
# set NEVA_BUILD_ID to some another new value in your webos-local.conf each time you want
# that to be initiated
NEVA_BUILD_ID ?= "default"

do_configure[vardeps] = "NEVA_BUILD_ID"

configure_gn_clean() {
    OUT_DIR="$1"

    OLD_USE_SISO_ARG=$(grep -s -o 'use_siso *= *[^ ]*' "${OUT_DIR}/args.gn" | head -n1 | cut -d= -f2 | tr -d ' ')
    OLD_USE_SISO_ARG=${OLD_USE_SISO_ARG:-false}

    if [ "${OLD_USE_SISO_ARG}" != "${USE_SISO}" ]; then
        echo "Change detected in GN argument 'use_siso': old='${OLD_USE_SISO_ARG}' new='${USE_SISO}' → running gn clean"
        cd ${S}/src
        gn clean ${OUT_DIR}
    fi
}

set_build_tool_and_parallel_make() {
    # Autoninja or Ninja depending on "use_siso" GN arg
    export BUILD_TOOL=$([ "${USE_SISO}" = "true" ] && echo "autoninja" || echo "ninja")

    # To set the PARALLEL_MAKE value, Siso requires the "-local_jobs" option, while Ninja requires "-j"
    JOBS_FOR_SISO=$(echo "${PARALLEL_MAKE}" | cut -d' ' -f2)
    export ADJUSTED_PARALLEL_MAKE=$([ "${USE_SISO}" = "true" ] && echo "-local_jobs=${JOBS_FOR_SISO}" || echo "${PARALLEL_MAKE}")
}
