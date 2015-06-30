/*
 * dependency.c
 *
 * V4
 * A group base dependency can be implemented,
 * Module info will be fnc, type, id, group id, dependencies
 * type could be early init, deferred or independent
 * a group id is another id than is done when running counter reach 0, each started module will increment this
 * beceause fnc are stored in execution order, always the module that bring a specific group will start execution before other depending on this
 *
 * kernel spend time monting /dev
 * and waiting for all async modules to be initialize. (
 *  arch/sh/drivers/pci/pcie-sh7786.c, line 572
    drivers/s390/kvm/virtio_ccw.c, line 1066
    drivers/s390/block/dasd.c, line 3233
    drivers/acpi/battery.c, line 1310
    drivers/base/power/main.c:
    drivers/block/floppy.c, line 4392
    drivers/gpu/drm/i915/i915_dma.c, line 446
    drivers/ata/libata-core.c, line 6161      ***** async_schedule(async_port_probe, ap);
    drivers/scsi/ufs/ufshcd.c, line 5540
    drivers/scsi/scsi_scan.c, line 1901

    http://superuser.com/questions/599333/how-to-disable-kernel-probing-for-drive
    libata.force=2.00:disable
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
#include <linux/string.h>

/*
 * Why do I did this?
 * I need to define a table with information, this table has to be static, no reason to fill up on running time
 * elements in the table are found using an id, the id is define in a file include for every source file
 * changing the id and asociated information force to recompile all
 * moving definition to porper source file descentralize all the configuration
 * it is are look for the specific file to change info and build. i can see a full picture from one place
 */
#define none_nfo                       disable
#define grp_none_nfo                   disable
#define grp_hid_nfo                    disable
#define grp_dma_nfo                    disable
#define grp_char_nfo                   disable
#define grp_none_nfo                   disable
#define grp_usb_nfo                    disable
#define grp_ssb_nfo                    disable
#define grp_snd_hda_nfo                disable
#define intel_cqm_init_nfo             disable
#define pmc_atom_init_nfo              disable   /* /arch/x86/kernel/pmc_atom.c  */
#define amd_ibs_init_nfo               disable

#define ssb_modinit_nfo                deferred,grp_ssb
#define bcma_modinit_nfo               deferred,grp_ssb,ssb_modinit
#define b43_init_nfo                   deferred,grp_none,grp_ssb  /**/
#define b43_nfo                        deferred,grp_none,grp_ssb,hwrng_modinit   /* b43.ko */
#define b43legacy_init_nfo             deferred,grp_none,grp_ssb,hwrng_modinit  /* b43legacy.ko */
#define b44_init_nfo                   deferred,grp_none,grp_ssb
#define bgmac_init_nfo                 deferred,grp_none,grp_ssb
#define tg3_driver_nfo                 deferred,grp_none,grp_ssb    /* pci to check */
#define b43_pcmcia_init_nfo            deferred,grp_none,grp_pcmcia
#define ssb_hcd_init_nfo               deferred,grp_none,grp_ssb

