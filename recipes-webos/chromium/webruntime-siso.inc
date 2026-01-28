# neva specific siso settings
# Trick to prevent neva-do_configure from copying siso config to out dir
# We will remove 'use_siso=false' later from GN_ARGS
python () {
    siso_reapi_address = (d.getVar('SISO_REAPI_ADDRESS', True) or '').strip()
    use_remote_cache_only = d.getVar('SISO_REMOTE_CACHE_ONLY', True) == "1"

    if siso_reapi_address or use_remote_cache_only:
        d.setVar('USE_SISO', 'false')
        d.setVar('GN_ARGS:remove', ' use_siso=false')

        siso_configuration(d)

        # Trick to use autoninja with siso remote build in neva recipe
        d.setVar('ADJUSTED_PARALLEL_MAKE', "")
        d.setVar('OUT_DIR', d.getVar('OUTPUT_DIR'))
}

# end of neva specific siso settings

# Use remote build cache without remote execution
SISO_REMOTE_CACHE_ONLY ?= "0"
SISO_REAPI_ADDRESS ?= ""
SISO_LOCAL_JOBS ?= ""
SISO_REMOTE_JOBS ?= "100"
SISO_PATH ?= ""
SISO_BUILD ?= "ninja -project='' -job_id=$(id -un)@$(hostname) -fs_min_flush_timeout=5m -reapi_keep_exec_stream=true -reapi_insecure -reapi_instance=ubuntu22-04"
SISO_LOCAL_CACHE_ENABLED ?= "0"

patch_interpreter() {
    bbnote "Patching interpreter for: $1"
    local abs_path=$(which $1 || echo "")
    [ -n "$abs_path" ] || return 0
    bbnote " - abs_path  : $abs_path"

    local real_path=$(readlink -f $abs_path || echo "")
    [ -n "$real_path" ] || return 0
    bbnote " - real_path : $real_path"

    local interp=$(patchelf --print-interpreter "$real_path" || echo "")
    [ -n "$interp" ] || return 0
    bbnote " - interp    : $interp"
    [ "$interp" != "/lib64/ld-linux-x86-64.so.2" ] || return 0

    cp -a "$real_path" "$real_path.new" && \
    mv -f "$real_path.new" "$real_path" && \
    patchelf --set-interpreter /lib64/ld-linux-x86-64.so.2 "$real_path"

    bbnote " - Interpreter is patched for: $1 ($real_path)"
}

patch_compiler() {
    for bin in gcc g++ clang clang++; do
        patch_interpreter "${HOST_PREFIX}${bin}"
        patch_interpreter "${BUILD_PREFIX}${bin}"
    done

    for tool in cc1 cc1plus as; do
        for prefix in "${BUILD_PREFIX}" "${HOST_PREFIX}"; do
            COMPILER=$prefix"gcc"
            ABS_PATH="$(which $COMPILER || echo '')"
            [ -n "$ABS_PATH" ] || continue

            DRIVER="$($ABS_PATH -print-prog-name=$tool || echo '')"
            [ -n "$DRIVER" ] && [ -f "$DRIVER" ] && patch_interpreter "$DRIVER"
        done
    done
}

def siso_configuration(d):
    # siso configuration
    siso_reapi_address = (d.getVar('SISO_REAPI_ADDRESS', True) or '').strip()
    use_remote_cache_only = d.getVar('SISO_REMOTE_CACHE_ONLY', True) == "1"

    if siso_reapi_address or use_remote_cache_only:
        siso_build = d.getVar('SISO_BUILD', True)

        if use_remote_cache_only:
            siso_build += " -re_exec_enable=false -re_cache_enable_write"
            siso_reapi_address = siso_reapi_address or "bb-scheduler.lge.com:30080"

        siso_build += f" -reapi_address={siso_reapi_address}"

        siso_build += f" -remote_jobs={d.getVar('SISO_REMOTE_JOBS', True)}"
        if d.getVar('SISO_LOCAL_JOBS', True):
            siso_build += f" -local_jobs={d.getVar('SISO_LOCAL_JOBS', True)}"

        if d.getVar('SISO_LOCAL_CACHE_ENABLED', True) == "1":
            siso_build += " -local_cache_enable=true"
        else:
            siso_build += " -local_cache_enable=false"

        import os

        src_dir = os.path.normpath(d.getVar('S', True))
        if os.path.basename(src_dir) != 'src':
            src_dir = os.path.join(src_dir, 'src')

        siso_path = (d.getVar('SISO_PATH', True) or '').strip()
        if not siso_path:
            siso_path = f"{src_dir}/build/neva/siso"

        siso_build = f"{siso_path} -log_dir={src_dir}/out/Release {siso_build}"

        d.setVar('RECIPE_SYSROOT', f"{src_dir}/out/recipe-sysroot")
        d.setVar('RECIPE_SYSROOT_NATIVE', f"{src_dir}/out/recipe-sysroot-native")

        # when multilib is used lib32- prefix is added automatically
        # force to use the directory without lib32- prefix
        d.setVar('STAGING_DIR_TARGET', f"{src_dir}/out/recipe-sysroot")
        d.setVar('STAGING_DIR_HOST', f"{src_dir}/out/recipe-sysroot")

        d.setVar('BUILD_TOOL', f"{siso_path} -log_dir={src_dir}/out/Release ninja -o -project=lge")
        d.setVar('BUILD_TOOL:task-compile', siso_build)
        d.setVar('OUTPUT_DIR', f"{src_dir}/out/Release")

        # remove ccache/icecc for siso build
        d.setVar('PARALLEL_MAKE:forcevariable', '')
        d.setVar('CCACHE:forcevariable', '')
        d.setVar('CCACHE_DISABLE:forcevariable', '1')
        d.setVar('ICECC_DISABLED:forcevariable', '1')

        # workaround for uninative loader issue
        d.appendVarFlag('do_prepare_recipe_sysroot', 'postfuncs', ' patch_compiler')
        d.appendVar('DEPENDS', ' patchelf-native ')

        d.setVar('GN_ARGS:append', ' use_remoteexec=true use_siso=true')

        # ignore cc_wrapper if already set
        if 'cc_wrapper=' in d.getVar('GN_ARGS', True):
            d.setVar('GN_ARGS:append', ' cc_wrapper=\\\"\\\"')

        # siso requires network access during compile
        for task in ['do_compile']:
            d.setVarFlag(task, 'network', '1')

