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
    fnc(ahci_pci_driver_init,asynchronized)  /* ahci.ko */ \
    \
    fnc(ahci_driver_init,asynchronized,ahci_pci_driver_init) /**/ \
    \
    fnc(acpi_ac_init,asynchronized) /**/ \
    \
    fnc(serial8250_init,deferred) /* */ \
    \
    fnc(nforce2_driver_init,deferred) /* */ \
    \
    fnc(crypto_xcbc_module_init,deferred) /* */ \
    \
    fnc(forcedeth_pci_driver_init,deferred) /* */ \
    \
    fnc(init_cifs,deferred)               /* */  \
    \
    fnc(acpi_pcc_driver,deferred,acpi_ac_init) \
    \
    fnc(acpi_hed_driver,deferred)               /* */  \
    \
    fnc(acpi_smb_hc_driver,deferred,acpi_ac_init)               /* */  \
    \
    fnc(crb_acpi_driver,deferred,acpi_ac_init)               /* */  \
    \
    fnc(acpi_smbus_cmi_driver,deferred)               /* */  \
    \
    fnc(atlas_acpi_driver,deferred,acpi_ac_init)               /* */  \
    \
    fnc(smo8800_driver,deferred,acpi_ac_init)               /* */  \
    \
    fnc(lis3lv02d_driver,deferred,acpi_ac_init)               /* */  \
    \
    fnc(irst_driver,deferred,acpi_ac_init)               /* */  \
    \
    fnc(smartconnect_driver,deferred,acpi_ac_init)               /* */  \
    \
    fnc(pvpanic_driver,deferred,acpi_ac_init)               /* */  \
    \
    fnc(acpi_topstar_driver,deferred,acpi_ac_init)               /* */  \
    \
    fnc(toshiba_bt_rfkill_driver,deferred,acpi_ac_init)               /* */  \
    \
    fnc(toshiba_haps_driver,deferred,acpi_ac_init)               /* */  \
    \
    fnc(xo15_ebook_driver,deferred,acpi_ac_init)               /* */  \
    \
    fnc(drm_fb_helper_modinit,deferred)   /* */  \
    \
    fnc(acpi_button_driver_init,deferred) /* */  \
    \
    fnc(acpi_power_meter_init,deferred) /* */  \
    \
    fnc(synusb_driver_init,deferred)    /*   */ \
    \
    fnc(usblp_driver_init,deferred)    /*   */ \
    \
    fnc(rfcomm_init,deferred)            /* rfcomm.ko */ \
    \
    fnc(alsa_timer_init,deferred)        /* snd-timer.ko */ \
    \
    fnc(alsa_hwdep_init,deferred)        /* snd-hwdep.ko */ \
    \
    fnc(alsa_seq_device_init,deferred)   /* snd-seq-device.ko */ \
    \
    fnc(alsa_mixer_oss_init,deferred)    /* snd-mixer-oss.ko */ \
    \
    fnc(snd_hrtimer_init,deferred,alsa_timer_init)   /* snd-hrtimer.ko */ \
    \
    fnc(alsa_pcm_init,deferred,alsa_timer_init)          /* snd-pcm.ko */ \
    \
    fnc(alsa_seq_init,deferred,alsa_seq_device_init,alsa_timer_init)          /* snd-seq.ko */ \
    \
    fnc(alsa_pcm_oss_init,deferred,alsa_mixer_oss_init,alsa_pcm_init)      /* snd-pcm-oss.ko */ \
    \
    fnc(alsa_seq_midi_event_init,deferred,alsa_seq_init)     /* snd-seq-midi-event.ko */ \
    \
    fnc(alsa_seq_dummy_init,deferred,alsa_seq_init)    /* snd-seq-dummy.ko */ \
    \
    fnc(snd_hda_controller,deferred)     /* snd-hda-controller.ko */ \
    \
    fnc(patch_si3054_init,deferred)      /* snd-hda-codec-si3054.ko */ \
    \
    fnc(patch_ca0132_init,deferred)      /* snd-hda-codec-ca0132.ko */ \
    \
    fnc(patch_hdmi_init,deferred)        /* snd-hda-codec-hdmi.ko */ \
    \
    fnc(alsa_seq_oss_init,deferred,alsa_seq_midi_event_init)      /* snd-seq-oss.ko */ \
    \
    fnc(snd_hda_intel,deferred,snd_hda_controller)          /* snd-hda-intel.ko */ \
    \
    fnc(patch_sigmatel_init,deferred)   /* snd-hda-codec-idt.ko */ \
    \
    fnc(patch_cirrus_init,deferred)      /* snd-hda-codec-cirrus.ko */ \
    \
    fnc(patch_ca0110_init,deferred)      /* snd-hda-codec-ca0110.ko */ \
    \
    fnc(patch_via_init,deferred)         /* snd-hda-codec-via.ko */ \
    \
    fnc(patch_realtek_init,deferred)     /* snd-hda-codec-realtek.ko */ \
    \
    fnc(patch_conexant_init,deferred)    /* snd-hda-codec-conexant.ko */ \
    \
    fnc(patch_cmedia_init,deferred)      /* snd-hda-codec-cmedia.ko */ \
    \
    fnc(patch_analog_init,deferred)      /* snd-hda-codec-analog.ko */ \
    \
    fnc(coretemp,deferred)  /* coretemp.ko */ \
    \
    fnc(gpio_fan,deferred)  /* gpio-fan.ko */ \
    \
    fnc(acpi_processor_driver_init,deferred)  /* acpi-power-meter.ko */ \
    \
    fnc(init_mtd,deferred)  /* mtd.ko */ \
    \
    fnc(ubi_init,deferred,init_mtd)  /* ubi.ko */ \
    \
    fnc(uio_init,deferred)  /* uio.ko */ \
    \
    fnc(hilscher_pci_driver_init,deferred,uio_init)  /* uio_cif.ko */ \
    \
    fnc(mxm_wmi_init,deferred)  /* mxm-wmi.ko */ \
    \
    fnc(speedstep_init,deferred)  /* speedstep-ich.ko */ \
    \
    fnc(mmc_blk_init,deferred)  /* mmc_block.ko */ \
    \
    fnc(uvcvideo,deferred)  /* uvcvideo.ko */ \
    \
    fnc(gspca_main,deferred)  /* gspca_main.ko */ \
    \
    fnc(ir_kbd_driver,deferred)  /* ir-kbd-i2c.ko module_i2c_driver */ \
    \
    fnc(i2c_mux_gpio_driver,deferred)  /* i2c-mux-gpio.ko module_platform_driver */ \
    \
    fnc(pca9541_driver,deferred)  /* i2c-mux-pca9541.ko module_i2c_driver */ \
    \
    fnc(pca954x_driver,deferred)  /* i2c-mux-pca954x.ko module_i2c_driver */ \
    \
    fnc(uhci_hcd_init,deferred,ehci_hcd_init)    /* uhci-hcd.ko */ \
    \
    fnc(usbmon,deferred)     /* usbmon.ko */ \
    \
    fnc(usb_storage_driver_init,deferred)     /* usb-storage.ko */ \
    \
    fnc(led_driver_init,deferred)     /* usbled.ko */ \
    \
    fnc(hid_generic_init,deferred,hid_init,uhid_init) /**/ \
    \
    fnc(hid_generic,deferred,hid_init,uhid_init) /**/ \
    \
    fnc(hid_init,deferred,ohci_platform_init)       /**/ \
    \
    fnc(cherry_driver_init,deferred,hid_init,uhid_init)  /**/ \
    \
    fnc(chicony_driver_init,deferred,hid_init,uhid_init) /**/ \
    \
    fnc(usb_hid_init,deferred)  /* usbhid.ko */ \
	\
	fnc(uhid_init,deferred,ohci_platform_init)		 /* uhid.ko */ \
	\
	fnc(apple_driver_init,deferred,hid_init,uhid_init) /**/ \
	\
	fnc(a4_driver_init,deferred,hid_init,uhid_init) /**/ \
    \
    fnc(ez_driver_init,deferred,hid_init,uhid_init) /**/ \
    \
    fnc(cp_driver_init,deferred,hid_init,uhid_init) /**/ \
    \
    fnc(ks_driver_init,deferred,hid_init,uhid_init) /**/ \
    \
    fnc(ms_driver_init,deferred,hid_init,uhid_init) /**/ \
    \
    fnc(lg_driver_init,deferred,hid_init,uhid_init) /**/ \
    \
    fnc(mr_driver_init,deferred,hid_init,uhid_init) /**/ \
    \
    fnc(belkin_driver_init,deferred,hid_init,uhid_init) /**/ \
    \
    fnc(plantronics_driver_init,deferred,hid_init,uhid_init) /**/ \
    \
    fnc(keytouch_driver_init,deferred,hid_init,uhid_init) /**/ \
    \
    fnc(ene_ub6250_driver_init,deferred,usb_storage_driver_init)  /* ums-eneub6250.ko */ \
    \
    fnc(uas_driver_init,deferred,usb_storage_driver_init)  /* uas.ko */ \
    \
    fnc(realtek_cr_driver_init,deferred,usb_storage_driver_init)  /* ums-realtek.ko */ \
    \
    fnc(ehci_pci_init,deferred,ehci_hcd_init)  /* ehci-pci.ko */ \
    \
    fnc(libphy,deferred)  /* libphy.ko */ \
    \
    fnc(smsc,deferred,libphy)  /* smsc.ko */ \
    \
    fnc(lib80211_init,deferred)  /* lib80211.ko */ \
    \
    fnc(lib80211_crypto_tkip_init,deferred,lib80211_init)  /* lib80211_crypt_tkip.ko */ \
    \
    fnc(lib80211_crypto_wep_init,deferred,lib80211_init)  /* lib80211_crypt_wep.ko */ \
    \
    fnc(lib80211_crypto_ccmp_init,deferred,lib80211_init)  /* lib80211_crypt_ccmp.ko */ \
    \
    fnc(libipw_init,deferred,lib80211_init)  /* libipw.ko */ \
    \
    fnc(led_class,deferred)  /* led-class.ko */ \
    \
    fnc(hwrng_modinit,deferred)  /* rng-core.ko */ \
    \
    fnc(prng_mod_init,deferred) /* */ \
    \
    fnc(ipw2100_init,deferred,lib80211_init)  /* ipw2100.ko */ \
    \
    fnc(leds_pca955x,deferred)  /* leds-pca955x.ko */ \
    \
    fnc(b43,deferred,hwrng_modinit)  /* b43.ko */ \
    \
    fnc(b43legacy,deferred,hwrng_modinit)  /* b43legacy.ko */ \
    \
    fnc(intel_rng_mod_init,deferred,hwrng_modinit)  /* intel-rng.ko */ \
    \
    fnc(agp_init,deferred)  /* agpgart.ko */ \
    \
    fnc(drm_core_init,deferred,agp_init)  /* drm.ko */ \
    \
    fnc(uvm_init,deferred,nvidia_frontend_init_module)  /* nvidia-uvm.ko */ \
    \
    fnc(ioat_init_module,deferred)  /* ioatdma.ko */ \
    \
    fnc(acpi_video_init,deferred)  /* video.ko */ \
    \
    fnc(af_alg_init,deferred)  /* af_alg.ko */ \
    \
    fnc(algif_hash_init,deferred,af_alg_init)  /* algif_hash.ko */ \
    \
    fnc(algif_skcipher_init,deferred,af_alg_init)  /* algif_skicupher.ko */ \
    \
    fnc(alg_hash,deferred)  /* alg_hash.ko */ \
    \
    fnc(lzo_mod_init,deferred)  /* lzo.ko */ \
    \
    fnc(crypto_authenc_module_init,deferred)  /* authenc.ko */ \
    \
    fnc(crypto_authenc_esn_module_init,deferred,crypto_authenc_module_init)  /* authencesn.ko */ \
    \
    fnc(cast5_mod_init,deferred)  /* cat5_generic.ko */ \
    \
    fnc(cast6_mod_init,deferred)  /* cast6_generic.ko */ \
    \
    fnc(blowfish_mod_init,deferred)  /* blowfish_generic.ko */ \
    \
    fnc(zlib_mod_init,deferred)  /* zlib.ko */ \
    \
    fnc(prgn_mod_init,deferred) /**/ \
    \
    fnc(crypto_cbc_module_init,deferred) /**/ \
    \
    fnc(crc32_mod_init,deferred) /**/ \
	\
	fnc(crc32c_mod_init,deferred) /**/ \
    \
    fnc(twofish_mod_init,deferred) /**/ \
    \
    fnc(wp512_mod_init,deferred) /**/ \
    \
    fnc(crct10dif_mod_init,deferred) /**/ \
    \
    fnc(crypto_null_mod_init,deferred) /**/ \
    \
    fnc(crypto_ecb_module_init,deferred) /**/ \
    \
    fnc(crypto_module_init,deferred) /**/ \
    \
    fnc(crypto_user_init,deferred) /**/ \
    \
    fnc(lz4_mod_init,deferred) /**/ \
    \
    fnc(md4_mod_init,deferred) /**/ \
    \
    fnc(md5_mod_init,deferred) /**/ \
    \
    fnc(rmd128_mod_init,deferred) /**/ \
    \
    fnc(rmd160_mod_init,deferred) /**/ \
    \
    fnc(rmd256_mod_init,deferred) /**/ \
    \
    fnc(rmd320_mod_init,deferred) /**/ \
    \
    fnc(sha1_generic_mod_init,deferred) /**/ \
    \
    fnc(sha256_generic_mod_init,deferred) /**/ \
    \
    fnc(sha512_generic_mod_init,deferred) /**/ \
    \
    fnc(elo_driver_init,deferred)  /* usbhid.ko */ \
    \
    fnc(ehci_hcd_init,deferred)  /* ehci-hcd.ko */ \
    \
    fnc(ohci_pci_init,deferred,ohci_hcd_mod_init,ehci_hcd_init)  /* ohci-pci.ko */ \
    \
    fnc(ohci_platform_init,deferred,ohci_hcd_mod_init)  /* ohci-platform.ko */ \
    \
    fnc(ohci_hcd_mod_init,deferred,ehci_platform_init)  /* ohci-hcd.ko */ \
    \
    fnc(ehci_platform_init,deferred,ehci_hcd_init)  /* ehci-platform.ko */ \
    \
    fnc(agp_nvidia_init,deferred,agp_init)  /* nvidia-agp.ko */ \
    \
    fnc(nvidia_frontend_init_module,deferred,drm_core_init)  /* nvidia.ko */ \
    \
    fnc(tcrypt_mod_init,deferred) /**/ \
    \
    fnc(tea_mod_init,deferred) /**/ \
    \
    fnc(tgr192_mod_init,deferred) /**/ \
    \
    fnc(pcie_portdrv_init,deferred) /**/ \
    \
    fnc(init_iso9660_fs,deferred)  /* isofs.ko */ \
    \
    fnc(fuse_init,deferred)  /* fuse.ko */ \
    \
    fnc(cuse_init,deferred,fuse_init)  /* cuse.ko */ \
    \
    fnc(journal_init,deferred)  /* jbd.ko */ \
    \
    fnc(init_ext3_fs,deferred,journal_init)  /* ext3.ko */ \
    \
    fnc(init_fat_fs,deferred)  /* fat.ko */ \
    \
    fnc(init_vfat_fs,deferred,init_fat_fs)  /* vfat.ko */ \
    \
    fnc(init_msdos_fs,deferred,init_fat_fs)  /* msdos.ko */ \
    \
    fnc(init_ntfs_fs,deferred) /**/ \
    \
    fnc(acpi_ipmi_init,deferred)/**/ \
    \
    fnc(acpi_pad_init,deferred)/**/ \
    \
    fnc(acpi_battery_init,deferred)/**/ \
    \
    fnc(acpi_sbs_init,deferred)/**/ \
    \
    fnc(acpi_thermal_init,deferred)/**/ \
    \
    fnc(cpufreq_gov_dbs_init,deferred) /**/ \
    \
    fnc(cpufreq_gov_powersave_init,deferred) /**/ \
    \
    fnc(cpufreq_stats_init,deferred) /**/ \
    \
    fnc(cpufreq_gov_userspace_init,deferred) /**/ \
    \
    fnc(hpet_init,deferred) /**/ \
    \
    fnc(pci_hotplug_init,deferred,pcie_portdrv_init) /**/ \
    \
    fnc(pcied_init,deferred,pci_hotplug_init) /**/ \
    \
    fnc(shpcd_init,deferred,pcied_init) /**/ \
    \
    fnc(twofish_generic,deferred)  /* twofish_generic.ko */ \
    \
    fnc(twofish_i586,deferred)  /* twofish-i586.ko */ \
    \
    fnc(asymmetric_key_init,deferred) /**/ \
    \
    fnc(pkcs7_key_init,deferred) /**/ \
    \
    fnc(x509_key_init,deferred) /**/ \
    \
    fnc(aes_init,deferred) /**/ \
    \
    fnc(vmac_module_init,deferred) /**/ \
    \
    fnc(mousedev_init,deferred) /**/ \
    \
    fnc(atkbd_init,deferred) /**/\
    \
    fnc(uinput_init,deferred) /**/\
    \
    fnc(psmouse_init,deferred) /**/\
    \
    fnc(serport_init,deferred) /**/\
    \
    fnc(b43_init,deferred) /**/\
    \
    fnc(vb2_thread_init,deferred) /**/\
    \
    fnc(b43legacy_init,deferred) /**/\
    \
    fnc(azx_driver_init,deferred) /**/\
    \
    fnc(crypto_algapi_init,deferred)  /**/ \
    \
    fnc(chainiv_module_init,deferred)  /**/ \
    \
    fnc(pcie_pme_service_init,deferred)  /**/ \
    \
    fnc(seqiv_module_init,deferred)  /**/ \
	\
	fnc(eseqiv_module_init,deferred)  /**/ \
	\
	fnc(crypto_cmac_module_init,deferred)  /**/ \
	\
	fnc(crypto_pcbc_module_init,deferred)  /**/ \
	\
	fnc(crypto_ctr_module_init,deferred)  /**/ \
	\
	fnc(crypto_gcm_module_init,deferred)  /**/ \
	\
	fnc(hmac_module_init,deferred)  /**/ \
	\
	fnc(crypto_cts_module_init,deferred)  /**/ \
	\
	fnc(crypto_ccm_module_init,deferred)  /**/ \
	\
	fnc(des_generic_mod_init,deferred)  /**/ \
	\
	fnc(fcrypt_mod_init,deferred)  /**/ \
	\
	fnc(serpent_mod_init,deferred)  /**/ \
	\
	fnc(camellia_init,deferred)  /**/ \
	\
	fnc(arc4_init,deferred)  /**/ \
	\
	fnc(khazad_mod_init,deferred)  /**/ \
	\
	fnc(seed_init,deferred)  /**/ \
	\
	fnc(anubis_mod_init,deferred)  /**/ \
	\
	fnc(salsa20_generic_mod_init,deferred)  /**/ \
	\
	fnc(krng_mod_init,deferred)  /**/ \
	\
	fnc(michael_mic_init,deferred)  /**/ \
	\
	fnc(ghash_mod_init,deferred)  /**/ \
	\
	fnc(async_pq_init,deferred)  /**/ \
	\
	fnc(deflate_mod_init,deferred)  /**/ \
	\
	fnc(brd_init,deferred) /**/ \
	\
	fnc(loop_init,deferred) /**/ \
	\
	fnc(tcp_congestion_default,deferred) /**/ \
	\
	fnc(i2c_hid_driver_init,deferred) /**/ \
	\
	fnc(smbalert_driver_init,deferred) /**/ \
	\
	fnc(pca9541_driver_init,deferred) /**/ \
	\
	fnc(pca954x_driver_init,deferred) /**/ \
	\
	fnc(pca955x_driver_init,deferred) /**/ \
	\
	fnc(ir_kbd_driver_init,deferred) /**/ \
    \
    fnc(i8042_init,deferred) /**/ \
    \
    /* ALL pci devices to be analize and add */ \
    fnc(pch_dma_driver_init,asynchronized)  /* drivers/dma/pch_dma.c */ \
    \
    fnc(ismt_driver_init,asynchronized)  /* drivers/i2c/busses/i2c-ismt.c */  \
    \
    fnc(lpc_sch_driver_init,asynchronized)  /* /drivers/mfd/lpc_sch.c */\
    \
    fnc(lpc_ich_driver_init,asynchronized)  /* drivers/mfd/lpc_ich.c  chipset */\
    \
    fnc(serial_pci_driver_init,deferred)  /* drivers/tty/serial/8250/8250_pci.c  chipset */ \
    \
    fnc(spi_gpio_driver_init,deferred) \
    \
    fnc(serial_pci_driver)




