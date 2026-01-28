# Copyright 2025 LG Electronics, Inc.

install_units:append() {
    script=${D}${systemd_system_unitdir}/scripts/webapp-mgr.sh
    sed -i '/--no-sandbox/d' $script
    sed -i 's/--webos-js-flags/--js-flags/' $script
    sed -i '/export ENABLE_FEATURES=/s/$/,NetworkServiceInProcess2/' $script
    sed -i '/--disable-gpu-rasterization/d' $script
    sed -i '/--disable-gpu-vsync/d' $script
    sed -i '/WAM_COMMON_SWITCHES=\"\ \\/a\    --disable-support-oop-rasterization \\' $script

    sed -i 's/# setup 4 Mb limitation mse audio buffer size/# setup 2 Mb limitation mse audio buffer size/' $script
    sed -i 's/export MSE_AUDIO_BUFFER_SIZE_LIMIT.*/export MSE_AUDIO_BUFFER_SIZE_LIMIT=2/' $script
    sed -i 's/# setup 50 Mb limitation mse video buffer size/# setup 15 Mb limitation mse video buffer size/' $script
    sed -i 's/export MSE_VIDEO_BUFFER_SIZE_LIMIT.*/export MSE_VIDEO_BUFFER_SIZE_LIMIT=15/' $script
}