#define ahci_pci_driver_init_nfo       asynchronized   /* ahci.ko */
#define ahci_driver_init_nfo           asynchronized,grp_none,ahci_pci_driver_init  /**/
#define fbmem_init_nfo                 asynchronized    /* drivers/video/fbdev/core/fbmem.c */
#define mda_console_init_nfo           asynchronized,grp_none,fbmem_init    /* drivers/video/console/mdacon.c */
#define newport_console_init_nfo       asynchronized,grp_none,fbmem_init    /* drivers/video/console/newport_con.c  */
#define sticonsole_init_nfo            asynchronized,grp_none,fbmem_init    /* drivers/video/console/sticon.c  */
#define acpi_ac_init_nfo               asynchronized  /**/
#define workingset_init_nfo            asynchronized   /* mm/workingset.c  */
#define fcntl_init_nfo                 asynchronized
#define agp_init_nfo                   asynchronized,grp_none,ioat_init_module,acpi_video_init   /* agpgart.ko */
#define agp_nvidia_init_nfo            asynchronized,grp_none,agp_init   /* nvidia-agp.ko */
#define cfq_init_nfo                   deferred   /* block/cfq-iosched.c  */
#define cmos_init_nfo                  deferred   /* rtc/rtc-cmos.c   */
#define cn_proc_init_nfo               asynchronized   /* connector/cn_proc.c */
#define configfs_init_nfo              asynchronized   /* fs/configfs/mount.c  */
#define coretemp_init_nfo              asynchronized   /* drivers/hwmon/coretemp.c  */
#define crc_t10dif_mod_init_nfo        asynchronized   /* lib/crc-t10dif.c   */
#define acpi_video_init_nfo            asynchronized   /* drivers/acpi/video.ko */
#define cubictcp_register_nfo          asynchronized   /* net/ipv4/tcp_cubic.c   */
#define dio_init_nfo                   asynchronized   /*  fs/direct-io.c  */
#define dnotify_init_nfo               asynchronized   /* /fs/notify/dnotify/dnotify.c  */
#define drm_core_init_nfo              asynchronized,grp_none,agp_init   /* drm.ko */
#define evdev_init_nfo                 asynchronized   /*  drivers/input/evdev.c  */
#define ext4_init_fs_nfo                asynchronized,grp_none,init_hugetlbfs_fs   /* fs/ext4/super.c  */
#define extfrag_debug_init_nfo         asynchronized   /* mm/vmstat.c  */
#define fcntl_init_nfo                 asynchronized   /**/
#define i2c_dev_init_nfo                asynchronized   /* /drivers/i2c/i2c-dev.c */
#define hugetlb_init_nfo                asynchronized   /* mm/hugetlb.c  */
#define i2c_i801_init_nfo               asynchronized   /* /i2c/busses/i2c-i801.c */
#define i2c_mux_gpio_driver_init_nfo    asynchronized   /* drivers/i2c/muxes/i2c-mux-gpio.c */
#define ikconfig_init_nfo               asynchronized   /* kernel/configs.c */
#define init_devpts_fs_nfo              asynchronized   /* fs/devpts/inode.c  */
#define init_hugetlbfs_fs_nfo           asynchronized   /* /fs/hugetlbfs/inode.c  */
#define init_mbcache_nfo                asynchronized   /* /fs/mbcache.c  */
#define deadline_init_nfo               asynchronized   /* /block/deadline-iosched.c */
#define init_nls_ascii_nfo              asynchronized   /* fs/nls/nls_ascii.c  */
#define init_nls_cp437_nfo              asynchronized   /* fs/nls/nls_cp437.c  */
#define init_nls_cp850_nfo              asynchronized   /* fs/nls/nls_cp850.c */
#define init_nls_cp852_nfo              asynchronized   /* fs/nls/nls_cp852.c   */
#define init_nls_iso8859_1_nfo          asynchronized   /*  fs/nls/nls_iso8859-1.c  */
#define init_nls_utf8_nfo               asynchronized   /* fs/nls/nls_utf8.c */
#define init_per_zone_wmark_min_nfo     asynchronized   /* mm/page_alloc.c */
#define init_sd_nfo                     asynchronized   /* /drivers/scsi/sd.c   */
#define init_sg_nfo                     asynchronized   /* /drivers/scsi/sg.c   */
#define init_nfo                           asynchronized   /**/
#define inotify_user_setup_nfo             asynchronized   /*  fs/notify/inotify/inotify_user.c  */
#define ioat_init_module_nfo               asynchronized,grp_dma   /* drivers/dma/ioat/pci.c ioatdma.ko */
#define proc_execdomains_init_nfo          asynchronized   /**/
#define proc_filesystems_init_nfo          asynchronized   /* fs/filesystems.c */
#define proc_genhd_init_nfo                asynchronized   /* block/genhd.c   */
#define proc_locks_init_nfo                asynchronized   /* fs/locks.c  */
#define proc_modules_init_nfo              asynchronized   /**/
#define proc_vmalloc_init_nfo              asynchronized   /* /mm/vmalloc.c  */
#define libcrc32c_mod_init_nfo             asynchronized   /* lib/libcrc32c.c  */
#define nvidia_frontend_init_module_nfo    asynchronized,grp_none,drm_core_init /* nvidia.ko */
#define nvram_init_nfo                     asynchronized,grp_none,pty_init   /* drivers/char/nvram.c */
#define percpu_counter_startup_nfo         asynchronized   /*  lib/percpu_counter.c  */
#define phy_module_init_nfo                asynchronized   /* /include/linux/phy.h   */
#define ptp_pch_init_nfo                   asynchronized   /* drivers/ptp/ptp_pch.c  */
#define pty_init_nfo                       asynchronized   /*  drivers/tty/pty.c  */
#define intel_idle_init_nfo                asynchronized   /* /drivers/idle/intel_idle.c */
#define pch_dma_driver_init_nfo            asynchronized   /* drivers/dma/pch_dma.c */
#define sbf_init_nfo                       asynchronized   /*  arch/x86/kernel/bootflag.c  */
#define setup_vmstat_nfo                   asynchronized   /* /mm/vmstat.c  */
#define uvm_init_nfo                       asynchronized,grp_none,nvidia_frontend_init_module   /* nvidia-uvm.ko */
#define sock_diag_init_nfo                 asynchronized   /* /net/core/sock_diag.c  */

//Sound
#define generic_driver_init_nfo           deferred,grp_none,grp_snd_hda  /*/sound/pci/hda/hda_generic.c*/
#define realtek_driver_init_nfo           deferred,grp_none,grp_snd_hda /*sound/pci/hda/patch_realtek.c */
#define cmedia_driver_init_nfo            deferred,grp_none,grp_snd_hda  /* sound/pci/hda/patch_cmedia.c */
#define analog_driver_init_nfo            deferred,grp_none,grp_snd_hda  /* sound/pci/hda/patch_analog.c */
#define sigmatel_driver_init_nfo          deferred,grp_none,grp_snd_hda /* sound/pci/hda/patch_sigmatel.c   */
#define si3054_driver_init_id_nfo         deferred,grp_none,grp_snd_hda /* sound/pci/hda/patch_si3054.c  */
#define cirrus_driver_init_nfo            deferred,grp_none,grp_snd_hda /*sound/pci/hda/patch_cirrus.c */
#define cirrus_driver_init_nfo            deferred,grp_none,grp_snd_hda /* sound/pci/hda/patch_cirrus.c */
#define ca0110_driver_init_nfo            deferred,grp_none,grp_snd_hda /* sound/pci/hda/patch_ca0110.c */
#define ca0132_driver_init_nfo            deferred,grp_none,grp_snd_hda  /* sound/pci/hda/patch_ca0132.c */
#define conexant_driver_init_nfo          deferred,grp_none,grp_snd_hda  /* sound/pci/hda/patch_conexant.c */
#define via_driver_init_nfo               deferred,grp_none,grp_snd_hda  /* sound/pci/hda/patch_via.c  */
#define hdmi_driver_init_nfo              deferred,grp_none,grp_snd_hda  /* sound/pci/hda/patch_hdmi.c  */
#define si3054_driver_init_nfo            deferred,grp_none,grp_snd_hda

