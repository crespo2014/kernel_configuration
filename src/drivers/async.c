/*
 * dependency.c
 *
 * It implements a task scheduler with dependencies.
 * Every task can depends only from another one, not multiple dependencies is allowed
 *
 * A task has this members.
 * status - block by dependency, waiting to run
 * depends - the task that depends on
 *
 * When a task is complete all dependencies are unblocked, one of them is execute in the same thread
 * if there is no more dependencies to unblock then a waiting one is pick from the list.
 *
 * every running task is move to the end of the list
 * when list size reach zero then the running thread finish.
 *
 * if there is no any task to do then the thread must wait in a waiting queue until waiting task !=0
 * ones all task are done ( list size ==0) then waiting task get the value -1 to wakeup all waiting threads
 * and release all thread because list is empty.
 * spin lock have to be use to access the list
 *
 * any module which its dependency is not found on the list will take the highest
 *
 * Picking a new task.
 * Wake up all waiting task.
 * If number of waked up task != 1 the decrement and waiting += waked up
 * if (waiting) wake_up queue
 * if list empty return null
 * if waiting == 0 then pick one, waiting = len, signal queue
 *
 *	Main thread will start others threads and pick task from the list.
 *	if all task are waiting for end_idx then wait for all threads
 *
 *	List holding task
 *
 *  |----------|---------|----------|------------|
 *    waiting     ready    running      done     |end
 *
 *  if waiting == running then all task became ready
 *
 *  two tables are in use
 *  1 . task data
 *  2 . task index -
 *
 *  Created on: 16 Apr 2015
 *      Author: lester.crespo
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
#include <linux/debugfs.h>
#include <linux/mm.h>  // mmap related stuff#include <linux/slab.h>#include <linux/types.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/types.h>
#include <asm/atomic.h>
#include <linux/delay.h>
#include <linux/kthread.h>  // for threads

/**
 * Full list of module init functions call
 */
