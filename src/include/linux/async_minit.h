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
        fnc(crypto_xcbc_module_init), /* */ \
        fnc(init_cifs),               /* */  \
        fnc(drm_fb_helper_modinit),   /* */  \
        fnc(acpi_button_driver_init), /* */  \
        fnc(acpi_power_meter_init), /* */  \
        fnc(synusb_driver_init),    /*   */ \
        fnc(usblp_driver_init),    /*   */ \
        fnc(rfcomm_init),            /* rfcomm.ko */ \
        fnc(alsa_timer_init),        /* snd-timer.ko */ \
        fnc(alsa_hwdep_init),        /* snd-hwdep.ko */ \
        fnc(alsa_seq_device_init),   /* snd-seq-device.ko */ \
        fnc(alsa_mixer_oss_init),    /* snd-mixer-oss.ko */ \
        fnc(snd_hrtimer_init),       /* snd-hrtimer.ko */ \
        fnc(alsa_pcm_init),          /* snd-pcm.ko */ \
        fnc(alsa_seq_init),          /* snd-seq.ko */ \
        fnc(alsa_pcm_oss_init),      /* snd-pcm-oss.ko */ \
        fnc(alsa_seq_midi_event_init),     /* snd-seq-midi-event.ko */ \
        fnc(alsa_seq_dummy_init),    /* snd-seq-dummy.ko */ \
        fnc(snd_hda_controller),     /* snd-hda-controller.ko */ \
        fnc(patch_si3054_init),      /* snd-hda-codec-si3054.ko */ \
        fnc(patch_ca0132_init),      /* snd-hda-codec-ca0132.ko */ \
        fnc(patch_hdmi_init),        /* snd-hda-codec-hdmi.ko */ \
        fnc(alsa_seq_oss_init),      /* snd-seq-oss.ko */ \
        fnc(snd_hda_intel),          /* snd-hda-intel.ko */ \
        fnc(patch_sigmatel_init),   /* snd-hda-codec-idt.ko */ \
        fnc(patch_cirrus_init),      /* snd-hda-codec-cirrus.ko */ \
        fnc(patch_ca0110_init),      /* snd-hda-codec-ca0110.ko */ \
        fnc(patch_via_init),         /* snd-hda-codec-via.ko */ \
        fnc(patch_realtek_init),     /* snd-hda-codec-realtek.ko */ \
        fnc(patch_conexant_init),    /* snd-hda-codec-conexant.ko */ \
        fnc(patch_cmedia_init),      /* snd-hda-codec-cmedia.ko */ \
        fnc(patch_analog_init),      /* snd-hda-codec-analog.ko */ \
        fnc(coretemp),  /* coretemp.ko */ \
        fnc(gpio_fan),  /* gpio-fan.ko */ \
        fnc(acpi_processor_driver_init),  /* acpi-power-meter.ko */ \
        fnc(init_mtd),  /* mtd.ko */ \
        fnc(ubi_init),  /* ubi.ko */ \
        fnc(uio_init),  /* uio.ko */ \
        fnc(hilscher_pci_driver_init),  /* uio_cif.ko */ \
        fnc(ahci_pci_driver_init),  /* ahci.ko */ \
        fnc(mxm_wmi_init),  /* mxm-wmi.ko */ \
        fnc(speedstep_init),  /* speedstep-ich.ko */ \
        fnc(mmc_blk_init),  /* mmc_block.ko */ \
        fnc(uvcvideo),  /* uvcvideo.ko */ \
        fnc(gspca_main),  /* gspca_main.ko */ \
        fnc(ir_kbd_driver),  /* ir-kbd-i2c.ko module_i2c_driver */ \
        fnc(i2c_mux_gpio_driver),  /* i2c-mux-gpio.ko module_platform_driver */ \
        fnc(pca9541_driver),  /* i2c-mux-pca9541.ko module_i2c_driver */ \
        fnc(pca954x_driver),  /* i2c-mux-pca954x.ko module_i2c_driver */ \
        fnc(uhci_hcd_init),    /* uhci-hcd.ko */ \
        fnc(usbmon),     /* usbmon.ko */ \
        fnc(usb_storage_driver_init),     /* usb-storage.ko */ \
        fnc(led_driver_init),     /* usbled.ko */ \
        fnc(hid_init),        /* usbhid.ko */ \
		fnc(uhid_init),		 /* uhid.ko */ \
        fnc(ene_ub6250_driver_init),  /* ums-eneub6250.ko */ \
        fnc(uas_driver_init),  /* uas.ko */ \
        fnc(realtek_cr_driver_init),  /* ums-realtek.ko */ \
        fnc(ehci_pci_init),  /* ehci-pci.ko */ \
        fnc(libphy),  /* libphy.ko */ \
        fnc(smsc),  /* smsc.ko */ \
        fnc(lib80211_init),  /* lib80211.ko */ \
        fnc(lib80211_crypto_tkip_init),  /* lib80211_crypt_tkip.ko */ \
        fnc(lib80211_crypto_wep_init),  /* lib80211_crypt_wep.ko */ \
        fnc(lib80211_crypto_ccmp_init),  /* lib80211_crypt_ccmp.ko */ \
        fnc(libipw_init),  /* libipw.ko */ \
        fnc(led_class),  /* led-class.ko */ \
        fnc(hwrng_modinit),  /* rng-core.ko */ \
        fnc(prng_mod_init), /* */ \
        fnc(ipw2100_init),  /* ipw2100.ko */ \
        fnc(leds_pca955x),  /* leds-pca955x.ko */ \
        fnc(b43),  /* b43.ko */ \
        fnc(b43legacy),  /* b43legacy.ko */ \
        fnc(intel_rng_mod_init),  /* intel-rng.ko */ \
        fnc(agp_init),  /* agpgart.ko */ \
        fnc(drm_core_init),  /* drm.ko */ \
        fnc(uvm_init),  /* nvidia-uvm.ko */ \
        fnc(ioat_init_module),  /* ioatdma.ko */ \
        fnc(acpi_video_init),  /* video.ko */ \
        fnc(af_alg_init),  /* af_alg.ko */ \
        fnc(algif_hash_init),  /* algif_hash.ko */ \
        fnc(algif_skcipher_init),  /* algif_skicupher.ko */ \
        fnc(alg_hash),  /* alg_hash.ko */ \
        fnc(lzo_mod_init),  /* lzo.ko */ \
        fnc(crypto_authenc_module_init),  /* authenc.ko */ \
        fnc(crypto_authenc_esn_module_init),  /* authencesn.ko */ \
        fnc(cast5_mod_init),  /* cat5_generic.ko */ \
        fnc(cast6_mod_init),  /* cast6_generic.ko */ \
        fnc(blowfish_mod_init),  /* blowfish_generic.ko */ \
        fnc(zlib_mod_init),  /* zlib.ko */ \
        fnc(prgn_mod_init), /**/ \
        fnc(crypto_cbc_module_init), /**/ \
        fnc(crc32_mod_init), /**/ \
		fnc(crc32c_mod_init), /**/ \
        fnc(twofish_mod_init), /**/ \
        fnc(wp512_mod_init), /**/ \
        fnc(crct10dif_mod_init), /**/ \
        fnc(crypto_null_mod_init), /**/ \
        fnc(crypto_ecb_module_init), /**/ \
        fnc(crypto_module_init), /**/ \
        fnc(crypto_user_init), /**/ \
        fnc(lz4_mod_init), /**/ \
        fnc(md4_mod_init), /**/ \
        fnc(md5_mod_init), /**/ \
        fnc(rmd128_mod_init), /**/ \
        fnc(rmd160_mod_init), /**/ \
        fnc(rmd256_mod_init), /**/ \
        fnc(rmd320_mod_init), /**/ \
        fnc(sha1_generic_mod_init), /**/ \
        fnc(sha256_generic_mod_init), /**/ \
        fnc(sha512_generic_mod_init), /**/ \
        fnc(elo_driver_init),  /* usbhid.ko */ \
        fnc(ehci_hcd_init),  /* ehci-hcd.ko */ \
        fnc(ohci_pci_init),  /* ohci-pci.ko */ \
        fnc(ohci_platform_init),  /* ohci-platform.ko */ \
        fnc(ohci_hcd_mod_init),  /* ohci-hcd.ko */ \
        fnc(ehci_platform_init),  /* ehci-platform.ko */ \
        fnc(agp_nvidia_init),  /* nvidia-agp.ko */ \
        fnc(nvidia_frontend_init_module),  /* nvidia.ko */ \
        fnc(tcrypt_mod_init), /**/ \
        fnc(tea_mod_init), /**/ \
        fnc(tgr192_mod_init), /**/ \
        fnc(pcie_portdrv_init), /**/ \
        fnc(init_iso9660_fs),  /* isofs.ko */ \
        fnc(fuse_init),  /* fuse.ko */ \
        fnc(cuse_init),  /* cuse.ko */ \
        fnc(journal_init),  /* jbd.ko */ \
        fnc(init_ext3_fs),  /* ext3.ko */ \
        fnc(init_fat_fs),  /* fat.ko */ \
        fnc(init_vfat_fs),  /* vfat.ko */ \
        fnc(init_msdos_fs),  /* msdos.ko */ \
        fnc(init_ntfs_fs), /**/ \
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
        fnc(twofish_generic),  /* twofish_generic.ko */ \
        fnc(twofish_i586),  /* twofish-i586.ko */ \
        fnc(asymmetric_key_init), /**/ \
        fnc(pkcs7_key_init), /**/ \
        fnc(x509_key_init), /**/ \
        fnc(aes_init), /**/ \
        fnc(vmac_module_init), /**/ \
        fnc(mousedev_init), /**/ \
        fnc(acpi_ac_init), /**/ \
        fnc(atkbd_init), /**/\
        fnc(uinput_init), /**/\
        fnc(psmouse_init), /**/\
        fnc(serport_init), /**/\
        fnc(b43_init), /**/\
        fnc(vb2_thread_init), /**/\
        fnc(b43legacy_init), /**/\
        fnc(azx_driver_init), /**/\
        fnc(i8042_init) /**/

