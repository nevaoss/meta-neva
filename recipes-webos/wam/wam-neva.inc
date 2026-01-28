# Copyright 2025 LG Electronics, Inc.

install_units:append() {
    if [ "${PREFERRED_PROVIDER_virtual/webruntime}" = "webruntime${WEBRUNTIME_TOOLCHAIN}" ]; then
        script=${D}${systemd_system_unitdir}/scripts/webapp-mgr.sh
        sed -i -e "s/NETWORK_STABLE_TIMEOUT/NETWORK_QUIET_TIMEOUT/gI" -e "s/network-stable-timeout/network-quiet-timeout/gI" $script
        sed -i -e "s/ignore-gpu-blacklist/ignore-gpu-blocklist/gI" $script
        sed -i '/export WAM_COMMON_SWITCHES=\" \\/a\    --enable-neva-user-agent-client-hints \\' $script
        sed -i '/export WAM_COMMON_SWITCHES=\" \\/a\    --enable-notification-for-unsupported-features \\' $script

        sed -i '/export DISABLE_BLINK_FEATURES/a\\nexport DISABLE_FEATURES=MemoryPurgeInBackground' $script
        sed -i '/export WAM_COMMON_SWITCHES=\" \\/a\    --disable-features=$DISABLE_FEATURES \\' $script

        sed -i '/export WAM_MEM_FLAGS=\" \\/a\    --local-storage-limit-per-second-level-domain=10 \\' $script

        sed -i 's/mse-audio-buffer-size-limit/mse-audio-buffer-size-limit-mb/g' $script
        sed -i 's/mse-video-buffer-size-limit/mse-video-buffer-size-limit-mb/g' $script
        sed -i 's/export MSE_AUDIO_BUFFER_SIZE_LIMIT.*/export MSE_AUDIO_BUFFER_SIZE_LIMIT=4/' $script
        sed -i 's/export MSE_VIDEO_BUFFER_SIZE_LIMIT.*/export MSE_VIDEO_BUFFER_SIZE_LIMIT=50/' $script

        sed -i '/--shared-mem-minimal-limit-mb=8/d' $script
        sed -i '/--shared-mem-pressure-divider=4/d' $script
        sed -i '/--shared-mem-system-mem-reduction-factor=10/d' $script
        sed -i '/SKIA_FONT_CACHE_SIZE/d' $script

        sed -i -z 's/    --enable-neva-media-service \\\n//g' $script

        if grep -q 'ozone-platform=' $script ; then
            sed -i 's/--ozone-platform=wayland_external/--ozone-platform=wayland/g' $script
        else
            sed -i '/export WAM_COMMON_SWITCHES/a\    --ozone-platform=wayland \\' $script
        fi
    fi
}