#define INIT_CALLS(fnc) \
    fnc(ahci_pci_driver_init,asynchronized)  /* ahci.ko */ \
    \
    fnc(ahci_driver_init,asynchronized,ahci_pci_driver_init) /**/ \
    \
    fnc(fbmem_init,asynchronized)   /* drivers/video/fbdev/core/fbmem.c */ \
    \
    fnc(mda_console_init,asynchronized,fbmem_init)   /* drivers/video/console/mdacon.c */ \
    \
    fnc(newport_console_init,asynchronized,fbmem_init)   /* drivers/video/console/newport_con.c  */ \
    \
    fnc(sticonsole_init,asynchronized,fbmem_init)   /* drivers/video/console/sticon.c  */ \
    \
    fnc(acpi_ac_init,asynchronized) /**/ \
    \
    fnc(agp_nvidia_init,asynchronized,agp_init)  /* nvidia-agp.ko */ \
    \
    fnc(nvidia_frontend_init_module,asynchronized,drm_core_init)  /* nvidia.ko */ \
    \
    fnc(uvm_init,asynchronized,nvidia_frontend_init_module)  /* nvidia-uvm.ko */ \
    \
    fnc(ioat_init_module,asynchronized)  /* drivers/dma/ioat/pci.c ioatdma.ko */ \
    \
    fnc(acpi_video_init,asynchronized)  /* drivers/acpi/video.ko */ \
    \
    fnc(agp_init,asynchronized,ioat_init_module,acpi_video_init)  /* agpgart.ko */ \
    \
    fnc(drm_core_init,asynchronized,agp_init)  /* drm.ko */ \
    \
    fnc(pch_dma_driver_init,asynchronized)  /* drivers/dma/pch_dma.c */ \
    \
    fnc(ismt_driver_init,asynchronized)  /* drivers/i2c/busses/i2c-ismt.c */  \
    \
    fnc(lpc_sch_driver_init,asynchronized)  /* /drivers/mfd/lpc_sch.c */\
    \
    fnc(lpc_ich_driver_init,asynchronized)  /* drivers/mfd/lpc_ich.c  chipset */\
    \
    fnc(serial8250_init,deferred) /* drivers/tty/serial/8250/8250_core.c */ \
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
    fnc(uhid_init,deferred,ohci_platform_init)       /* uhid.ko */ \
    \
    fnc(hid_init,deferred,ohci_platform_init)       /**/ \
    \
    fnc(hid_generic_init,deferred,hid_init,uhid_init) /**/ \
    \
    fnc(hid_generic,deferred,hid_init,uhid_init) /**/ \
    \
    fnc(cherry_driver_init,deferred,hid_init,uhid_init)  /**/ \
    \
    fnc(chicony_driver_init,deferred,hid_init,uhid_init) /**/ \
    \
    fnc(usb_hid_init,deferred)  /* usbhid.ko */ \
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
    fnc(serial_pci_driver_init,deferred)  /* drivers/tty/serial/8250/8250_pci.c  chipset */ \
    \
    fnc(spi_gpio_driver_init,deferred) \
    \
    /* Full module list */ \
    fnc(init_per_zone_wmark_min,asynchronized)  /* mm/page_alloc.c */ \
    \
    fnc(init,asynchronized)  /**/ \
    \
    fnc(proc_execdomains_init,asynchronized)  /**/ \
    \
    fnc(kswapd_init,asynchronized)  /**/ \
    \
    fnc(proc_modules_init,asynchronized)  /**/ \
    \
    fnc(fcntl_init,asynchronized)  /**/ \
    \
    fnc(acpi_fan_driver_init,asynchronized)  /**/ \
    \
    fnc(cn_proc_init,asynchronized)  /* connector/cn_proc.c */ \
    \
    fnc(nvram_init,asynchronized,pty_init)  /* drivers/char/nvram.c */ \
    \
    fnc(mod_init,asynchronized,pty_init)  /* /drivers/char/hw_random/intel-rng.c */ \
    \
    fnc(coretemp_init,asynchronized)  /* drivers/hwmon/coretemp.c  */ \
    \
    fnc(gpio_fan_driver_init,asynchronized)  /*  drivers/hwmon/gpio-fan.c  */ \
    \
    fnc(i2c_dev_init,asynchronized)  /* /drivers/i2c/i2c-dev.c */ \
    \
    fnc(i2c_i801_init,asynchronized)  /* /i2c/busses/i2c-i801.c */ \
    \
    fnc(smbus_sch_driver_init,asynchronized)  /*  drivers/i2c/busses/i2c-isch.c  */ \
    \
    fnc(i2c_mux_gpio_driver_init ,asynchronized)  /* drivers/i2c/muxes/i2c-mux-gpio.c */ \
    \
    fnc(intel_idle_init,asynchronized)  /* /drivers/idle/intel_idle.c */ \
    \
    fnc(simtec_i2c_driver_init,asynchronized)  /* drivers/i2c/busses/i2c-simtec.c  */ \
    \
    fnc(evdev_init,asynchronized)  /*  drivers/input/evdev.c  */ \
    \
    fnc(gspca_init,deferred)  /*  /drivers/media/usb/gspca/gspca.c */ \
    \
    fnc(uvc_init,deferred)  /*  drivers/media/usb/uvc/uvc_driver.c   */ \
    \
    fnc(ptp_pch_init,asynchronized)  /* drivers/ptp/ptp_pch.c  */ \
    \
    fnc(phy_module_init,asynchronized)  /* /include/linux/phy.h   */ \
    \
    fnc(cmos_init,asynchronized)  /* rtc/rtc-cmos.c   */ \
    \
    fnc(init_sd,asynchronized)  /* /drivers/scsi/sd.c   */ \
    \
    fnc(pty_init,asynchronized)  /*  drivers/tty/pty.c  */ \
    \
    fnc(init_sg,asynchronized)  /* /drivers/scsi/sg.c   */ \
    \
    fnc(sbf_init,asynchronized)  /*  arch/x86/kernel/bootflag.c  */ \
    \
    fnc(setup_vmstat,asynchronized)  /* /mm/vmstat.c  */ \
    \
    fnc(extfrag_debug_init,asynchronized)  /* mm/vmstat.c  */ \
    \
    fnc(proc_filesystems_init,asynchronized)  /* fs/filesystems.c */ \
    \
    fnc(dio_init,asynchronized)  /*  fs/direct-io.c  */ \
    \
    fnc(init_autofs4_fs,deferred)  /*  /fs/autofs4/init.c  */ \
    \
    fnc(configfs_init,asynchronized)  /* fs/configfs/mount.c  */ \
    \
    fnc(init_devpts_fs,asynchronized)  /* fs/devpts/inode.c  */ \
    \
    fnc(ext4_init_fs,asynchronized,init_hugetlbfs_fs)  /* fs/ext4/super.c  */ \
    \
    fnc(init_ext2_fs,deferred)  /* fs/ext2/super.c  */ \
    \
    fnc(init_hugetlbfs_fs,asynchronized)  /* /fs/hugetlbfs/inode.c  */ \
    \
    fnc(init_nls_cp437,asynchronized)  /* fs/nls/nls_cp437.c  */ \
    \
    fnc(init_nls_cp850,asynchronized)  /* fs/nls/nls_cp850.c */ \
    \
    fnc(init_nls_cp852,asynchronized)  /* fs/nls/nls_cp852.c   */ \
    \
    fnc(dnotify_init,asynchronized)  /* /fs/notify/dnotify/dnotify.c  */ \
    \
    fnc(init_nls_ascii,asynchronized)  /* fs/nls/nls_ascii.c  */ \
    \
    fnc(init_nls_iso8859_1,asynchronized)  /*  fs/nls/nls_iso8859-1.c  */ \
    \
    fnc(init_nls_utf8,asynchronized)  /* fs/nls/nls_utf8.c */ \
    \
    fnc(inotify_user_setup,asynchronized)  /*  fs/notify/inotify/inotify_user.c  */ \
    \
    fnc(proc_locks_init,asynchronized)  /* fs/locks.c  */ \
    \
    fnc(init_udf_fs,deferred)  /* fs/udf/super.c */ \
    \
    fnc(init_mbcache,asynchronized)  /* /fs/mbcache.c  */ \
    \
    fnc(proc_genhd_init,asynchronized)  /* block/genhd.c   */ \
    \
    fnc(noop_init,asynchronized)  /*  /block/noop-iosched.c  */ \
    \
    fnc(deadline_init,asynchronized)  /* /block/deadline-iosched.c */ \
    \
    fnc(cfq_init,asynchronized)  /* block/cfq-iosched.c  */ \
    \
    fnc(init_dns_resolver,asynchronized)  /* net/dns_resolver/dns_key.c  */ \
    \
    fnc(sock_diag_init ,asynchronized)  /* /net/core/sock_diag.c  */ \
    \
    fnc(cubictcp_register,asynchronized)  /* net/ipv4/tcp_cubic.c   */ \
    \
    fnc(packet_init,asynchronized)  /* net/packet/af_packet.c   */ \
    \
    fnc(slab_proc_init,asynchronized)  /* /mm/slab_common.c  */ \
    \
    fnc(workingset_init,asynchronized)  /* mm/workingset.c  */ \
    \
    fnc(hugetlb_init,asynchronized)  /* mm/hugetlb.c  */ \
    \
    fnc(proc_vmalloc_init,asynchronized)  /* /mm/vmalloc.c  */ \
    \
    fnc(ikconfig_init,asynchronized)  /* kernel/configs.c */ \
    \
    fnc(pmc_atom_init,asynchronized)  /* /arch/x86/kernel/pmc_atom.c  */ \
    \
    fnc(crc_t10dif_mod_init,asynchronized)  /* lib/crc-t10dif.c   */ \
    \
    fnc(libcrc32c_mod_init,asynchronized)  /* lib/libcrc32c.c  */ \
    \
    fnc(percpu_counter_startup,asynchronized)  /*  lib/percpu_counter.c  */ \
    \
    fnc(alsa_sound_last_init,disable) /* sound/core/last.c */