#if 0
        /* ARCH  SUBSYS POSTCORE */
        fnc(dca),  /* dca.ko */  \
        fnc(backlight),  /* backlight.ko */ \
        fnc(cast_common),  /* cast_common.ko */ \
        fnc(zlib_deflate),  /* zlib_deflate.ko */ \
        fnc(zlib_inflate),  /* zlib_inflate.ko */ \
        fnc(blowfish_common),  /* blowfish_common.ko */ \
        fnc(ieee80211_init),  /* mac80211.ko */ \
        fnc(v4l2_common),  /* v4l2-common.ko */ \
        fnc(videobuf2_memops),  /* videobuf2-memops.ko */ \
        fnc(videobuf2_core),  /* videobuf2-core.ko */ \
        fnc(videobuf2_vmalloc),  /* videobuf2-vmalloc.ko */ \
        fnc(rc_core),  /* rc-core.ko */ \
        fnc(i2c_mux),  /* i2c-mux.ko */ \
        fnc(libata),  /* libata.ko */ \
        fnc(libahci),  /* libahci.ko */ \
        fnc(bt_init),                 /* bluetooth.ko */  \
        fnc(soundcore),              /* soundcore.ko */ \
        fnc(snd),              /* snd.ko */ \
        fnc(hwmon),             /* hwmon.ko */ \
        fnc(wmi),  /* wmi.ko */ \
        fnc(mmc_core),  /* mmc_core.ko */ \
        fnc(speedstep),  /* speedstep-lib.ko */ \
        fnc(cfg80211),  /* cfg80211.ko */ \
        fnc(fbdev),  /* fbdev.ko */ \
        fnc(fb),  /* fb.ko */ \
        fnc(softcursor),  /* softcursor.ko */ \
        fnc(font),  /* font.ko */ \
        fnc(bitblit),  /* bitblit.ko */ \
        fnc(fb_console_init),  /* fbcon.ko */ \
        fnc(lzo_decompress),  /* lzo_decompress.ko */ \
        fnc(lzo_compress),  /* lzo_compress.ko */ \
        fnc(twofish_common),  /* twofish_common.ko */ \
        fnc(videodev_init),  /* videodev.ko */ \
        fnc(usb_common),  /* usb-common.ko */ \
        fnc(usb_core),  /* usbcore.ko */ \
        fnc(snd_hda_codec),          /* snd-hda-codec.ko */ \
        fnc(snd_hda_codec_generic),  /* snd-hda-codec-generic.ko */ \

