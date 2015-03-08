how to build

make KCONFIG_CONFIG=../linux/zbox-oi520.config menuconfig

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
