/*
 * async_minit.h
 *
 *  Created on: 18 Apr 2015
 *      Author: lester
 *
 *  Async module initialization header file
 */

#ifndef ASYNC_MINIT_H_
#define ASYNC_MINIT_H_

#include <linux/init.h>

/**
 * Full list of module init functions call
 */
#define INIT_CALLS(fnc) \
        fnc(ahci_pci_driver),      /* */ \
        fnc(crypto_xcbc_module_init), /* */ \
        fnc(init_cifs), /* */  \
        fnc(drm_fb_helper_modinit), /* */  \
        fnc(acpi_button_driver_init), /* */  \
        fnc(init_iso9660_fs), /* */  \
        fnc(bt_init),      /* bluetooth.ko */  \
        fnc(rfcomm_init),  /* rfcomm.ko */ \
        fnc(init_mtd2),  /* soundcore.ko */ \
        fnc(init_mtd3),  /* snd.ko */ \
        fnc(alsa_timer_init),  /* snd-timer.ko */ \
        fnc(alsa_hwdep_init),  /* snd-hwdep.ko */ \
        fnc(alsa_seq_device_init),  /* snd-seq-device.ko */ \
        fnc(alsa_mixer_oss_init),  /* snd-mixer-oss.ko */ \
        fnc(snd_hrtimer_init),  /* snd-hrtimer.ko */ \
        fnc(alsa_pcm_oss_init),  /* snd-pcm.ko */ \
        fnc(alsa_seq_init),  /* snd-seq.ko */ \
        fnc(init_mtd11),  /* snd-pcm-oss.ko */ \
        fnc(init_mtd12),  /* snd-hda-codec.ko */ \
        fnc(alsa_seq_midi_init),  /* snd-seq-midi-event.ko */ \
        fnc(alsa_seq_dummy_init),  /* snd-seq-dummy.ko */ \
        fnc(init_mtd15),  /* snd-hda-controller.ko */ \
        fnc(init_mtd16),  /* snd-hda-codec-generic.ko */ \
        fnc(patch_si3054_init),  /* snd-hda-codec-si3054.ko */ \
        fnc(patch_ca0132_init),  /* snd-hda-codec-ca0132.ko */ \
        fnc(patch_hdmi_init),  /* snd-hda-codec-hdmi.ko */ \
        fnc(init_mtd20),  /* snd-seq-oss.ko */ \
        fnc(init_mtd21),  /* snd-hda-intel.ko */ \
        fnc(patch_sigmantel_init),  /* snd-hda-codec-idt.ko */ \
        fnc(patch_cirrus_init),  /* snd-hda-codec-cirrus.ko */ \
        fnc(patch_ca0110_init),  /* snd-hda-codec-ca0110.ko */ \
        fnc(patch_via_init),  /* snd-hda-codec-via.ko */ \
        fnc(patch_realtek_init),  /* snd-hda-codec-realtek.ko */ \
        fnc(patch_conexant_init),  /* snd-hda-codec-conexant.ko */ \
        fnc(patch_cmedia_init),  /* snd-hda-codec-cmedia.ko */ \
        fnc(patch_analog_init),  /* snd-hda-codec-analog.ko */ \
        fnc(init_mtd30),  /* hwmon.ko */ \
        fnc(init_mtd31),  /* coretemp.ko */ \
        fnc(init_mtd32),  /* gpio-fan.ko */ \
        fnc(acpi_processor_driver_init),  /* acpi-power-meter.ko */ \
        fnc(init_mtd),  /* mtd.ko */ \
        fnc(ubi_init),  /* ubi.ko */ \
        fnc(init_mtd36),  /* uio.ko */ \
        fnc(init_mtd37),  /* uio_cif.ko */ \
        fnc(init_mtd38),  /* libata.ko */ \
        fnc(init_mtd39),  /* libahci.ko */ \
        fnc(ahci_pci_driver_init),  /* ahci.ko */ \
        fnc(init_mtd41),  /* wmi.ko */ \
        fnc(init_mtd42),  /* mxm-wmi.ko */ \
        fnc(init_mtd43),  /* speedstep-lib.ko */ \
        fnc(init_mtd44),  /* speedstep-ich.ko */ \
        fnc(init_mtd45),  /* mmc_core.ko */ \
        fnc(init_mtd46),  /* mmc_block.ko */ \
        fnc(init_mtd47),  /* videodev.ko */ \
        fnc(init_mtd48),  /* v4l2-common.ko */ \
        fnc(init_mtd49),  /* videobuf2-memops.ko */ \
        fnc(init_mtd50),  /* videobuf2-core.ko */ \
        fnc(init_mtd51),  /* videobuf2-vmalloc.ko */ \
        fnc(init_mtd52),  /* uvcvideo.ko */ \
        fnc(init_mtd53),  /* gspca_main.ko */ \
        fnc(init_mtd54),  /* rc-core.ko */ \
        fnc(init_mtd55),  /* ir-kbd-i2c.ko */ \
        fnc(init_mtd56),  /* i2c-mux.ko */ \
        fnc(init_mtd57),  /* i2c-mux-gpio.ko */ \
        fnc(init_mtd58),  /* i2c-mux-pca9541.ko */ \
        fnc(init_mtd59),  /* i2c-mux-pca954x.ko */ \
        fnc(init_mtd60),  /* usb-common.ko */ \
        fnc(init_mtd61),  /* usb-core.ko */ \
        fnc(uhci_hcd_init),    /* uhci-hcd.ko */ \
        fnc(init_mtd64),     /* usbmon.ko */ \
        fnc(init_mtd65),     /* usb-storage.ko */ \
        fnc(init_mtd66),     /* usbled.ko */ \
        fnc(uhid_init),       /* usbhid.ko */ \
        fnc(hid_init),        /* usbhid.ko */ \
        fnc(elo_driver_init),  /* usbhid.ko */ \
        fnc(ehci_hcd_init),  /* ehci-hcd.ko */ \
        fnc(ohci_pci_init),  /* ohci-pci.ko */ \
        fnc(ohci_platform_init),  /* ohci-platform.ko */ \
        fnc(ohci_hcd_mod_init),  /* ohci-hcd.ko */ \
        fnc(ehci_platform_init),  /* ehci-platform.ko */ \
        fnc(agp_nvidia_init),  /* nvidia-agp.ko */ \
        fnc(nvidia_frontend_init_module),  /* nvidia.ko */ \
        fnc(init_mtd71),  /* ums-eneub6250.ko */ \
        fnc(init_mtd72),  /* uas.ko */ \
        fnc(init_mtd73),  /* ums-realtek.ko */ \
        fnc(ehci_pci_init),  /* ehci-pci.ko */ \
        fnc(init_mtd76),  /* libphy.ko */ \
        fnc(init_mtd77),  /* smsc.ko */ \
        fnc(init_mtd78),  /* lib80211.ko */ \
        fnc(init_mtd79),  /* lib80211_crypt_tkip.ko */ \
        fnc(init_mtd80),  /* lib80211_crypt_wep.ko */ \
        fnc(init_mtd81),  /* lib80211_crypt_ccmp.ko */ \
        fnc(init_mtd82),  /* libipw.ko */ \
        fnc(init_mtd83),  /* cfg80211.ko */ \
        fnc(init_mtd84),  /* mac80211.ko */ \
        fnc(init_mtd85),  /* led-class.ko */ \
        fnc(init_mtd86),  /* rng-core.ko */ \
        fnc(init_mtd87),  /* ipw2100.ko */ \
        fnc(init_mtd88),  /* leds-pca955x.ko */ \
        fnc(init_mtd89),  /* b43.ko */ \
        fnc(init_mtd90),  /* b43legacy.ko */ \
        fnc(init_mtd91),  /* intel-rng.ko */ \
        fnc(agp_init),  /* agpgart.ko */ \
        fnc(drm_core),  /* drm.ko */ \
        fnc(uvm_init),  /* nvidia-uvm.ko */ \
        fnc(init_mtd100),  /* dca.ko */ \
        fnc(init_mtd101),  /* ioatdma.ko */ \
        fnc(init_mtd102),  /* backlight.ko */ \
        fnc(init_mtd103),  /* video.ko */ \
        fnc(init_mtd104),  /* fbdev.ko */ \
        fnc(init_mtd105),  /* fb.ko */ \
        fnc(init_mtd106),  /* af_alg.ko */ \
        fnc(init_mtd107),  /* algif_skicupher.ko */ \
        fnc(init_mtd108),  /* alg_hash.ko */ \
        fnc(init_mtd109),  /* softcursor.ko */ \
        fnc(init_mtd110),  /* font.ko */ \
        fnc(init_mtd111),  /* bitblit.ko */ \
        fnc(init_mtd112),  /* fbcon.ko */ \
        fnc(init_mtd113),  /* lzo_decompress.ko */ \
        fnc(init_mtd114),  /* lzo_compress.ko */ \
        fnc(init_mtd115),  /* lzo.ko */ \
        fnc(init_mtd116),  /* authenc.ko */ \
        fnc(init_mtd117),  /* authencesn.ko */ \
        fnc(init_mtd118),  /* cast_common.ko */ \
        fnc(init_mtd119),  /* cat5_generic.ko */ \
        fnc(init_mtd120),  /* cast6_generic.ko */ \
        fnc(init_mtd121),  /* blowfish_common.ko */ \
        fnc(init_mtd122),  /* blowfish_generic.ko */ \
        fnc(init_mtd123),  /* zlib_deflate.ko */ \
        fnc(init_mtd124),  /* zlib_inflate.ko */ \
        fnc(init_mtd125),  /* zlib.ko */ \
        fnc(prgn_mod_init), /**/ \
        fnc(blowfish_mod_init), /**/ \
        fnc(crypto_cbc_module_init), /**/ \
        fnc(crc32_mod_init), /**/ \
    fnc(twofish_mod_init), /**/ \
    fnc(wp512_mod_init), /**/ \
    fnc(crct10dif_mod_init), /**/ \
    fnc(crypto_null_mod_init), /**/ \
    fnc(crypto_ecb_module_init), /**/ \
    fnc(crypto_module_init), /**/ \
    fnc(crypto_user_init), /**/ \
    fnc(lz4_mod_init), /**/ \
    fnc(lzo_mod_init), /**/ \
    fnc(md4_mod_init), /**/ \
    fnc(md5_mod_init), /**/ \
    fnc(rmd128_mod_init), /**/ \
    fnc(rmd160_mod_init), /**/ \
    fnc(rmd256_mod_init), /**/ \
    fnc(rmd320_mod_init), /**/ \
    fnc(sha1_generic_mod_init), /**/ \
    fnc(sha256_generic_mod_init), /**/ \
    fnc(sha512_generic_mod_init), /**/ \
    fnc(tcrypt_mod_init), /**/ \
    fnc(tea_mod_init), /**/ \
    fnc(tgr192_mod_init), /**/ \
        fnc(pcie_portdrv_init), /**/ \
        fnc(init_mtd126),  /* iosfs.ko */ \
        fnc(init_mtd127),  /* fuse.ko */ \
        fnc(init_mtd128),  /* cuse.ko */ \
        fnc(init_mtd129),  /* jbd.ko */ \
        fnc(init_mtd130),  /* ext3.ko */ \
        fnc(init_fat_fs),  /* fat.ko */ \
        fnc(init_vfat_fs),  /* vfat.ko */ \
        fnc(init_msdos_fs),  /* msdos.ko */ \
        fnc(init_ntfs_fs), /**/ \
        fnc(acpi_video_init),/**/ \
        fnc(acpi_ipmi_init),/**/ \
        fnc(acpi_pad_init),/**/ \
        fnc(acpi_battery_init),/**/ \
        fnc(acpi_sbs_init),/**/ \
        fnc(acpi_thermal_init),/**/ \
        fnc(cpufreq_gov_dbs_init), /**/ \
        fnc(cpufreq_gov_powersave_init), /**/ \
        fnc(cpufreq_stats_init), /**/ \
        fnc(cpufreq_gov_userspace_init), /**/ \
        fnc(hpet_init), /**/ \
        fnc(pci_hotplug_init), /**/ \
        fnc(pcied_init), /**/ \
        fnc(shpcd_init), /**/ \
        fnc(init_mtd134),  /* twofish_common.ko */ \
        fnc(init_mtd135),  /* twofish_generic.ko */ \
        fnc(init_mtd136),  /* twofish-i586.ko */ \
        fnc(asymmetric_key_init), /**/ \
        fnc(pkcs7_key_init), /**/ \
        fnc(x509_key_init), /**/ \
        fnc(aes_init), /**/ \
        fnc(acpi_ac_init) /**/

        //fnc(init_msdos_fs), /* msdos.ko */

