# Copyright 2025 LG Electronics, Inc.

require webruntime-common-ose.inc

GN_ARGS:append:hardware = " \
  enable_skia_force_invalidate=true \
"

GN_ARGS:remove:hardware = "enable_webm_video_codecs=false"

GN_ARGS:append:qemux86 = " use_vaapi=false "
GN_ARGS:append:qemux86-64 = " use_vaapi=false "

GN_ARGS:append = " enable_pinch_to_zoom=true "

UNITTESTS_TARGETS:append = "${@oe.utils.conditional('DEPLOY_UNITTESTS', 'true', ' ui/ozone:ozone_unittests', '', d)}"

do_install:append() {
    ${BUILD_TOOL} ${ADJUSTED_PARALLEL_MAKE} -C ${OUT_DIR} webos/install/error_page_permission

    if ${DEPLOY_WAM_DEMO}; then
        sed -i '/CHROME_FLAGS=\"\ \\/a\  --disable-support-oop-rasterization \\' ${W_DIR}/run_wam_demo
        sed -i '/--no-sandbox/d' ${W_DIR}/run_wam_demo
    fi

    if ${DEPLOY_BROWSERSHELL}; then
        sed -i '/CHROME_FLAGS=\"\ \\/a\  --disable-support-oop-rasterization \\' ${BS_DIR}/run_browser_shell
        sed -i '/--no-sandbox/d' ${BS_DIR}/run_browser_shell
        sed -i '/--no-zygote/d' ${BS_DIR}/run_browser_shell
    fi
}

ERROR_PAGE_PERMISSION = "com.webos.settingsservice.client"

FILES:${PN} += "${webos_sysbus_manifestsdir}/${ERROR_PAGE_PERMISSION}.manifest.json \
                ${webos_sysbus_permissionsdir}/${ERROR_PAGE_PERMISSION}.perm.json \
                ${webos_sysbus_rolesdir}/${ERROR_PAGE_PERMISSION}.role.json \
               "
# Disable webos-codec
PACKAGECONFIG:remove = "webos-codec"
