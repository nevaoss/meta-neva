# How to build Neva webruntime for webOS/OSE

## Summary
meta-neva is wrapper layer for [meta-webosose](https://github.com/webosose/meta-webosose) used
to build webOS OSE (Open Source Edition) with NEVA specifics.

## Preparation

### System configuration
Build is tested and works on Ubuntu 22.04 only
```bash
$ lsb_release -a
No LSB modules are available.
Distributor ID: Ubuntu
Description:    Ubuntu 22.04.4 LTS
Release:        22.04
Codename:       jammy
```

### Build toolstack & sources
To get the build toolstack and Neva source code,
you need to do the following:
- Clone https://github.com/webosose/build-webos for webOS/OSE build
toolstack.
- Clone https://chromium.googlesource.com/chromium/tools/depot_tools
for Google depot_tools.
- Clone and check out neva-main branch of https://github.com/nevaoss/chromium.git
directory for Neva itself.

Note: We've currently pinned out our build-webos to builds/master/843,
please check it out as follows:
```bash
WORKSPACE$ $ git clone https://github.com/webosose/build-webos -b builds/master/843
```

```bash
WORKSPACE$ git clone https://chromium.googlesource.com/chromium/tools/depot_tools
WORKSPACE$ cd depot_tools
WORKSPACE/depot_tools$ ./gclient        # to avoid certificate error
WORKSPACE/depot_tools$ cd ..
```

```bash
WORKSPACE$ git clone -b neva-main https://github.com/nevaoss/chromium.git chromium/src
```

You need to install Chromium build dependencies with the help
of .../build/install-build-deps.sh script held in build directory
under Chromium sources:
```bash
WORKSPACE$ sudo ./chromium/src/build/install-build-deps.sh
```

You also need to install all webOS/OSE build toolstack prerequisites:
```bash
WORKSPACE$ sudo ./build-webos/scripts/prerequisites.sh
```

### Bitbake metas and recipes via MCF
You need to change weboslayers.py file manually in this way:
```bash
WORKSPACE$ cd build-webos
WORKSPACE/build-webos$ git diff weboslayers.py
diff --git a/weboslayers.py b/weboslayers.py
index 3c4967d..a4a4938 100644
--- a/weboslayers.py
+++ b/weboslayers.py
@@ -83,4 +83,5 @@ webos_layers = [
 ('meta-webos-virtualization', 53, 'https://github.com/webosose/meta-webosose.git',          '', ''),
 
 ('meta-security',             76, 'https://git.yoctoproject.org/git/meta-security',         'branch=scarthgap,commit=bc865c5276c', ''),
+('meta-neva',                 99, 'https://github.com/nevaoss/meta-neva.git',               'branch=webosose/843,commit=webosose/843', ''),
 ]
```

Next, you need to check out all metalayers with the help of mcf tool:
```bash
WORKSPACE/build-webos$ ./mcf -p 6 -b 6 raspberrypi4-64
```

Note: You highly likely want to decrease a number of parallel bitbake
threads -b 6 and a number of parallel compiler processes -p 6
to be less than a number of your processor cores/hyperthreads:
```bash
WORKSPACE/build-webos$ nproc
8
WORKSPACE/build-webos$ ./mcf -p 4 -b 4 raspberrypi4-64
```

### Bitbake configuration

Bitbake can use local directories as sources for recipes
through the externalsrc mechanism.
This allows Neva Chromium to be built directly from your workspace
without repackaging or fetching sources, which is especially useful
during development and speeds up iterative builds.

To enable this behavior, the webos-local.conf file must include
externalsrc and specify paths to your local Chromium tree and depot_tools.
Optional build accelerators such as ccache or ccache+icecc can also
be configured the same way if needed.

Instead of preparing the configuration manually, you can generate
a complete and ready‑to‑use file with the provided helper script.
It automatically substitutes @@WORKSPACE@@ and fills in all required
variables:
```bash
WORKSPACE/build-webos$ ./meta-neva/scripts/make-webos-local-conf
```

---

## Actual building

### Building Neva webruntime

To build Neva webruntime, you should build webruntime-clang recipe
by make just-webruntime-clang instead of building a whole webOS image:
```bash
WORKSPACE/build-webos$ make just-webruntime-clang
```

or you can alternatively build it manually by old plain bitbake call:
```bash
WORKSPACE/build-webos$ . oe-init-build-env
WORKSPACE/build-webos$ bitbake webruntime-clang
```

### Build artifacts

After build you will find some *.ipk under BUILD/deploy/ipk/raspberrypi4_64/ directory:
```bash
...
WORKSPACE/build-webos$ find BUILD/deploy/ipk/raspberrypi4_64/ | grep -E 'webruntime'
BUILD/deploy/ipk/raspberrypi4_64/webruntime-clang_143.0.7499.0-r5webosrpi5neva1_raspberrypi4_64.ipk
BUILD/deploy/ipk/raspberrypi4_64/webruntime-clang-dev_143.0.7499.0-r5webosrpi5neva1_raspberrypi4_64.ipk
BUILD/deploy/ipk/raspberrypi4_64/webruntime-clang-dbg_143.0.7499.0-r5webosrpi5neva1_raspberrypi4_64.ipk
...
```

Now you're ready to build a whole Neva-based webOS/OSE root image:
```bash
WORKSPACE/build-webos$ ionice -c idle chrt -i 0 make just-webos-image
```

and then find it here:
```bash
WORKSPACE/build-webos$ find BUILD/deploy/images/raspberrypi4-64/*webos-image-raspberrypi4-64*
...
BUILD/deploy/images/raspberrypi4-64/webos-image-raspberrypi4-64.manifest
BUILD/deploy/images/raspberrypi4-64/webos-image-raspberrypi4-64-master-20251111081814.manifest
BUILD/deploy/images/raspberrypi4-64/webos-image-raspberrypi4-64-master-20251111081814.tar.bz2
BUILD/deploy/images/raspberrypi4-64/webos-image-raspberrypi4-64-master-20251111081814.testdata.json
BUILD/deploy/images/raspberrypi4-64/webos-image-raspberrypi4-64-master-20251111081814.wic.bmap
BUILD/deploy/images/raspberrypi4-64/webos-image-raspberrypi4-64-master-20251111081814.wic.bz2
BUILD/deploy/images/raspberrypi4-64/webos-image-raspberrypi4-64.tar.bz2
BUILD/deploy/images/raspberrypi4-64/webos-image-raspberrypi4-64.testdata.json
BUILD/deploy/images/raspberrypi4-64/webos-image-raspberrypi4-64.wic.bmap
BUILD/deploy/images/raspberrypi4-64/webos-image-raspberrypi4-64.wic.bz2
...
```

and now you're finally ready to boot it to your RPi4!

---

## Re-building

### Rebuilding webruntime after updating GN/gni files (including updating Chromium version)

If you are updating any GN/gni files (including updating Chromium
version) it is recommended to re-build webruntime via bitbake,
because Make might not see changes, since we are using externalsrc way.

After updating GN/gni files the webruntime should be re-built
beginning from the step do_configure:
```bash
WORKSPACE/build-webos$ . oe-init-build-env
WORKSPACE/build-webos$ bitbake -f -C configure webruntime-clang
```

After updating Chromium version the webruntime should be re-built
beginning from the step do_gclient_sync:
```bash
WORKSPACE/build-webos$ . oe-init-build-env
WORKSPACE/build-webos$ bitbake -f -C gclient_sync webruntime-clang
```

### After switching to new base webOS build

To save some space on the disk, after first build on new base webOS
build, you might remove BUILD directory and build the webOS
from the scratch.

In this case previously saved built states for recipes will be taken
from local sstate-cache and the BUILD directory will be quite smaller.

---

## Flashing webOS/OSE image

You can find out how to install the webOS/OSE build
in the [official documentation](https://www.webosose.org/docs/guides/setup/flashing-webos-ose/).

**Happy testing!**