#if 0

fnc(,deferred),  /**/ \
fnc(,deferred),  /**/ \


fnc(,asynchronized)  /**/ \

fnc(,asynchronized)  /**/ \
fnc(,asynchronized)  /**/ \

fnc(,asynchronized)  /**/ \
fnc(,asynchronized)  /**/ \
fnc(,asynchronized)  /**/ \
fnc(,asynchronized)  /**/ \
fnc(,asynchronized)  /**/ \
fnc(,asynchronized)  /**/ \
fnc(,asynchronized)  /**/ \
fnc(,asynchronized)  /**/ \
fnc(,asynchronized)  /**/ \
fnc(,asynchronized)  /**/ \


        /* ARCH  SUBSYS POSTCORE */
        fnc(dca),  /* drivers/dca/dca.ko */  \
        fnc(backlight),  /* drivers/video/backlight/backlight.ko */ \
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

/**
 * Static struct holding all data
 */
extern struct init_fn_t __async_initcall_start[], __async_initcall_end[];
//extern struct dependency_t __async_modules_depends_start[], __async_modules_depends_end[];

#ifdef CONFIG_ASYNCHRO_MODULE_INIT_DEBUG
#define printk_debug(...) printk(KERN_ERR  __VA_ARGS__)
#else
#define printk_debug(...) do {} while(0)
#endif

