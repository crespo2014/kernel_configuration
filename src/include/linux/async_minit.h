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
#define MODULES_ID(fnc) \
    fnc(ahci_pci_driver_init) \
    \
    fnc(ahci_driver_init) /**/ \
    \
    fnc(fbmem_init) \
    \
    fnc(mda_console_init)   /* drivers/video/console/mdacon.c */ \
    \
    fnc(newport_console_init)   /* drivers/video/console/newport_con.c  */ \
    \
    fnc(sticonsole_init)   /* drivers/video/console/sticon.c  */ \
    \
    fnc(acpi_ac_init) /**/ \
    \
    fnc(agp_nvidia_init)  /* nvidia-agp.ko */ \
    \
    fnc(nvidia_frontend_init_module)  /* nvidia.ko */ \
    \
    fnc(uvm_init)  /* nvidia-uvm.ko */ \
    \
    fnc(ioat_init_module)  /* drivers/dma/ioat/pci.c ioatdma.ko */ \
    \
    fnc(acpi_video_init)  /* drivers/acpi/video.ko */ \
    \
    fnc(agp_init)  /* agpgart.ko */ \
    \
    fnc(drm_core_init)  /* drm.ko */ \
    \
    fnc(pch_dma_driver_init)  /* drivers/dma/pch_dma.c */ \
    \
    fnc(ismt_driver_init)  /* drivers/i2c/busses/i2c-ismt.c */  \
    \
    fnc(lpc_sch_driver_init)  /* /drivers/mfd/lpc_sch.c */\
    \
    fnc(lpc_ich_driver_init)  /* drivers/mfd/lpc_ich.c  chipset */\
    \
    fnc(serial8250_init) /* */ \
    \
    fnc(nforce2_driver_init) /* */ \
    \
    fnc(crypto_xcbc_module_init) /* */ \
    \
    fnc(forcedeth_pci_driver_init) /* */ \
    \
    fnc(init_cifs)               /* */  \
    \
    fnc(acpi_pcc_driver) \
    \
    fnc(acpi_hed_driver)               /* */  \
    \
    fnc(acpi_smb_hc_driver)               /* */  \
    \
    fnc(crb_acpi_driver)               /* */  \
    \
    fnc(acpi_smbus_cmi_driver)               /* */  \
    \
    fnc(atlas_acpi_driver)               /* */  \
    \
    fnc(smo8800_driver)               /* */  \
    \
    fnc(lis3lv02d_driver)               /* */  \
    \
    fnc(irst_driver)               /* */  \
    \
    fnc(smartconnect_driver)               /* */  \
    \
    fnc(pvpanic_driver)               /* */  \
    \
    fnc(acpi_topstar_driver)               /* */  \
    \
    fnc(toshiba_bt_rfkill_driver)               /* */  \
    \
    fnc(toshiba_haps_driver)               /* */  \
    \
    fnc(xo15_ebook_driver)               /* */  \
    \
    fnc(drm_fb_helper_modinit)   /* */  \
    \
    fnc(acpi_button_driver_init) /* */  \
    \
    fnc(acpi_power_meter_init) /* */  \
    \
    fnc(synusb_driver_init)    /*   */ \
    \
    fnc(usblp_driver_init)    /*   */ \
    \
    fnc(rfcomm_init)            /* rfcomm.ko */ \
    \
    fnc(alsa_timer_init)        /* snd-timer.ko */ \
    \
    fnc(alsa_hwdep_init)        /* snd-hwdep.ko */ \
    \
    fnc(alsa_seq_device_init)   /* snd-seq-device.ko */ \
    \
    fnc(alsa_mixer_oss_init)    /* snd-mixer-oss.ko */ \
    \
    fnc(snd_hrtimer_init)   /* snd-hrtimer.ko */ \
    \
    fnc(alsa_pcm_init)          /* snd-pcm.ko */ \
    \
    fnc(alsa_seq_init)          /* snd-seq.ko */ \
    \
    fnc(alsa_pcm_oss_init)      /* snd-pcm-oss.ko */ \
    \
    fnc(alsa_seq_midi_event_init)     /* snd-seq-midi-event.ko */ \
    \
    fnc(alsa_seq_dummy_init)    /* snd-seq-dummy.ko */ \
    \
    fnc(snd_hda_controller)     /* snd-hda-controller.ko */ \
    \
    fnc(patch_si3054_init)      /* snd-hda-codec-si3054.ko */ \
    \
    fnc(patch_ca0132_init)      /* snd-hda-codec-ca0132.ko */ \
    \
    fnc(patch_hdmi_init)        /* snd-hda-codec-hdmi.ko */ \
    \
    fnc(alsa_seq_oss_init)      /* snd-seq-oss.ko */ \
    \
    fnc(snd_hda_intel)          /* snd-hda-intel.ko */ \
    \
    fnc(patch_sigmatel_init)   /* snd-hda-codec-idt.ko */ \
    \
    fnc(patch_cirrus_init)      /* snd-hda-codec-cirrus.ko */ \
    \
    fnc(patch_ca0110_init)      /* snd-hda-codec-ca0110.ko */ \
    \
    fnc(patch_via_init)         /* snd-hda-codec-via.ko */ \
    \
    fnc(patch_realtek_init)     /* snd-hda-codec-realtek.ko */ \
    \
    fnc(patch_conexant_init)    /* snd-hda-codec-conexant.ko */ \
    \
    fnc(patch_cmedia_init)      /* snd-hda-codec-cmedia.ko */ \
    \
    fnc(patch_analog_init)      /* snd-hda-codec-analog.ko */ \
    \
    fnc(coretemp)  /* coretemp.ko */ \
    \
    fnc(gpio_fan)  /* gpio-fan.ko */ \
    \
    fnc(acpi_processor_driver_init)  /* acpi-power-meter.ko */ \
    \
    fnc(init_mtd)  /* mtd.ko */ \
    \
    fnc(ubi_init)  /* ubi.ko */ \
    \
    fnc(uio_init)  /* uio.ko */ \
    \
    fnc(hilscher_pci_driver_init)  /* uio_cif.ko */ \
    \
    fnc(mxm_wmi_init)  /* mxm-wmi.ko */ \
    \
    fnc(speedstep_init)  /* speedstep-ich.ko */ \
    \
    fnc(mmc_blk_init)  /* mmc_block.ko */ \
    \
    fnc(uvcvideo)  /* uvcvideo.ko */ \
    \
    fnc(gspca_main)  /* gspca_main.ko */ \
    \
    fnc(ir_kbd_driver)  /* ir-kbd-i2c.ko module_i2c_driver */ \
    \
    fnc(i2c_mux_gpio_driver)  /* i2c-mux-gpio.ko module_platform_driver */ \
    \
    fnc(pca9541_driver)  /* i2c-mux-pca9541.ko module_i2c_driver */ \
    \
    fnc(pca954x_driver)  /* i2c-mux-pca954x.ko module_i2c_driver */ \
    \
    fnc(uhci_hcd_init)    /* uhci-hcd.ko */ \
    \
    fnc(usbmon)     /* usbmon.ko */ \
    \
    fnc(usb_storage_driver_init)     /* usb-storage.ko */ \
    \
    fnc(led_driver_init)     /* usbled.ko */ \
    \
    fnc(uhid_init)       /* uhid.ko */ \
    \
    fnc(hid_init)       /**/ \
    \
    fnc(hid_generic_init) /**/ \
    \
    fnc(hid_generic) /**/ \
    \
    fnc(cherry_driver_init)  /**/ \
    \
    fnc(chicony_driver_init) /**/ \
    \
    fnc(usb_hid_init)  /* usbhid.ko */ \
    \
    fnc(apple_driver_init) /**/ \
    \
    fnc(a4_driver_init) /**/ \
    \
    fnc(ez_driver_init) /**/ \
    \
    fnc(cp_driver_init) /**/ \
    \
    fnc(ks_driver_init) /**/ \
    \
    fnc(ms_driver_init) /**/ \
    \
    fnc(lg_driver_init) /**/ \
    \
    fnc(mr_driver_init) /**/ \
    \
    fnc(belkin_driver_init) /**/ \
    \
    fnc(plantronics_driver_init) /**/ \
    \
    fnc(keytouch_driver_init) /**/ \
    \
    fnc(ene_ub6250_driver_init)  /* ums-eneub6250.ko */ \
    \
    fnc(uas_driver_init)  /* uas.ko */ \
    \
    fnc(realtek_cr_driver_init)  /* ums-realtek.ko */ \
    \
    fnc(ehci_pci_init)  /* ehci-pci.ko */ \
    \
    fnc(libphy)  /* libphy.ko */ \
    \
    fnc(smsc)  /* smsc.ko */ \
    \
    fnc(lib80211_init)  /* lib80211.ko */ \
    \
    fnc(lib80211_crypto_tkip_init)  /* lib80211_crypt_tkip.ko */ \
    \
    fnc(lib80211_crypto_wep_init)  /* lib80211_crypt_wep.ko */ \
    \
    fnc(lib80211_crypto_ccmp_init)  /* lib80211_crypt_ccmp.ko */ \
    \
    fnc(libipw_init)  /* libipw.ko */ \
    \
    fnc(led_class)  /* led-class.ko */ \
    \
    fnc(hwrng_modinit)  /* rng-core.ko */ \
    \
    fnc(prng_mod_init) /* */ \
    \
    fnc(ipw2100_init)  /* ipw2100.ko */ \
    \
    fnc(leds_pca955x)  /* leds-pca955x.ko */ \
    \
    fnc(b43)  /* b43.ko */ \
    \
    fnc(b43legacy)  /* b43legacy.ko */ \
    \
    fnc(intel_rng_mod_init)  /* intel-rng.ko */ \
    \
    fnc(af_alg_init)  /* af_alg.ko */ \
    \
    fnc(algif_hash_init)  /* algif_hash.ko */ \
    \
    fnc(algif_skcipher_init)  /* algif_skicupher.ko */ \
    \
    fnc(alg_hash)  /* alg_hash.ko */ \
    \
    fnc(lzo_mod_init)  /* lzo.ko */ \
    \
    fnc(crypto_authenc_module_init)  /* authenc.ko */ \
    \
    fnc(crypto_authenc_esn_module_init)  /* authencesn.ko */ \
    \
    fnc(cast5_mod_init)  /* cat5_generic.ko */ \
    \
    fnc(cast6_mod_init)  /* cast6_generic.ko */ \
    \
    fnc(blowfish_mod_init)  /* blowfish_generic.ko */ \
    \
    fnc(zlib_mod_init)  /* zlib.ko */ \
    \
    fnc(prgn_mod_init) /**/ \
    \
    fnc(crypto_cbc_module_init) /**/ \
    \
    fnc(crc32_mod_init) /**/ \
    \
    fnc(crc32c_mod_init) /**/ \
    \
    fnc(twofish_mod_init) /**/ \
    \
    fnc(wp512_mod_init) /**/ \
    \
    fnc(crct10dif_mod_init) /**/ \
    \
    fnc(crypto_null_mod_init) /**/ \
    \
    fnc(crypto_ecb_module_init) /**/ \
    \
    fnc(crypto_module_init) /**/ \
    \
    fnc(crypto_user_init) /**/ \
    \
    fnc(lz4_mod_init) /**/ \
    \
    fnc(md4_mod_init) /**/ \
    \
    fnc(md5_mod_init) /**/ \
    \
    fnc(rmd128_mod_init) /**/ \
    \
    fnc(rmd160_mod_init) /**/ \
    \
    fnc(rmd256_mod_init) /**/ \
    \
    fnc(rmd320_mod_init) /**/ \
    \
    fnc(sha1_generic_mod_init) /**/ \
    \
    fnc(sha256_generic_mod_init) /**/ \
    \
    fnc(sha512_generic_mod_init) /**/ \
    \
    fnc(elo_driver_init)  /* usbhid.ko */ \
    \
    fnc(ehci_hcd_init)  /* ehci-hcd.ko */ \
    \
    fnc(ohci_pci_init)  /* ohci-pci.ko */ \
    \
    fnc(ohci_platform_init)  /* ohci-platform.ko */ \
    \
    fnc(ohci_hcd_mod_init)  /* ohci-hcd.ko */ \
    \
    fnc(ehci_platform_init)  /* ehci-platform.ko */ \
    \
    fnc(tcrypt_mod_init) /**/ \
    \
    fnc(tea_mod_init) /**/ \
    \
    fnc(tgr192_mod_init) /**/ \
    \
    fnc(pcie_portdrv_init) /**/ \
    \
    fnc(init_iso9660_fs)  /* isofs.ko */ \
    \
    fnc(fuse_init)  /* fuse.ko */ \
    \
    fnc(cuse_init)  /* cuse.ko */ \
    \
    fnc(journal_init)  /* jbd.ko */ \
    \
    fnc(init_ext3_fs)  /* ext3.ko */ \
    \
    fnc(init_fat_fs)  /* fat.ko */ \
    \
    fnc(init_vfat_fs)  /* vfat.ko */ \
    \
    fnc(init_msdos_fs)  /* msdos.ko */ \
    \
    fnc(init_ntfs_fs) /**/ \
    \
    fnc(acpi_ipmi_init)/**/ \
    \
    fnc(acpi_pad_init)/**/ \
    \
    fnc(acpi_battery_init)/**/ \
    \
    fnc(acpi_sbs_init)/**/ \
    \
    fnc(acpi_thermal_init)/**/ \
    \
    fnc(cpufreq_gov_dbs_init) /**/ \
    \
    fnc(cpufreq_gov_powersave_init) /**/ \
    \
    fnc(cpufreq_stats_init) /**/ \
    \
    fnc(cpufreq_gov_userspace_init) /**/ \
    \
    fnc(hpet_init) /**/ \
    \
    fnc(pci_hotplug_init) /**/ \
    \
    fnc(pcied_init) /**/ \
    \
    fnc(shpcd_init) /**/ \
    \
    fnc(twofish_generic)  /* twofish_generic.ko */ \
    \
    fnc(twofish_i586)  /* twofish-i586.ko */ \
    \
    fnc(asymmetric_key_init) /**/ \
    \
    fnc(pkcs7_key_init) /**/ \
    \
    fnc(x509_key_init) /**/ \
    \
    fnc(aes_init) /**/ \
    \
    fnc(vmac_module_init) /**/ \
    \
    fnc(mousedev_init) /**/ \
    \
    fnc(atkbd_init) /**/\
    \
    fnc(uinput_init) /**/\
    \
    fnc(psmouse_init) /**/\
    \
    fnc(serport_init) /**/\
    \
    fnc(b43_init) /**/\
    \
    fnc(vb2_thread_init) /**/\
    \
    fnc(b43legacy_init) /**/\
    \
    fnc(azx_driver_init) /**/\
    \
    fnc(crypto_algapi_init)  /**/ \
    \
    fnc(chainiv_module_init)  /**/ \
    \
    fnc(pcie_pme_service_init)  /**/ \
    \
    fnc(seqiv_module_init)  /**/ \
    \
    fnc(eseqiv_module_init)  /**/ \
    \
    fnc(crypto_cmac_module_init)  /**/ \
    \
    fnc(crypto_pcbc_module_init)  /**/ \
    \
    fnc(crypto_ctr_module_init)  /**/ \
    \
    fnc(crypto_gcm_module_init)  /**/ \
    \
    fnc(hmac_module_init)  /**/ \
    \
    fnc(crypto_cts_module_init)  /**/ \
    \
    fnc(crypto_ccm_module_init)  /**/ \
    \
    fnc(des_generic_mod_init)  /**/ \
    \
    fnc(fcrypt_mod_init)  /**/ \
    \
    fnc(serpent_mod_init)  /**/ \
    \
    fnc(camellia_init)  /**/ \
    \
    fnc(arc4_init)  /**/ \
    \
    fnc(khazad_mod_init)  /**/ \
    \
    fnc(seed_init)  /**/ \
    \
    fnc(anubis_mod_init)  /**/ \
    \
    fnc(salsa20_generic_mod_init)  /**/ \
    \
    fnc(krng_mod_init)  /**/ \
    \
    fnc(michael_mic_init)  /**/ \
    \
    fnc(ghash_mod_init)  /**/ \
    \
    fnc(async_pq_init)  /**/ \
    \
    fnc(deflate_mod_init)  /**/ \
    \
    fnc(brd_init) /**/ \
    \
    fnc(loop_init) /**/ \
    \
    fnc(tcp_congestion_default) /**/ \
    \
    fnc(i2c_hid_driver_init) /**/ \
    \
    fnc(smbalert_driver_init) /**/ \
    \
    fnc(pca9541_driver_init) /**/ \
    \
    fnc(pca954x_driver_init) /**/ \
    \
    fnc(pca955x_driver_init) /**/ \
    \
    fnc(ir_kbd_driver_init) /**/ \
    \
    fnc(i8042_init) /**/ \
    \
    fnc(serial_pci_driver_init)  /* drivers/tty/serial/8250/8250_pci.c  chipset */ \
    \
    fnc(spi_gpio_driver_init) \
    \
    /* Full module list */ \
    fnc(init_per_zone_wmark_min)  /* mm/page_alloc.c */ \
    \
    fnc(init)  /**/ \
    \
    fnc(proc_execdomains_init)  /**/ \
    \
    fnc(kswapd_init)  /**/ \
    \
    fnc(proc_modules_init)  /**/ \
    \
    fnc(fcntl_init)  /**/ \
    \
    fnc(acpi_fan_driver_init)  /**/ \
    \
    fnc(cn_proc_init)  /* connector/cn_proc.c */ \
    \
    fnc(nvram_init)  /* drivers/char/nvram.c */ \
    \
    fnc(mod_init)  /* /drivers/char/hw_random/intel-rng.c */ \
    \
    fnc(coretemp_init)  /* drivers/hwmon/coretemp.c  */ \
    \
    fnc(gpio_fan_driver_init)  /*  drivers/hwmon/gpio-fan.c  */ \
    \
    fnc(i2c_dev_init)  /* /drivers/i2c/i2c-dev.c */ \
    \
    fnc(i2c_i801_init)  /* /i2c/busses/i2c-i801.c */ \
    \
    fnc(smbus_sch_driver_init)  /*  drivers/i2c/busses/i2c-isch.c  */ \
    \
    fnc(i2c_mux_gpio_driver_init )  /* drivers/i2c/muxes/i2c-mux-gpio.c */ \
    \
    fnc(intel_idle_init)  /* /drivers/idle/intel_idle.c */ \
    \
    fnc(simtec_i2c_driver_init)  /* drivers/i2c/busses/i2c-simtec.c  */ \
    \
    fnc(evdev_init)  /*  drivers/input/evdev.c  */ \
    \
    fnc(gspca_init)  /*  /drivers/media/usb/gspca/gspca.c */ \
    \
    fnc(uvc_init)  /*  drivers/media/usb/uvc/uvc_driver.c   */ \
    \
    fnc(ptp_pch_init)  /* drivers/ptp/ptp_pch.c  */ \
    \
    fnc(phy_module_init)  /* /include/linux/phy.h   */ \
    \
    fnc(cmos_init)  /* rtc/rtc-cmos.c   */ \
    \
    fnc(init_sd)  /* /drivers/scsi/sd.c   */ \
    \
    fnc(pty_init)  /*  drivers/tty/pty.c  */ \
    \
    fnc(init_sg)  /* /drivers/scsi/sg.c   */ \
    \
    fnc(sbf_init)  /*  arch/x86/kernel/bootflag.c  */ \
    \
    fnc(setup_vmstat)  /* /mm/vmstat.c  */ \
    \
    fnc(extfrag_debug_init)  /* mm/vmstat.c  */ \
    \
    fnc(proc_filesystems_init)  /* fs/filesystems.c */ \
    \
    fnc(dio_init)  /*  fs/direct-io.c  */ \
    \
    fnc(init_autofs4_fs)  /*  /fs/autofs4/init.c  */ \
    \
    fnc(configfs_init)  /* fs/configfs/mount.c  */ \
    \
    fnc(init_devpts_fs)  /* fs/devpts/inode.c  */ \
    \
    fnc(ext4_init_fs)  /* fs/ext4/super.c  */ \
    \
    fnc(init_ext2_fs)  /* fs/ext2/super.c  */ \
    \
    fnc(init_hugetlbfs_fs)  /* /fs/hugetlbfs/inode.c  */ \
    \
    fnc(init_nls_cp437)  /* fs/nls/nls_cp437.c  */ \
    \
    fnc(init_nls_cp850)  /* fs/nls/nls_cp850.c */ \
    \
    fnc(init_nls_cp852)  /* fs/nls/nls_cp852.c   */ \
    \
    fnc(dnotify_init)  /* /fs/notify/dnotify/dnotify.c  */ \
    \
    fnc(init_nls_ascii)  /* fs/nls/nls_ascii.c  */ \
    \
    fnc(init_nls_iso8859_1)  /*  fs/nls/nls_iso8859-1.c  */ \
    \
    fnc(init_nls_utf8)  /* fs/nls/nls_utf8.c */ \
    \
    fnc(inotify_user_setup)  /*  fs/notify/inotify/inotify_user.c  */ \
    \
    fnc(proc_locks_init)  /* fs/locks.c  */ \
    \
    fnc(init_udf_fs)  /* fs/udf/super.c */ \
    \
    fnc(init_mbcache)  /* /fs/mbcache.c  */ \
    \
    fnc(proc_genhd_init)  /* block/genhd.c   */ \
    \
    fnc(noop_init)  /*  /block/noop-iosched.c  */ \
    \
    fnc(deadline_init)  /* /block/deadline-iosched.c */ \
    \
    fnc(cfq_init)  /* block/cfq-iosched.c  */ \
    \
    fnc(init_dns_resolver)  /* net/dns_resolver/dns_key.c  */ \
    \
    fnc(sock_diag_init )  /* /net/core/sock_diag.c  */ \
    \
    fnc(cubictcp_register)  /* net/ipv4/tcp_cubic.c   */ \
    \
    fnc(packet_init)  /* net/packet/af_packet.c   */ \
    \
    fnc(slab_proc_init)  /* /mm/slab_common.c  */ \
    \
    fnc(workingset_init)  /* mm/workingset.c  */ \
    \
    fnc(hugetlb_init)  /* mm/hugetlb.c  */ \
    \
    fnc(proc_vmalloc_init)  /* /mm/vmalloc.c  */ \
    \
    fnc(ikconfig_init)  /* kernel/configs.c */ \
    \
    fnc(pmc_atom_init)  /* /arch/x86/kernel/pmc_atom.c  */ \
    \
    fnc(crc_t10dif_mod_init)  /* lib/crc-t10dif.c   */ \
    \
    fnc(libcrc32c_mod_init)  /* lib/libcrc32c.c  */ \
    \
    fnc(percpu_counter_startup)  /*  lib/percpu_counter.c  */ \
    \
    fnc(alsa_sound_last_init) \
    \
    fnc(pcips2_driver_init) /* drivers/input/serio/pcips2.c */ \
    \
    fnc(sermouse_drv_init) \
    \
    fnc(serio_raw_drv_init) \
    \
    fnc(oprofile_init) \
    \
    fnc(add_pcspkr) \
    \
    fnc(acpi_smb_hc_driver_init)



#define MOD_DEPENDENCY_ITEM(child,parent)	{ child ## _id, parent ## _id }

#define TASK_STRING(id)                   #id
#define TASK_NAME(id,...)                 TASK_STRING(id),
#define TASK_ID(id,...)                   id ## _id,
#define TASK_TYPE(id,type,...)            type
#define TASK_DEPENDS(id,type,...)         id,##__VA_ARGS__
#define ASYNC_MODULE_INFO(id,type,...)    {type},

/**
 * Implement static enum to string map
 */
typedef enum   {
    none = 0,           // not found or not element
    MODULES_ID(TASK_ID)
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