#endif


        //fnc(init_msdos_fs), /* msdos.ko */

#define MOD_DEPENDENCY_ITEM(child,parent)	{ child ## _id, parent ## _id }

#define ADD_MODULE_DEPENDENCY(child,parent) \
         static struct dependency_t child##parent __used \
		 __attribute__((__section__(".async_modules_depends.init"))) = \
		 {child ## _id, parent ## _id};    

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

struct dependency_t
{
    modules_e task_id;
    modules_e parent_id;
};

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
#if defined(CONFIG_ASYNCHRO_MODULE_INIT) && !defined(MODULE)

#define async_module_init(fnc)  \
  static struct init_fn_t init_fn_##fnc __used \
  __attribute__((__section__(".async_initcall.init"))) = {asynchronized,fnc ## _id,fnc};

//
#define deferred_module_init(fnc)  \
  static struct init_fn_t init_fn_##fnc __used \
  __attribute__((__section__(".async_initcall.init"))) = {deferred,fnc ## _id,fnc};

// Usefull for ACPI and USB maybe PCI
#define async_module_driver(__driver, __register, __unregister) \
static int __init __driver##_init(void) \
{ \
  return __register(&(__driver)); \
} \
async_module_init(__driver##_init); \
static void __exit __driver##_exit(void) \
{ \
  __unregister(&(__driver)); \
} \
module_exit(__driver##_exit);

// ACPI USB PCI
#define deferred_module_driver(__driver, __register, __unregister) \
static int __init __driver##_init(void) \
{ \
  return __register(&(__driver)); \
} \
deferred_module_init(__driver##_init); \
static void __exit __driver##_exit(void) \
{ \
  __unregister(&(__driver)); \
} \
module_exit(__driver##_exit);

//
#define async_module_pci_driver(__pci_driver) async_module_driver(__pci_driver, pci_register_driver,pci_unregister_driver);
#define deferred_module_pci_driver(__pci_driver) deferred_module_driver(__pci_driver, pci_register_driver,pci_unregister_driver);

#define async_module_platform_driver(__platform_driver) \
    async_module_driver(__platform_driver, platform_driver_register,platform_driver_unregister);

#else

#define async_module_init(fnc)      module_init(fnc);
#define deferred_module_init(fnc)   module_init(fnc);

#define async_module_driver(__driver, __register, __unregister)    module_driver(__driver, __register, __unregister);
#define deferred_module_driver(__driver, __register, __unregister) module_driver(__driver, __register, __unregister);

#define async_module_pci_driver(__pci_driver)    module_pci_driver(__pci_driver);
#define deferred_module_pci_driver(__pci_driver) module_pci_driver(__pci_driver);

#define async_module_platform_driver(__platform_driver) module_driver(__platform_driver, platform_driver_register,platform_driver_unregister);

#endif


#endif /* ASYNC_MINIT_H_ */