#define macro_str(x)    #x
#define macro_name(x)   x ## _id

/**
 * Implement static enum to string map
 */
typedef enum   {
    none = 0,           // not found or not element
    INIT_CALLS(macro_name)
    ,end
} modules_e;


/**
 * Task type or execution priority
 * async - needs to be execute in an asynchronized way
 * deferred - it can be execute at user initialization time
 */
typedef enum { asynchronized, deferred } task_type_t;

struct init_fn_t
{
  task_type_t type_;    // task type
  modules_e  id;
  initcall_t fnc;
};

/*
 * TODO Disable when module build as module
 * add support for bus devices like acpi, pci, etc
*/
//
#ifdef CONFIG_ASYNCHRO_MODULE_INIT

//
#define async_module_init(fnc, ... )  \
  static struct init_fn_t init_fn_##fnc __used \
  __attribute__((__section__(".async_initcall.init"))) = {asynchronized,fnc ## _id,fnc};

//
#define deferred_module_init(fnc,...)  \
  static struct init_fn_t init_fn_##fnc __used \
  __attribute__((__section__(".async_initcall.init"))) = {deferred,fnc ## _id,fnc};


// Usefull for ACPI and USB maybe PCI
#define async_module_driver(__driver, __register, __unregister,__depends, ...) \
static int __init __driver##_init(void) \
{ \
  return __register(&(__driver) , ##__VA_ARGS__); \
} \
async_module_init(__driver##_init,__depends); \
static void __exit __driver##_exit(void) \
{ \
  __unregister(&(__driver) , ##__VA_ARGS__); \
} \
module_exit(__driver##_exit);

//
#define deferred_module_driver(__driver, __register, __unregister,__depends, ...) \
static int __init __driver##_init(void) \
{ \
  return __register(&(__driver) , ##__VA_ARGS__); \
} \
deferred_module_init(__driver##_init,__depends); \
static void __exit __driver##_exit(void) \
{ \
  __unregister(&(__driver) , ##__VA_ARGS__); \
} \
module_exit(__driver##_exit);

//
#define async_module_pci_driver(__pci_driver) \
  async_module_driver(__pci_driver, pci_register_driver, \
           pci_unregister_driver,)

#define deferred_module_pci_driver(__pci_driver) \
    deferred_module_driver(__pci_driver, pci_register_driver, \
           pci_unregister_driver,)

#else

#define async_module_init(fnc, ... )      module_init(fnc);
#define deferred_module_init(fnc, ... )   module_init(fnc);

#define async_module_driver(__driver, __register, __unregister,__depends, ...)    module_driver(__driver, __register, __unregister, ##__VA_ARGS__);
#define deferred_module_driver(__driver, __register, __unregister,__depends, ...) module_driver(__driver, __register, __unregister, ##__VA_ARGS__);

#endif


#endif /* ASYNC_MINIT_H_ */