/**
 * Macros to build static data about modules from arguments.
 * until 10 dependencies available
 * (module_id,module_type,dependencies ... )
 */

#define DEPENDS_0()
#define DEPENDS_1(a)
#define DEPENDS_2(a,b)      MOD_DEPENDENCY_ITEM(a,b),
#define DEPENDS_3(a,b,...)  DEPENDS_2(a,b) DEPENDS_2(a,__VA_ARGS__)
#define DEPENDS_4(a,b,...)  DEPENDS_2(a,b) DEPENDS_3(a,__VA_ARGS__)
#define DEPENDS_5(a,b,...)  DEPENDS_2(a,b) DEPENDS_4(a,__VA_ARGS__)
#define DEPENDS_6(a,b,...)  DEPENDS_2(a,b) DEPENDS_5(a,__VA_ARGS__)
#define DEPENDS_7(a,b,...)  DEPENDS_2(a,b) DEPENDS_6(a,__VA_ARGS__)
#define DEPENDS_8(a,b,...)  DEPENDS_2(a,b) DEPENDS_7(a,__VA_ARGS__)
#define DEPENDS_9(a,b,...)  DEPENDS_2(a,b) DEPENDS_8(a,__VA_ARGS__)
#define DEPENDS_10(a,b,...) DEPENDS_2(a,b) DEPENDS_9(a,__VA_ARGS__)

#define GET_10(fnc,n0,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10,...) fnc##n10
#define COUNT(fnc,...) GET_10(fnc,__VA_ARGS__,10,9,8,7,6,5,4,3,2,1)
#define CALL_FNC(fnc,...) COUNT(fnc,__VA_ARGS__)(__VA_ARGS__)

#define DEPENDS_BUILD(id,type,...) CALL_FNC(DEPENDS_,id,##__VA_ARGS__)

const char* getName(modules_e id)
{
#ifdef CONFIG_ASYNCHRO_MODULE_INIT_DEBUG
    static const char* const module_name[] =
    {   "",INIT_CALLS(TASK_NAME)};
    if (id > module_last)
        return "";
    return module_name[id];
#endif
    return "";
}


static const struct async_module_info_t /*__initconst*/ module_info[] =
{   {disable}, INIT_CALLS(ASYNC_MODULE_INFO) {disable}};

/**
 * Dependencies list is declared next
 */
static const struct dependency_t /*__initconst*/ module_depends[] =
{ INIT_CALLS(DEPENDS_BUILD) };


#define MAX_TASKS (unsigned)module_last+2

/**
 * all initcall will be enums. a tbl will store all names
 */
struct task_t
{
    task_type_t type;        //
    modules_e id;           // idx in string table
    initcall_t fnc;         // ptr to init function
    atomic_t waiting_count; // how many task does it depend on
    volatile unsigned long status;        // bit 0 1 free to get, bit 1 1 doing
    unsigned child_count;   // how many task it triggers
    struct task_t**  first_cild; // first child to be release
};