#define a4_driver_init_nfo                deferred,grp_none,hid_init,uhid_init  /**/
#define acpi_battery_init_nfo             deferred /**/
#define acpi_button_driver_init_nfo       deferred  /* */
#define acpi_fan_driver_init_nfo          deferred // asynchronized   /**/
#define acpi_hed_driver_nfo               deferred                /* */
#define acpi_ipmi_init_nfo                deferred /**/
#define acpi_pad_init_nfo                 deferred /**/
#define acpi_pcc_driver_nfo               deferred,grp_none,acpi_ac_init
#define acpi_power_meter_init_nfo         deferred  /* */
#define acpi_processor_driver_init_nfo    deferred   /* acpi-power-meter.ko */
#define acpi_sbs_init_nfo                 deferred /**/
#define acpi_smb_hc_driver_init_nfo       deferred
#define acpi_smb_hc_driver_nfo            deferred,grp_none,acpi_ac_init                /* */
#define acpi_smbus_cmi_driver_nfo         deferred                /* */
#define acpi_thermal_init_nfo             deferred /**/
#define acpi_topstar_driver_nfo           deferred,grp_none,acpi_ac_init                /* */
#define add_pcspkr_nfo                     deferred     /* arch/x86/kernel/pcspeaker.c */
#define aes_init_nfo                       deferred  /**/
#define af_alg_init_nfo                    deferred   /* af_alg.ko */
#define alg_hash_nfo                       deferred   /* alg_hash.ko */
#define algif_hash_init_nfo                deferred,grp_none,af_alg_init   /* algif_hash.ko */
#define algif_skcipher_init_nfo            deferred,grp_none,af_alg_init   /* algif_skicupher.ko */
#define alsa_hwdep_init_nfo                deferred         /* snd-hwdep.ko */
#define alsa_mixer_oss_init_nfo            deferred     /* snd-mixer-oss.ko */
#define alsa_pcm_init_nfo                  deferred,grp_none,alsa_timer_init           /* snd-pcm.ko */
#define alsa_pcm_oss_init_nfo              deferred,grp_none,alsa_mixer_oss_init,alsa_pcm_init       /* snd-pcm-oss.ko */
#define alsa_seq_device_init_nfo           deferred    /* snd-seq-device.ko */
#define alsa_seq_dummy_init_nfo            deferred,grp_none,alsa_seq_init     /* snd-seq-dummy.ko */
#define alsa_seq_init_nfo                  deferred,grp_none,alsa_seq_device_init,alsa_timer_init           /* snd-seq.ko */
#define alsa_seq_midi_event_init_nfo       deferred,grp_none,alsa_seq_init      /* snd-seq-midi-event.ko */
#define alsa_seq_oss_init_nfo               deferred,grp_none,alsa_seq_midi_event_init       /* snd-seq-oss.ko */
#define alsa_sound_last_init_nfo            disable  /* sound/core/last.c */
#define alsa_timer_init_nfo                 deferred         /* snd-timer.ko */
#define anubis_mod_init_nfo                 deferred   /**/
#define apple_driver_init_nfo               deferred,grp_none,hid_init,uhid_init  /**/
#define arc4_init_nfo                       deferred   /**/
#define asymmetric_key_init_nfo             deferred  /**/
#define async_pq_init_nfo                   deferred   /**/
#define atkbd_init_nfo                      deferred  /**/
#define atlas_acpi_driver_nfo               deferred,grp_none,acpi_ac_init                /* */
#define azx_driver_init_nfo                 deferred  /**/

