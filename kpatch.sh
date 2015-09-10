#!/bin/bash
#patching

patch -r out -f -p0 < ../kernel_configuration/linux.diff 


for i in include/linux/hid.h include/linux/i2c.h include/linux/pci.h include/linux/usb.h 
do
sed -i s/module_driver\(/_async_module_driver\(/g $i
done

for i in arch/x86/kernel/pcspeaker.c 
do
sed -i s/device_initcall\(/async_module_init\(/g $i
done

for i in \
arch/x86/kernel/pcspeaker.c \
crypto/ansi_cprng.c \
crypto/arc4.c \
crypto/842.c \
crypto/asymmetric_keys/pkcs7_key_type.c \
crypto/asymmetric_keys/x509_public_key.c \
crypto/async_tx/async_tx.c \
crypto/authenc.c \
crypto/authencesn.c \
crypto/blowfish_generic.c \
crypto/camellia_generic.c \
crypto/cast5_generic.c \
crypto/cast6_generic.c \
crypto/cbc.c \
crypto/ccm.c \
crypto/chainiv.c \
crypto/cmac.c \
crypto/crc32.c \
crypto/crc32c_generic.c \
crypto/crct10dif_generic.c \
crypto/crypto_null.c \
crypto/crypto_user.c \
crypto/ctr.c \
crypto/cts.c \
crypto/deflate.c \
crypto/des_generic.c \
crypto/drbg.c \
crypto/ecb.c \
crypto/eseqiv.c \
crypto/fcrypt.c \
crypto/gcm.c \
crypto/ghash-generic.c \
crypto/hmac.c \
crypto/khazad.c \
crypto/krng.c \
crypto/lrw.c \
crypto/lz4.c \
crypto/lz4hc.c \
crypto/lzo.c \
crypto/md4.c \
crypto/md5.c \
crypto/michael_mic.c \
crypto/pcbc.c \
crypto/pcrypt.c \
crypto/rmd128.c \
crypto/rmd256.c \
crypto/rmd320.c \
crypto/salsa20_generic.c \
crypto/seed.c \
crypto/seqiv.c \
crypto/serpent_generic.c \
crypto/sha1_generic.c \
crypto/sha256_generic.c \
crypto/sha512_generic.c \
crypto/tcrypt.c \
crypto/tea.c \
crypto/tgr192.c \
crypto/twofish_generic.c \
crypto/vmac.c  \
crypto/wp512.c  \
crypto/xcbc.c   \
crypto/xts.c   \
crypto/zlib.c  \
drivers/acpi/acpi_ipmi.c  \
drivers/acpi/acpi_pad.c   \
drivers/acpi/processor_driver.c \
drivers/acpi/sbs.c \
drivers/acpi/thermal.c \
drivers/block/brd.c \
drivers/block/loop.c \
drivers/char/agp/backend.c \
drivers/char/agp/nvidia-agp.c \
drivers/char/hpet.c \
drivers/acpi/video.c
do
sed -i 's/module_init(/deferred_module_init(/g' $i
done

for i in \
drivers/acpi/ac.c  \
drivers/acpi/acpi_extlog.c \
drivers/acpi/apei/einj.c \
drivers/acpi/apei/erst-dbg.c  \
drivers/acpi/apei/ghes.c \
drivers/acpi/battery.c \
drivers/acpi/pmic/intel_pmic_crc.c \
drivers/acpi/pmic/intel_pmic_xpower.c \
drivers/ata/acard-ahci.c \
drivers/ata/ahci.c \
drivers/ata/ahci_platform.c \
drivers/char/agp/ali-agp.c \
drivers/char/agp/alpha-agp.c \
drivers/char/agp/amd64-agp.c \
drivers/char/agp/amd-k7-agp.c \
drivers/char/agp/ati-agp.c \
drivers/char/agp/efficeon-agp.c \
drivers/char/agp/hp-agp.c \
drivers/char/agp/i460-agp.c \
drivers/char/agp/intel-agp.c \
drivers/char/agp/parisc-agp.c \
drivers/char/agp/sgi-agp.c \
drivers/char/agp/sis-agp.c \
drivers/char/agp/sworks-agp.c \
drivers/char/agp/uninorth-agp.c \
drivers/char/agp/via-agp.c \
drivers/char/apm-emulation.c \
drivers/char/applicom.c \
drivers/char/bfin-otp.c \
drivers/char/bsr.c \
drivers/char/ds1302.c \
drivers/char/ds1620.c \
drivers/char/dsp56k.c \
drivers/char/dtlk.c \
drivers/char/efirtc.c \
drivers/char/generic_nvram.c \
drivers/char/genrtc.c \
drivers/char/hangcheck-timer.c \
drivers/char/hw_random/core.c \
drivers/char/hw_random/geode-rng.c \
drivers/char/hw_random/amd-rng.c \
drivers/char/hw_random/ixp4xx-rng.c \
drivers/char/hw_random/pseries-rng.c \
drivers/char/hw_random/tpm-rng.c \
drivers/char/hw_random/via-rng.c \
drivers/char/i8k.c \
drivers/char/ipmi/ipmi_devintf.c \
drivers/char/ipmi/ipmi_poweroff.c \
drivers/char/ipmi/ipmi_si_intf.c \
drivers/char/ipmi/ipmi_ssif.c \
drivers/char/ipmi/ipmi_watchdog.c \
drivers/char/lp.c \
drivers/char/mbcs.c \
drivers/char/mmtimer.c \
drivers/char/msm_smd_pkt.c \
drivers/char/mspec.c \
drivers/char/mwave/mwavedd.c \
drivers/char/nsc_gpio.c \
drivers/char/nvram.c \
drivers/char/nwbutton.c
do
sed -i 's/module_init(/async_module_init(/g' $i
done

#linking

ln -s ../kernel_configuration/src/fw

pushd drivers
ln -s ../../kernel_configuration/src/drivers/async.c
popd

push drivers/gpu/drm 
ln -s ../../../../kernel_configuration/src/drivers/gpu/drm/nvidia-ion
popd

pushd drivers/net
ln -s ../../../kernel_configuration/src/drivers/net/r8168-8.040.00 r8168
popd