// TODO join together all task with taskid table to allow dynamic allocation
struct task_list_t
{
    struct task_t all[MAX_TASKS];       // full list of task
    struct task_t* task_end;
    task_type_t     type_;
    struct task_t*  current_tasks[MAX_TASKS];        // list of actived task
    struct task_t** task_last_done;      // first task to be done
    struct task_t** task_last;           // one pass last task to be done
    struct task_t*  childs[sizeof(module_depends)/sizeof(*module_depends)];  //  list of child to be relase ordered by parent
};

static struct task_list_t tasks;

struct task_t* getTask(modules_e id)
{
    struct task_t* t = tasks.all;
    while(t != tasks.task_end && t->id != id)
        ++t;
    return (t == tasks.task_end) ? NULL : t;
}

static DEFINE_SPINLOCK(list_lock);
static DECLARE_WAIT_QUEUE_HEAD( list_wait);

/**
 * Check if one task depends on another
 * 0 - false
 * 1 - true
 */
unsigned  doesDepends(modules_e child, modules_e parent)
{
    const struct dependency_t * it_dependency;
    for (it_dependency = module_depends; it_dependency != module_depends + sizeof(module_depends)/sizeof(*module_depends); ++it_dependency)
    {
        if (it_dependency->parent_id == parent && it_dependency->task_id == child)
            return 1;
    }
    return 0;
}
/*
 * Read all information from static mmeory an expand it to dynamic memory
 */
void  FillTasks(struct init_fn_t* begin, struct init_fn_t* end)
{
    const struct dependency_t *it_dependency;
    struct init_fn_t* it_init_fnc;
    struct task_t* it_task;
    struct task_t* ptask;
    tasks.task_end = tasks.all;
    for (it_init_fnc = begin; it_init_fnc < end; ++it_init_fnc, ++tasks.task_end)
    {
        tasks.task_end->id = it_init_fnc->id;
        tasks.task_end->type = module_info[it_init_fnc->id].type_;
        tasks.task_end->fnc = it_init_fnc->fnc;
        atomic_set(&tasks.task_end->waiting_count,0);
    }
    // resolve dependencies
    for (it_task = tasks.all; it_task != tasks.task_end; ++it_task)
    {
        for (it_dependency = module_depends; it_dependency != module_depends + sizeof(module_depends)/sizeof(*module_depends); ++it_dependency)
        {
            if (it_dependency->task_id == it_task->id)
            {
                // Do not register a dependency that does not exist
                ptask = getTask(it_dependency->parent_id);
                if (ptask == NULL)
                {
                    printk(KERN_ERR "async Dependency id %d %s not found for id %d %s\n",it_dependency->parent_id,getName(it_dependency->parent_id),it_dependency->task_id,getName(it_dependency->task_id));
                }
                else
                {
                    // register dependency
                    atomic_inc(&it_task->waiting_count);
                }
            }
            if (it_dependency->parent_id == it_task->id)
            {
                ++it_task->child_count;
            }
        }
        printk_debug("async registered '%pF' depends on %d tasks\n", it_task->fnc,atomic_read(&it_task->waiting_count));
    }
}
/*
 * Prepare task to be done using task pointer list
 */
