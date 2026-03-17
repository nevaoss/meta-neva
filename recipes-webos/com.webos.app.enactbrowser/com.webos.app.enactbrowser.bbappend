DEPENDS += "nodejs-native"

# LIC_FILES_CHKSUM override required to remove the check for oss-pkg-info.yaml file
LIC_FILES_CHKSUM = "file://LICENSE;md5=6e00eb832d81f89a0f47fac10db717c7"

SUPPORT_BROWSERSHELL = "true"
WEBOS_ENACTJS_SHRINKWRAP_OVERRIDE = "false"
LOCAL_ENACT = "${S}/samples/enact-based/node_modules/.bin/enact"

WEBOS_ENACTJS_PACK_UIOVERLAY = "\
    && cd ../../uioverlay/ \
    && ${LOCAL_ENACT} pack ${WEBOS_ENACTJS_PACK_OPTS} -o ../samples/enact-based/dist/uioverlay \
    && cd ../samples/enact-based \
"

WEBOS_ENACTJS_PREPARE_RESOURCES = "\
    && cp -f label.js dist/ \
    && cp -f background.js dist/ \
    && cp -f defaults.js dist/ \
    && sed -i -E \'s/(useBuiltInErrorPages:) *false/\1 true/g\' dist/defaults.js \
"

WEBOS_ENACTJS_PACK_FOR_BROWSERSHELL = "${@oe.utils.conditional('SUPPORT_BROWSERSHELL', \
   'true', '${WEBOS_ENACTJS_PACK_UIOVERLAY}', '', d)}"

# It is a copy of the code from meta-lg-webos/meta-webos/recipes-webos/com.webos.app.enactbrowser/com.webos.app.enactbrowser.bb
# There is only one difference: "ENACT_DEV_LEGACY" replaced with "ENACT_DEV"
WEBOS_ENACTJS_PACK_OVERRIDE = "\
    ${LOCAL_ENACT} pack ${WEBOS_ENACTJS_PACK_OPTS} && \
    ${WEBOS_NODE_BIN} resbundler.js dist && \
    rm -fr ./dist/resources && \
    rm -fr ./dist/node_modules/@enact/moonstone/resources && \
    cp -f webos-locale.js dist/webos-locale.js && \
    ln -sfn /usr/share/javascript/ilib/localedata/ ./dist/ilibdata && \
    cp -f manifest.json dist/ && \
    ${WEBOS_NODE_BIN} extract-inline.js ./dist \
"

WEBOS_ENACTJS_PACK_OVERRIDE += "\
    && cp -rf resources/ dist/resources \
    && ./scripts/install-manifest.js --from=manifest.json --to=dist/manifest.json --version_suffix=`git rev-parse HEAD` \
    ${WEBOS_ENACTJS_PACK_FOR_BROWSERSHELL} \
"

python neva_magic_hack() {
  "Normalize S to have a path separator at the end to match parent recipe expectations"
  import os

  if d.getVar('EXTERNALSRC', True):
    d.setVar('S', d.getVar('EXTERNALSRC', True).rstrip(os.path.sep) + os.path.sep)
}

do_install[prefuncs] += "neva_magic_hack"

do_compile:prepend() {
    cd ${S}
}

do_install:prepend() {
    cd ${S}
}

do_compile:remove() {
    ./scripts/disable-ilib.sh
}

do_npm_install:remove() {
    git apply --no-index --verbose ../samples/enact-based/enact_agate_internal_l.patch
    git apply --no-index --verbose enact_agate_internal_l.patch
    ${WEBOS_NODE_BIN} ./scripts/cli.js transpile
}

do_npm_install:append() {
    cd ${S}
    ${WEBOS_NPM_BIN} run "transpile"
}
