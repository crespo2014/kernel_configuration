Tips:

diff -rup -x '*.o' -x '*.h' -x 'bzImage' --exclude=.cproject --exclude=build --exclude=drivers/gpu/drm/nvidia-ion linux-4.0 linux-4.0.orig >> kernel_configuration/linux-4.0-patch.diff

Keep module name at the end

sed 's/.*\/\([[:alnum:]_-]*\.ko\)$/\1/g'

reduce module name for dependencies.

sed 's/.*\/\([[:alnum:]_-]*\.ko\)\(:.*\)/\1\2/g'

depmod -v 4.0.0-311c-module > 311c.modules.dep

Removing files from history
git filter-branch --index-filter "git rm --cached -f --ignore-unmatch linux-4.0-patch.diff"  -- all

find -name '*.c' -exec grep _initcall\( \{} \;
sort <file> -o <out-file>


git clone git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
cd linux
git checkout v2.6.36.2
git checkout v3.5.2

To update your repository to include all of the latest tags and commits:

git fetch


how to build

make KCONFIG_CONFIG=../linux/zbox-oi520.config menuconfig

make O=build/oi520 KCONFIG_CONFIG=`pwd`/../kernel_configuration/zbox-oi520.config -j4 all
make O=build/311c KCONFIG_CONFIG=`pwd`/../kernel_configuration/311c.config -j4 all

KCONFIG_CONFIG
--------------------------------------------------
This environment variable can be used to specify a default kernel config
file name to override the default name of ".config".

KCONFIG_OVERWRITECONFIG
--------------------------------------------------
If you set KCONFIG_OVERWRITECONFIG in the environment, Kconfig will not
break symlinks when .config is a symlink to somewhere else.

CONFIG_
--------------------------------------------------
If you set CONFIG_ in the environment, Kconfig will prefix all symbols
with its value when saving the configuration, instead of using the default,
"CONFIG_".