#define belkin_driver_init_nfo              deferred,grp_none,hid_init,uhid_init  /**/
#define blowfish_mod_init_nfo               deferred   /* blowfish_generic.ko */
#define brd_init_nfo                         deferred  /**/
#define camellia_init_nfo                   deferred   /**/
#define cast5_mod_init_nfo                  deferred   /* cat5_generic.ko */
#define cast6_mod_init_nfo                  deferred   /* cast6_generic.ko */
#define chainiv_module_init_nfo              deferred   /**/
#define cherry_driver_init_nfo              deferred,grp_none,hid_init,uhid_init   /**/
#define chicony_driver_init_nfo              deferred,grp_none,hid_init,uhid_init  /**/
#define coretemp_nfo                         deferred   /* coretemp.ko */
#define cp_driver_init_nfo                     deferred,grp_none,hid_init,uhid_init  /**/
#define cpufreq_gov_dbs_init_nfo               deferred  /**/
#define cpufreq_gov_powersave_init_nfo          deferred  /**/
#define cpufreq_gov_userspace_init_nfo          deferred  /**/
#define cpufreq_stats_init_nfo                  deferred  /**/
#define crb_acpi_driver_nfo                    deferred,grp_none,acpi_ac_init                /* */
#define crc32_mod_init_nfo                     deferred  /**/
#define crc32c_mod_init_nfo                   deferred  /**/
#define crct10dif_mod_init_nfo                 deferred  /**/
#define crypto_algapi_init_nfo               deferred   /**/
#define crypto_authenc_esn_module_init_nfo    deferred,grp_none,crypto_authenc_module_init   /* authencesn.ko */
#define crypto_authenc_module_init_nfo        deferred   /* authenc.ko */
#define crypto_cbc_module_init_nfo            deferred  /**/
#define crypto_ccm_module_init_nfo            deferred   /**/
#define crypto_cmac_module_init_nfo           deferred   /**/
#define crypto_ctr_module_init_nfo            deferred   /**/
#define crypto_cts_module_init_nfo            deferred   /**/
#define crypto_ecb_module_init_nfo            deferred  /**/
#define crypto_gcm_module_init_nfo            deferred   /**/
#define crypto_module_init_nfo                deferred  /**/
#define crypto_null_mod_init_nfo              deferred  /**/
#define crypto_pcbc_module_init_nfo           deferred   /**/
#define crypto_user_init_nfo                  deferred  /**/
#define crypto_xcbc_module_init_nfo          deferred  /* */
#define cuse_init_nfo                        deferred,grp_none,fuse_init   /* cuse.ko */
#define deflate_mod_init_nfo                 deferred   /**/
#define des_generic_mod_init_nfo           deferred   /**/
#define drm_fb_helper_modinit_nfo          deferred    /* */
#define ehci_hcd_init_nfo                  deferred   /* ehci-hcd.ko */
#define ehci_pci_init_nfo                  deferred,grp_none,ehci_hcd_init   /* ehci-pci.ko */
#define ehci_platform_init_nfo             deferred,grp_none,ehci_hcd_init   /* ehci-platform.ko */
#define elo_driver_init_nfo                deferred   /* usbhid.ko */
#define ene_ub6250_driver_init_nfo        deferred,grp_none,usb_storage_driver_init   /* ums-eneub6250.ko */
#define eseqiv_module_init_nfo            deferred   /**/
#define ez_driver_init_nfo                deferred,grp_none,hid_init,uhid_init  /**/
#define fcrypt_mod_init_nfo               deferred   /**/
#define forcedeth_pci_driver_init_nfo     deferred  /* */
#define fuse_init_nfo                    deferred   /* fuse.ko */
#define ghash_mod_init_nfo               deferred   /**/
#define gpio_fan_driver_init_nfo         deferred//  asynchronized   /*  drivers/hwmon/gpio-fan.c  */
#define gpio_fan_nfo                     deferred   /* gpio-fan.ko */
#define gspca_init_nfo                   deferred   /*  /drivers/media/usb/gspca/gspca.c */
#define gspca_main_nfo                   deferred   /* gspca_main.ko */
#define hid_generic_init_nfo             deferred,grp_none,hid_init,uhid_init  /**/
#define hid_generic_nfo                   deferred,grp_none,hid_init,uhid_init  /**/
#define hid_init_nfo                      deferred,grp_none,ohci_platform_init        /**/
#define hilscher_pci_driver_init_nfo      deferred,grp_none,uio_init   /* uio_cif.ko */
#define hmac_module_init_nfo              deferred   /**/
#define hpet_init_nfo                     deferred  /**/
#define hwrng_modinit_nfo                 deferred   /* rng-core.ko */
#define i2c_hid_driver_init_nfo           deferred  /**/
#define i2c_mux_gpio_driver_nfo           deferred   /* i2c-mux-gpio.ko module_platform_driver */
#define i8042_init_nfo                    deferred  /**/
#define init_autofs4_fs_nfo               deferred   /*  /fs/autofs4/init.c  */
#define init_cifs_nfo                     deferred                /* */
#define init_iso9660_fs_nfo               deferred   /* isofs.ko */
#define init_dns_resolver_nfo             deferred  //asynchronized   /* net/dns_resolver/dns_key.c  */
#define init_ext2_fs_nfo                  deferred   /* fs/ext2/super.c  */
#define init_ext3_fs_nfo                 deferred,grp_none,journal_init   /* ext3.ko */
#define init_fat_fs_nfo                  deferred   /* fat.ko */
#define init_msdos_fs_nfo                deferred,grp_none,init_fat_fs   /* msdos.ko */
#define init_mtd_nfo                    deferred   /* mtd.ko */
#define init_ntfs_fs_nfo                deferred  /**/
#define init_udf_fs_nfo                 deferred   /* fs/udf/super.c */
#define init_vfat_fs_nfo                deferred,grp_none,init_fat_fs   /* vfat.ko */
#define intel_rng_mod_init_nfo          deferred,grp_none,hwrng_modinit   /* intel-rng.ko */
#define ipw2100_init_nfo                deferred,grp_none,lib80211_init   /* ipw2100.ko */
#define ir_kbd_driver_init_nfo          deferred  /**/
#define ir_kbd_driver_nfo               deferred   /* ir-kbd-i2c.ko module_i2c_driver */
#define irst_driver_nfo                 deferred,grp_none,acpi_ac_init                /* */
#define ismt_driver_init_nfo            deferred //asynchronized   /* drivers/i2c/busses/i2c-ismt.c */
#define journal_init_nfo                deferred   /* jbd.ko */
#define keytouch_driver_init_nfo        deferred,grp_none,hid_init,uhid_init  /**/
#define khazad_mod_init_nfo             deferred   /**/
#define krng_mod_init_nfo               deferred   /**/
#define ks_driver_init_nfo              deferred,grp_none,hid_init,uhid_init  /**/
#define kswapd_init_nfo                 asynchronized   /**/
#define led_class_nfo                   deferred   /* led-class.ko */
#define led_driver_init_nfo             deferred      /* usbled.ko */
#define leds_pca955x_nfo                deferred   /* leds-pca955x.ko */
#define lg_driver_init_nfo              deferred,grp_none,hid_init,uhid_init  /**/
#define lib80211_crypto_ccmp_init_nfo    deferred,grp_none,lib80211_init   /* lib80211_crypt_ccmp.ko */
#define lib80211_crypto_tkip_init_nfo    deferred,grp_none,lib80211_init   /* lib80211_crypt_tkip.ko */
#define lib80211_crypto_wep_init_nfo    deferred,grp_none,lib80211_init   /* lib80211_crypt_wep.ko */
#define lib80211_init_nfo                deferred   /* lib80211.ko */
#define libipw_init_nfo                 deferred,grp_none,lib80211_init   /* libipw.ko */
#define libphy_nfo                      deferred   /* libphy.ko */
#define lis3lv02d_driver_nfo            deferred,grp_none,acpi_ac_init                /* */
#define loop_init_nfo                   deferred  /**/
#define lpc_ich_driver_init_nfo        deferred // asynchronized   /* drivers/mfd/lpc_ich.c  chipset */
#define lpc_sch_driver_init_nfo        deferred // asynchronized   /* /drivers/mfd/lpc_sch.c */
#define lz4_mod_init_nfo               deferred  /**/
#define lzo_mod_init_nfo               deferred   /* lzo.ko */
#define md4_mod_init_nfo               deferred  /**/
#define md5_mod_init_nfo               deferred  /**/
#define michael_mic_init_nfo           deferred   /**/
#define mmc_blk_init_nfo               deferred   /* mmc_block.ko */
#define mod_init_nfo                   deferred // asynchronized,grp_none,pty_init   /* /drivers/char/hw_random/intel-rng.c */
#define mousedev_init_nfo              deferred  /**/
#define mr_driver_init_nfo             deferred,grp_none,hid_init,uhid_init  /**/
#define ms_driver_init_nfo             deferred,grp_none,hid_init,uhid_init  /**/
#define mxm_wmi_init_nfo               deferred   /* mxm-wmi.ko */
#define nforce2_driver_init_nfo        deferred  /* */
#define nforce2_init_nfo               deferred  /* drivers/cpufreq/cpufreq-nforce2.c */
#define noop_init_nfo                  deferred// asynchronized   /*  /block/noop-iosched.c  */
#define ohci_hcd_mod_init_nfo          deferred,ehci_platform_init   /* ohci-hcd.ko */
#define ohci_pci_init_nfo              deferred,ohci_hcd_mod_init,ehci_hcd_init   /* ohci-pci.ko */
#define ohci_platform_init_nfo         deferred,ohci_hcd_mod_init   /* ohci-platform.ko */
#define oprofile_init_nfo              deferred // asynchronized     /* oprofile/oprof.c */
#define packet_init_nfo                deferred // asynchronized   /* net/packet/af_packet.c   */
#define patch_analog_init_nfo          deferred       /* snd-hda-codec-analog.ko */
#define patch_ca0110_init_nfo          deferred       /* snd-hda-codec-ca0110.ko */
#define patch_ca0132_init_nfo          deferred       /* snd-hda-codec-ca0132.ko */
#define patch_cirrus_init_nfo          deferred       /* snd-hda-codec-cirrus.ko */
#define patch_cmedia_init_nfo          deferred       /* snd-hda-codec-cmedia.ko */
#define patch_conexant_init_nfo        deferred     /* snd-hda-codec-conexant.ko */
#define patch_hdmi_init_nfo            deferred         /* snd-hda-codec-hdmi.ko */
#define patch_realtek_init_nfo         deferred      /* snd-hda-codec-realtek.ko */
#define patch_si3054_init_nfo          deferred       /* snd-hda-codec-si3054.ko */
#define patch_sigmatel_init_nfo        deferred    /* snd-hda-codec-idt.ko */
#define patch_via_init_nfo             deferred          /* snd-hda-codec-via.ko */
#define pca9541_driver_init_nfo        deferred  /**/
#define pca9541_driver_nfo             deferred   /* i2c-mux-pca9541.ko module_i2c_driver */
#define pca954x_driver_init_nfo        deferred  /**/
#define pca954x_driver_nfo             deferred   /* i2c-mux-pca954x.ko module_i2c_driver */
#define pca955x_driver_init_nfo        deferred  /**/
#define pci_hotplug_init_nfo           deferred,grp_none,pcie_portdrv_init  /**/
#define pcie_pme_service_init_nfo      deferred   /**/
#define pcie_portdrv_init_nfo           deferred  /**/
#define pcied_init_nfo                  deferred,grp_none,pci_hotplug_init  /**/
#define pcips2_driver_init_nfo         deferred // asynchronized  /* drivers/input/serio/pcips2.c */
#define pkcs7_key_init_nfo             deferred  /**/
#define plantronics_driver_init_nfo    deferred,grp_none,hid_init,uhid_init  /**/
#define prgn_mod_init_nfo             deferred  /**/
#define prng_mod_init_nfo             deferred  /* */
#define psmouse_init_nfo              deferred  /**/
#define pvpanic_driver_nfo            deferred,grp_none,acpi_ac_init                /* */
#define realtek_cr_driver_init_nfo    deferred,grp_none,usb_storage_driver_init   /* ums-realtek.ko */
#define rfcomm_init_nfo               deferred             /* rfcomm.ko */
#define rmd128_mod_init_nfo           deferred  /**/
#define rmd160_mod_init_nfo            deferred  /**/
#define rmd256_mod_init_nfo           deferred  /**/
#define rmd320_mod_init_nfo           deferred  /**/
#define salsa20_generic_mod_init_nfo    deferred   /**/
#define seed_init_nfo                     deferred   /**/
#define seqiv_module_init_nfo          deferred   /**/
#define serial_pci_driver_init_nfo     deferred   /* drivers/tty/serial/8250/8250_pci.c  chipset */
#define serial8250_init_nfo            deferred  /* drivers/tty/serial/8250/8250_core.c */
#define serio_raw_drv_init_nfo         deferred //asynchronized  /* drivers/input/serio/serio_raw.c*/
#define sermouse_drv_init_nfo          deferred // asynchronized   /* drivers/input/mouse/sermouse.c */
#define serpent_mod_init_nfo           deferred   /**/
#define serport_init_nfo               deferred  /**/
#define sha1_generic_mod_init_nfo      deferred  /**/
#define sha256_generic_mod_init_nfo    deferred  /**/
#define sha512_generic_mod_init_nfo     deferred  /**/
#define shpcd_init_nfo                 deferred,grp_none,pcied_init  /**/
#define simtec_i2c_driver_init_nfo    deferred // asynchronized   /* drivers/i2c/busses/i2c-simtec.c  */
#define slab_proc_init_nfo              asynchronized   /* /mm/slab_common.c  */
#define smartconnect_driver_nfo        deferred,grp_none,acpi_ac_init                /* */
#define smbalert_driver_init_nfo       deferred  /**/
#define smbus_sch_driver_init_nfo     deferred //  asynchronized   /*  drivers/i2c/busses/i2c-isch.c  */
#define smo8800_driver_nfo             deferred,grp_none,acpi_ac_init                /* */
#define smsc_nfo                       deferred,grp_none,libphy   /* smsc.ko */
#define snd_compress_init_nfo          deferred  /* sound/core/compress_offload.c */
#define snd_hda_controller_nfo         deferred      /* snd-hda-controller.ko */
#define snd_hda_intel_nfo              deferred,grp_none,snd_hda_controller           /* snd-hda-intel.ko */
#define snd_hrtimer_init_nfo           deferred,grp_none,alsa_timer_init    /* snd-hrtimer.ko */
#define speedstep_init_nfo             deferred   /* speedstep-ich.ko */
#define spi_gpio_driver_init_nfo       deferred
#define synusb_driver_init_nfo         deferred     /*   */
#define tcp_congestion_default_nfo     deferred  /**/
#define tcrypt_mod_init_nfo            deferred  /**/
#define tea_mod_init_nfo               deferred  /**/
#define tgr192_mod_init_nfo            deferred  /**/
#define toshiba_bt_rfkill_driver_nfo    deferred,grp_none,acpi_ac_init                /* */
#define toshiba_haps_driver_nfo        deferred,grp_none,acpi_ac_init                /* */
#define twofish_generic_nfo            deferred   /* twofish_generic.ko */
#define twofish_i586_nfo               deferred   /* twofish-i586.ko */
#define twofish_mod_init_nfo           deferred  /**/
#define uas_driver_init_nfo            deferred,grp_none,usb_storage_driver_init   /* uas.ko */
#define ubi_init_nfo                   deferred,grp_none,init_mtd   /* ubi.ko */
#define uhci_hcd_init_nfo              deferred,grp_none,ehci_hcd_init     /* uhci-hcd.ko */
#define uhid_init_nfo                  deferred,grp_none,ohci_platform_init        /* uhid.ko */
#define uinput_init_nfo                deferred  /**/
#define uio_init_nfo                   deferred   /* uio.ko */
#define usb_hid_init_nfo               deferred   /* usbhid.ko */
#define usb_storage_driver_init_nfo    deferred      /* usb-storage.ko */
#define usblp_driver_init_nfo          deferred     /*   */
#define usbmon_nfo                     deferred      /* usbmon.ko */
#define uvc_init_nfo                   deferred   /*  drivers/media/usb/uvc/uvc_driver.c   */
#define uvcvideo_nfo                   deferred   /* uvcvideo.ko */
#define vb2_thread_init_nfo            deferred  /**/
#define vmac_module_init_nfo           deferred  /**/
#define wp512_mod_init_nfo             deferred  /**/
#define x509_key_init_nfo              deferred  /**/
#define xo15_ebook_driver_nfo          deferred,grp_none,acpi_ac_init                /* */
#define zlib_mod_init_nfo              deferred   /* zlib.ko */
#define max_nfo                        disable
#define pcspkr_platform_driver_init_nfo     deferred
#define deinterlace_pdrv_init_nfo           deferred  /* /drivers/media/platform/m2m-deinterlace.c*/

