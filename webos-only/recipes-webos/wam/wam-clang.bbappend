# Copyright 2025 LG Electronics, Inc.

require wam-ose-neva.inc

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# Add required patches for WAM here.
SRC_URI += " \
    file://0001-Add-DidWebContentsChanged-event-to-support-PageDisca.patch \
    file://0001-build-wam-Replace-gfx-Rect-with-webos-WebAppWindowBa.patch \
    file://0001-Fix-error-templates-must-have-cpp-linkage.patch \
    file://0001-op-code-health-Remove-std-codecvt_utf8_utf16.patch \
"