void  FillTasks2(struct init_fn_t* begin, struct init_fn_t* end)
{
    const struct dependency_t *it_dependency;
    struct init_fn_t* it_init_fnc;
    struct task_t* it_task;
    struct task_t* ptask;
    struct task_t** release_end;            // pointer to list of childs
    tasks.task_end = tasks.all;
    for (it_init_fnc = begin; it_init_fnc < end; ++it_init_fnc, ++tasks.task_end)
    {
        tasks.task_end->id = it_init_fnc->id;
        tasks.task_end->type = module_info[it_init_fnc->id].type_;
        tasks.task_end->fnc = it_init_fnc->fnc;
        atomic_set(&tasks.task_end->waiting_count,0);
        tasks.task_end->first_cild = NULL;
        tasks.task_end->child_count = 0;
        tasks.task_end->status = 3;
    }
    // for every task find all child to be release and store.
    release_end = tasks.childs;
    for (it_task = tasks.all; it_task != tasks.task_end; ++it_task)
    {
        it_task->first_cild = release_end;
        for (it_dependency = module_depends; it_dependency != module_depends + sizeof(module_depends)/sizeof(*module_depends); ++it_dependency)
        {
            // get all childs
            if (it_dependency->parent_id == it_task->id)
            {
                // Do not register a dependency that does not exist
                ptask = getTask(it_dependency->task_id);
                if (ptask == NULL)
                {
                    printk(KERN_ERR "async Child id %d %s not found for parent id %d %s\n",it_dependency->task_id,getName(it_dependency->task_id),it_dependency->parent_id,getName(it_dependency->parent_id));
                }
                else
                {
                    // register dependency
                    ++it_task->child_count;
                    *release_end = ptask;
                    ++release_end;
                    if (it_task->type == deferred && ptask->type == asynchronized)
                    {
                        printk(KERN_ERR "async Child id %d %s will not released by parent id %d %s\n",it_dependency->task_id,getName(it_dependency->task_id),it_dependency->parent_id,getName(it_dependency->parent_id));
                    }
                    if (it_task > ptask)
                    {
                        printk(KERN_ERR "async child id %d %s was registered before parent id %d %s\n",it_dependency->task_id,getName(it_dependency->task_id),it_dependency->parent_id,getName(it_dependency->parent_id));
                    }
                    atomic_inc(&ptask->waiting_count);
                }
            }
        }
        printk_debug("async registered '%pF' %s release %d tasks\n", it_task->fnc,getName(it_task->id),it_task->child_count);
    }
}
/*
 * Prepare pointers to task do not use index
 */
void  Prepare2(task_type_t type)
{
    // Pick only task of type from all task
    struct task_t* it_task;
    //
    printk_debug("async Preparing ... \n");

    tasks.task_last = tasks.current_tasks;
    tasks.task_last_done = tasks.current_tasks;
    tasks.type_ = type;

    for (it_task = tasks.all; it_task != tasks.task_end; ++it_task)
    {
        if (it_task->type == type)
        {
            *tasks.task_last  = it_task;
            ++tasks.task_last;
        }
        else
            if (it_task->type == disable)
            {
                printk(KERN_ERR "async task id %d %s is disable \n",it_task->id,getName(it_task->id));
            }
    }
    printk_debug("async %d tasks \n",tasks.task_last - tasks.current_tasks);
}
/**
 * Mark task as done
 */
void  TaskDone2(struct task_t* ptask)
{
    struct task_t** it_task;
    unsigned i;
    if (ptask != NULL)
    {
       clear_bit(1,&ptask->status);      // task done
       // release all childs
       it_task = ptask->first_cild;
       for(i = 0;i<ptask->child_count;++i,++it_task)
       {
           atomic_dec(&(*it_task)->waiting_count);
       }
       if (ptask->child_count > 1)
           wake_up_interruptible(&list_wait);
    }
    // try to update last done task
    spin_lock(&list_lock);
    while (tasks.task_last_done != tasks.task_last && (*tasks.task_last_done)->status == 0)
        ++tasks.task_last_done;
    spin_unlock(&list_lock);

    // Check for end condition
    if (tasks.task_last_done == tasks.task_last)
        wake_up_interruptible_all(&list_wait);
}

/*
 * Try to get a task ready to run
 */
struct task_t*  PeekTask(void)
{
    struct task_t** it_task;
    for (it_task = tasks.task_last_done;it_task != tasks.task_last;++it_task)
    {
        if (atomic_read(&(*it_task)->waiting_count) == 0)
        {
            if (test_and_clear_bit(0,&(*it_task)->status) == 1) return *it_task;
        }
    }
    return NULL;
}

/**
 * Thread for version 2
 */

int  ProcessThread2(void *data)
{
    int ret;
    struct task_t* ptask = NULL;
    printk_debug("async %d starts\n", (unsigned )data);
    do
    {
        ret = wait_event_interruptible(list_wait, (ptask = PeekTask()) != NULL || tasks.task_last_done == tasks.task_last);
        if (ret != 0)
        {
            printk("async init wake up returned %d\n", ret);
            break;
        }
        if (ptask != NULL)
        {
            printk_debug("async %d %pF %s\n", (unsigned)data, ptask->fnc,getName(ptask->id));
            do_one_initcall(ptask->fnc);
            TaskDone2(ptask);
        }
    } while (ptask != 0);
    printk_debug("async %d ends\n", (unsigned)data);
    return 0;
}