#if 0
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

/*
 * TODO - to keep init data,
 * create a thread at driver initialization that  do everything in stages.
 * when a stage finish wait for the next one
 * this way allow to file operations trigger initialization.
 */

/**
 * Static struct holding all data
 */
#ifndef TEST
extern struct init_fn_t_4 __async_initcall_start[], __async_initcall_end[];
#endif

#ifdef CONFIG_ASYNCHRO_MODULE_INIT_DEBUG
#define printk_debug(...) printk(KERN_ERR  __VA_ARGS__)
#else
#define printk_debug(...) do {} while(0)
#endif

#ifndef CONFIG_ASYNCHRO_MODULE_INIT_THREADS
#define CONFIG_ASYNCHRO_MODULE_INIT_THREADS 1
#endif

/*
 * Modules v4. Optimization using more static data fromo modules
 */
enum task_status_t_4
{
  st_disable = 0,    //
  st_waiting,    //
  st_running,    //
  st_done,
};

struct task_info_t_4
{
    unsigned ref;       // how many modules we need to release this task
    unsigned status;        // task status (disable, waiting, running, done)
    unsigned child_count;    // count of child task waiting for this one
};

#define get_nfo_1(type)             {type,grp_none_id, none_id,none_id } ,
#define get_nfo_2(type,grp)         {type,grp ## _id, none_id,none_id }  ,
#define get_nfo_3(type,grp,p1)      {type,grp ## _id, p1 ## _id,none_id },
#define get_nfo_4(type,grp,p1,p2)   {type,grp ## _id, p1 ## _id, p2 ## _id },