#if 0


fnc(,deferred),  /**/ \
fnc(,deferred),  /**/ \
fnc(),  /**/ \
fnc(),  /**/ \
fnc(),  /**/ \
fnc(),  /**/ \

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

#define MOD_DEPENDENCY_ITEM(child,parent)	{ child ## _id, parent ## _id }

#define ADD_MODULE_DEPENDENCY(child,parent) \
         static struct dependency_t child##parent __used \
		 __attribute__((__section__(".async_modules_depends.init"))) = \
		 {child ## _id, parent ## _id};    


#define TASK_STRING(id)                   #id
#define TASK_NAME(id,...)                 TASK_STRING(id)
#define TASK_ID(id,...)                   id ## _id,
#define TASK_TYPE(id,type,...)            type
#define TASK_DEPENDS(id,type,...)         id,##__VA_ARGS__
#define ASYNC_MODULE_INFO(id,type,...)    {type},

/**
 * Implement static enum to string map
 */
typedef enum   {
    none = 0,           // not found or not element
    INIT_CALLS(TASK_ID)
    module_last
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
typedef enum { disable,asynchronized, deferred } task_type_t;

/**
 * Module static information.
 *
 */
struct async_module_info_t
{
    task_type_t type_;
};

struct init_fn_t
{
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
  __attribute__((__section__(".async_initcall.init"))) = {fnc ## _id,fnc};

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

//
#define async_module_pci_driver(__pci_driver) async_module_driver(__pci_driver, pci_register_driver,pci_unregister_driver);


#define async_module_platform_driver(__platform_driver) \
    async_module_driver(__platform_driver, platform_driver_register,platform_driver_unregister);

#else

#define async_module_init(fnc)      module_init(fnc);

#define async_module_driver(__driver, __register, __unregister)    module_driver(__driver, __register, __unregister);

#define async_module_pci_driver(__pci_driver)    module_pci_driver(__pci_driver);

#define async_module_platform_driver(__platform_driver) module_driver(__platform_driver, platform_driver_register,platform_driver_unregister);


#endif


#endif /* ASYNC_MINIT_H_ */
