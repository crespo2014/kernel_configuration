how to build

make KCONFIG_CONFIG=../linux/zbox-oi520.config menuconfig

make O=build/oi520 KCONFIG_CONFIG=`pwd`/../linux/zbox-oi520.config -j4 all
make O=build/311c KCONFIG_CONFIG=`pwd`/../linux/311c.config -j4 all

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