#define get_nfo(x) CALL_FNC(get_nfo_,x ## _nfo)

// it is better use a unique place for modules info
struct  init_fnc_info_4 {
  enum task_type_t type;
  modules_e  grp_id;
  modules_e  parent1_id;
  modules_e  parent2_id;
};

const static struct  init_fnc_info_4  /*__initdata*/ init_info[] = {
    MODULES_ID(get_nfo)
    {} };

struct task_info_t_4  /*__initdata*/ info_4[module_last + 1];    // all task info
//struct init_fn_t* current_init_fnc;      // current init function to execute
//enum task_type_t  current_type;          // current type.


//static DEFINE_SPINLOCK(list_lock);
//static DECLARE_WAIT_QUEUE_HEAD( list_wait);


//#ifdef CONFIG_ASYNCHRO_MODULE_INIT_DEBUG
const char* getName(modules_e id)
{
    static const char* const module_name[] =
    {   MODULES_ID(TASK_NAME)};
    if (id >= module_last)
        return "";
    return module_name[id];
}
//#else
//const char* getName(modules_e id) { return ""; }
//#endif

/*
 * Initialize modules dynamic data
 */
/*
 * Read all information from static memory an expand it to dynamic memory
 */
void FillTasks(const struct init_fn_t_4* begin, const struct init_fn_t_4* end)
{
  const struct init_fn_t_4* it;
  const struct init_fnc_info_4* nfo;
  memset(info_4, 0, sizeof(info_4));      //clear all status information
  // update child and group reference counter
  for (it = begin; it != end; ++it)
  {
    nfo = &init_info[it->id];
    info_4[it->id].status = st_waiting;
    if (nfo->parent1_id != none_id)
    {
      ++info_4[nfo->parent1_id].child_count;
    }
    if (nfo->parent2_id != none_id)
      ++info_4[nfo->parent2_id].child_count;

    if (nfo->grp_id != grp_none_id)
    {
      info_4[nfo->grp_id].ref++;     // also use as group counter
      info_4[nfo->grp_id].status = st_waiting;
    }
  }
  info_4[none_id].status = st_done;
  info_4[grp_none_id].status = st_done;
}

