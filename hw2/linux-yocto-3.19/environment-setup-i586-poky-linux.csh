setenv SDKTARGETSYSROOT /scratch/opt/poky/1.8/sysroots/i586-poky-linux
setenv PATH /scratch/opt/poky/1.8/sysroots/i686-pokysdk-linux/usr/bin:/scratch/opt/poky/1.8/sysroots/i686-pokysdk-linux/usr/bin/i586-poky-linux:$PATH
#setenv CCACHE_PATH $sdkpathnative$bindir:$sdkpathnative$bindir/i586-poky-linux:$CCACHE_PATH
setenv PKG_CONFIG_SYSROOT_DIR $SDKTARGETSYSROOT
setenv PKG_CONFIG_PATH $SDKTARGETSYSROOT/usr/lib/pkgconfig
setenv CONFIG_SITE /scratch/opt/poky/1.8/site-config-i586-poky-linux
setenv OECORE_NATIVE_SYSROOT "/scratch/opt/poky/1.8/sysroots/i686-pokysdk-linux"
setenv OECORE_TARGET_SYSROOT "$SDKTARGETSYSROOT"
setenv OECORE_ACLOCAL_OPTS "-I /scratch/opt/poky/1.8/sysroots/i686-pokysdk-linux/usr/share/aclocal"
setenv PYTHONHOME /scratch/opt/poky/1.8/sysroots/i686-pokysdk-linux/usr
setenv CC "i586-poky-linux-gcc  -m32 -march=i586 --sysroot=$SDKTARGETSYSROOT"
setenv CXX "i586-poky-linux-g++  -m32 -march=i586 --sysroot=$SDKTARGETSYSROOT"
setenv CPP "i586-poky-linux-gcc -E  -m32 -march=i586 --sysroot=$SDKTARGETSYSROOT"
setenv AS "i586-poky-linux-as  "
setenv LD "i586-poky-linux-ld   --sysroot=$SDKTARGETSYSROOT"
setenv GDB i586-poky-linux-gdb
setenv STRIP i586-poky-linux-strip
setenv RANLIB i586-poky-linux-ranlib
setenv OBJCOPY i586-poky-linux-objcopy
setenv OBJDUMP i586-poky-linux-objdump
setenv AR i586-poky-linux-ar
setenv NM i586-poky-linux-nm
setenv M4 m4
setenv TARGET_PREFIX i586-poky-linux-
setenv CONFIGURE_FLAGS "--target=i586-poky-linux --host=i586-poky-linux --build=i686-linux --with-libtool-sysroot=$SDKTARGETSYSROOT"
setenv CFLAGS " -O2 -pipe -g -feliminate-unused-debug-types"
setenv CXXFLAGS " -O2 -pipe -g -feliminate-unused-debug-types"
setenv LDFLAGS "-Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed"
setenv CPPFLAGS ""
setenv KCFLAGS "--sysroot=$SDKTARGETSYSROOT"
setenv OECORE_DISTRO_VERSION "1.8"
setenv OECORE_SDK_VERSION "1.8"
setenv ARCH x86
setenv CROSS_COMPILE i586-poky-linux-
echo "Environment variables updated"