/**
 * Execute all initialization for an specific type
 * We need wait for everything done as a barrier to avoid problems
 */
int  start_threads(task_type_t type,int(* thread_fnc) (void*) )
{
    unsigned max_cpus = num_online_cpus();
    unsigned it;
    static struct task_struct *thr;
    if (max_cpus > CONFIG_ASYNCHRO_MODULE_INIT_THREADS)
    {
        max_cpus = CONFIG_ASYNCHRO_MODULE_INIT_THREADS;
    }
    // validated cpu count
    if (max_cpus == 0)
        max_cpus = 1;

    // leave one thread free
    if (type == deferred &&  max_cpus > 1)
    {
        //--max_cpus;
    }
    printk_debug("async using %d cpus\n", max_cpus);
    for (it=0; it < max_cpus;  ++it)
    {
        //start working threads
        thr = kthread_create(thread_fnc, (void* )(it), "async%d",it);
        if (thr != ERR_PTR(-ENOMEM))
        {
            kthread_bind(thr, it);
            wake_up_process(thr);
        }
        else
        {
            printk("Async module initialization thread failed .. fall back to normal mode");
            thread_fnc(NULL);
        }
    }
    return 0;
}

/**
 * Second initialization USB devices, some PCI
 *
 *     int test_and_clear_bit(int nr, void *addr)
 *
 */
static atomic_t deferred_initcalls_done = ATOMIC_INIT(0);
static int  deferred_initialization(void)
{
    int old = atomic_xchg(&deferred_initcalls_done,1);
    if (old == 0)
    {
        printk_debug("async started deferred\n");
        wait_event_interruptible(list_wait, (tasks.task_last_done == tasks.task_last));
        Prepare2(deferred);
        start_threads(deferred,ProcessThread2);
    }
    return 0;
}

static ssize_t deferred_initcalls_read_proc(struct file *file, char __user *buf,size_t nbytes, loff_t *ppos)
{
   int len, ret;
   char tmp[3] = "1\n";

   if (*ppos >= 3)
       return 0;

   // if offset != 0
   if (!(*ppos)) {
       tmp[0] = '0';
       deferred_initialization();
       // wait for completion, any process that depends on driver can wait for this to complete
       wait_event_interruptible(list_wait, (tasks.task_last_done == tasks.task_last));
   }
   len = min(nbytes, (size_t)3);
   ret = copy_to_user(buf, tmp, len);
   if (ret)
       return -EFAULT;
   *ppos += len;
   return len;
}

static const struct file_operations deferred_initcalls_fops = {
   .read           = deferred_initcalls_read_proc,
};

/**
 * Module initialization is taking a long time, more than any other.
 * Module initialization and fist execution is going to be do from thread
 */

int async_init_thread(void* d)
{
    FillTasks2(__async_initcall_start, __async_initcall_end);
    Prepare2(asynchronized);
    start_threads(asynchronized,ProcessThread2);
    return 0;
}

/**
 * First initialization of module. Disk diver and AGP  
 */
static int  async_initialization(void)
{
    struct task_struct *thr;
    printk_debug("async started asynchronized\n");
    thr = kthread_create(async_init_thread, (void* )(0), "async_init_thread");
    wake_up_process(thr);

//    FillTasks2(__async_initcall_start, __async_initcall_end);
//    Prepare2(asynchronized);
//    start_threads(asynchronized,ProcessThread2);

    return 0;
}

/**
 * Register proc entry to allow deferred initialization
 */
static int async_late_init(void)
{
    proc_create("deferred_initcalls", 0, NULL, &deferred_initcalls_fops);
    // do we need to wait until async finish before allow user space start
    //deferred_initialization();
    return 0;
}

__initcall(async_initialization);
late_initcall_sync(async_late_init);		// Second stage, last to do before jump to high level initialization