//const task_info_t* peekTask(const task_info_t* it)
//  {
//    bool towait;    // if true means wait for completion, false return current task or null
//    unsigned child_count = 0; // how many child to wakeup
//    std::unique_lock < std::mutex > lock(mtx);
//    if (it != nullptr)
//    {
//      status[it->id].status = done;
//      child_count += status[it->id].child_count;
//      if (it->grp_id != ID::grp_none_id)
//      {
//        --status[it->grp_id].grp_ref;
//        if (status[it->grp_id].grp_ref == 0)
//        {
//          child_count += status[it->grp_id].child_count;
//          status[it->grp_id].status = done;
//        }
//      }
//      // put back all done task
//      if (it == begin)
//      {
//        while (begin != end && status[begin->id].status == done)
//          ++begin;
//      }
//    }
//    for (;;)
//    {
//      towait = false;    // no task to waiting for
//      for (it = begin; it != end; ++it)
//      {
//        // find any ready task
//        if (status[it->id].status == waiting)
//        {
//          if (status[it->parent_id].status == done && status[it->parent_id2].status == done)
//          {
//            status[it->id].status = running;
//            break;
//          }
//          towait = true;
//        }
//      }
//      if (it != end)
//        break;     // get out for ;;
//      // we got nothing
//      if (towait)    // wait and try again
//      {
//        std::cout << 'W' << std::endl;
//        cond_var.wait(lock);
//      }
//      else
//      {
//        it = nullptr;    // we done here
//        child_count = 3;  // wake_up all task because we done
//        break;    // get out for ;;
//      }
//    }


/**
 * Thread for version 2
 */

int  ProcessThread2(void *data)
{
//    int ret;
//    struct task_t* ptask = NULL;
//    printk_debug("async %d starts\n", (unsigned )data);
//    do
//    {
//        ret = wait_event_interruptible(list_wait, (ptask = PeekTask()) != NULL || tasks.type_ != waiting);
//        if (ret != 0)
//        {
//            printk("async init wake up returned %d\n", ret);
//            break;
//        }
//        if (ptask != NULL)
//        {
//            printk_debug("async %d %pF %s\n", (unsigned)data, ptask->fnc,getName(ptask->id));
//            ret = do_one_initcall(ptask->fnc);
//            //TODO check return code and invalidate all task that depends on this one
//            TaskDone2(ptask);
//        }
//    } while (ptask != 0);
//    printk_debug("async %d ends\n", (unsigned)data);
    return 0;
}

/**
 * Execute all initialization for an specific type
 * We need wait for everything done as a barrier to avoid problems
 */
int  start_threads(int(* thread_fnc) (void*) )
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
    if (/*type == deferred && */ max_cpus > 1)
    {
        --max_cpus;
    }
    printk_debug("async using %d cpus\n", max_cpus);
    for (it=0; it < max_cpus;  ++it)
    {
        //start working threads
        thr = kthread_create(thread_fnc, (void* )(it), "async_thread_%d",it);
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

static atomic_t init_done = ATOMIC_INIT(0);
extern atomic_t  free_init_ref;         // become zero when all drivers are initialize.
// current module



/*
 * Do all initialization by default way
 */
int doall_default(void* d)
{
    int old = atomic_xchg(&init_done, 1);
    if (old == 0)
    {
        int ret;
        const struct init_fn_t_4* it_init_fnc;
        for (it_init_fnc = __async_initcall_start; it_init_fnc < __async_initcall_end; ++it_init_fnc)
        {
            ret = do_one_initcall(it_init_fnc->fnc);
            schedule();        // give time to system to do other things
        }
    }
    if (atomic_dec_and_test(&free_init_ref) )
          free_initmem();
    return 0;
}

/*
 * wait for all task to be done
 * todo use a global counter equal to 2 to known when all stages are done (asyn,deferred) use a wait queue for notification
 */
inline void wait_(void)
{
 //   wait_event_interruptible(list_wait, (tasks.type_ == waiting));
}

/**
 * Module initialization is taking a long time, more than any other.
 * Module initialization and fist execution is going to be do from thread
 */


/**
 * Do an normal initialization for an specific type
 */
int do_asynchronized(void* d)
{
    int ret;
    const struct init_fn_t_4* it_init_fnc;
    for (it_init_fnc = __async_initcall_start; it_init_fnc < __async_initcall_end; ++it_init_fnc)
    {
        if (init_info[it_init_fnc->id].type == asynchronized)
        {
            ret = do_one_initcall(it_init_fnc->fnc);
            schedule();        // give time to system to do other things
        }
    }
//    if (atomic_dec_and_test(&free_init_ref) )
//             free_initmem();
    return 0;
}

/*
 * Structure holding all device file data
 */
struct device_t
{
    struct init_fn_t* it_init_fnc;      // current init function to execute
};

int device_open(struct inode * i, struct file * f)
{
    int old = atomic_xchg(&init_done, 1);
    if (old == 1)
    {
        printk(KERN_ERR "deferred init already done");
        return -EINVAL;      // already done
    }
    f->private_data = __async_initcall_start;
    return 0;
}

static ssize_t device_read(struct file *file, char __user *buf,size_t nbytes, loff_t *ppos)
{
    struct init_fn_t_4* it_init_fnc;
    const char* initcall_name;
    size_t count;
    count = 0;
    it_init_fnc = (struct init_fn_t_4*)file->private_data;

    while (it_init_fnc < __async_initcall_end && init_info[it_init_fnc->id].type != deferred)
        ++it_init_fnc;
    if (it_init_fnc != __async_initcall_end)
    {
        do_one_initcall(it_init_fnc->fnc);
        initcall_name = getName(it_init_fnc->id);
        ++it_init_fnc;
        file->private_data = it_init_fnc;
        if (initcall_name == 0 || *initcall_name == 0)
        {
            initcall_name = ".";
        }
        if (nbytes != 0)
        {
           count = strlen(initcall_name) + 1;  // including zero to swap with a \n char
           if (count > nbytes)
               count = nbytes;
           copy_to_user(buf,initcall_name,count);
           if (count > 1)
               buf[count-1] = '\n';     // if there is only space for one char we do not set \n
        }
    }
    else
    {
      if (atomic_dec_and_test(&free_init_ref) )
               free_initmem();
    }
    return count;
}

static const struct file_operations deferred_initcalls_fops = {
   .open = device_open, //
   .read = device_read,
   //TODO .write = device_write, // write ascii 1 to do all calls in one go
};

/**
 * Module entry point
 */
//static int async_init(void)
//{
//  struct task_struct *thr;
//
//  current_type = asynchronized;
//  atomic_inc(&free_init_ref);
//  thr = kthread_create(do_asynchronized, (void*) (0), "do_type");
//  kthread_bind(thr, num_online_cpus() - 1);
//  wake_up_process(thr);
//  return 0;
//}

/**
 * Register proc entry to allow deferred initialization
 */
static int async_init(void)
{
  // print full list
  const struct init_fn_t_4* it_init_fnc;
  struct task_struct *thr;

//    for (it_init_fnc = __async_initcall_start; it_init_fnc < __async_initcall_end; ++it_init_fnc)
//    {
//      printk("%s %d \n",getName(it_init_fnc->id),init_info[it_init_fnc->id].type);
//    }

    atomic_inc(&free_init_ref);
    //atomic_inc(&free_init_ref);

    //current_type = asynchronized;
    atomic_inc(&free_init_ref);
    thr = kthread_create(do_asynchronized, (void*) (0), "do_type");
    kthread_bind(thr, num_online_cpus() - 1);
    wake_up_process(thr);

    proc_create("deferred_initcalls", 0, NULL, &deferred_initcalls_fops);
    return 0;
}

//__initcall(async_init);
module_init(async_init);		// Second stage, last to do before jump to high level initialization

