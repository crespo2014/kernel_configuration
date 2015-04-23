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
#define INIT_CALLS(fnc)  \
        fnc(alsa_timer_init), \
        fnc(alsa_mixer_oss_init), \
        fnc(alsa_hwdep_init),\
        fnc(alsa_seq_device_init),\
        fnc(alsa_seq_init), \
        fnc(ahci_pci_driver),\
        fnc(agp_nvidia_init),\
        fnc(drm_core_init),\
        fnc(drm_fb_helper_modinit),\
        fnc(bt_init),      /* bluetooth.ko */  \
        fnc(init_mtd1),  /* rfcomm.ko */ \
        fnc(init_mtd2),  /* soundcore.ko */ \
        fnc(init_mtd3),  /* snd.ko */ \
        fnc(init_mtd4),  /* snd-timer.ko */ \
        fnc(init_mtd5),  /* snd-hwdep.ko */ \
        fnc(init_mtd6),  /* snd-seq-device.ko */ \
        fnc(init_mtd7),  /* snd-mixer-oss.ko */ \
        fnc(init_mtd8),  /* snd-hrtimer.ko */ \
        fnc(init_mtd9),  /* snd-pcm.ko */ \
        fnc(init_mtd10),  /* snd-seq.ko */ \
        fnc(init_mtd11),  /* snd-pcm-oss.ko */ \
        fnc(init_mtd12),  /* snd-hda-codec.ko */ \
        fnc(init_mtd13),  /* snd-seq-midi-event.ko */ \
        fnc(init_mtd14),  /* snd-seq-dummy.ko */ \
        fnc(init_mtd15),  /* snd-hda-controller.ko */ \
        fnc(init_mtd16),  /* snd-hda-codec-generic.ko */ \
        fnc(init_mtd17),  /* snd-hda-codec-si3054.ko */ \
        fnc(init_mtd18),  /* snd-hda-codec-ca0132.ko */ \
        fnc(init_mtd19),  /* snd-hda-codec-hdmi.ko */ \
        fnc(init_mtd20),  /* snd-seq-oss.ko */ \
        fnc(init_mtd21),  /* snd-hda-intel.ko */ \
        fnc(init_mtd22),  /* snd-hda-codec-idt.ko */ \
        fnc(init_mtd23),  /* snd-hda-codec-cirrus.ko */ \
        fnc(init_mtd24),  /* snd-hda-codec-ca0110.ko */ \
        fnc(init_mtd25),  /* snd-hda-codec-via.ko */ \
        fnc(init_mtd26),  /* snd-hda-codec-realtek.ko */ \
        fnc(init_mtd27),  /* snd-hda-codec-conexant.ko */ \
        fnc(init_mtd28),  /* snd-hda-codec-cmedia.ko */ \
        fnc(init_mtd29),  /* snd-hda-codec-analog.ko */ \
        fnc(init_mtd30),  /* hwmon.ko */ \
        fnc(init_mtd31),  /* coretemp.ko */ \
        fnc(init_mtd32),  /* gpio-fan.ko */ \
        fnc(init_mtd33),  /* acpi-power-meter.ko */ \
        fnc(init_mtd34),  /* mtd.ko */ \
        fnc(init_mtd35),  /* ubi.ko */ \
        fnc(init_mtd36),  /* uio.ko */ \
        fnc(init_mtd37),  /* uio_cif.ko */ \
        fnc(init_mtd38),  /* libata.ko */ \
        fnc(init_mtd39),  /* libahci.ko */ \
        fnc(init_mtd40),  /* ahci.ko */ \
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
        fnc(init_mtd62),  /* ohci-hcd.ko */ \
        fnc(init_mtd63),  /* uhci-hcd.ko */ \
        fnc(init_mtd64),  /* usbmon.ko */ \
        fnc(init_mtd65),  /* usb-storage.ko */ \
        fnc(init_mtd66),  /* usbled.ko */ \
        fnc(init_mtd67),  /* usbhid.ko */ \
        fnc(init_mtd68),  /* ehci-hcd.ko */ \
        fnc(init_mtd69),  /* ohci-pci.ko */ \
        fnc(init_mtd70),  /* ohci-platform.ko */ \
        fnc(init_mtd71),  /* ums-eneub6250.ko */ \
        fnc(init_mtd72),  /* uas.ko */ \
        fnc(init_mtd73),  /* ums-realtek.ko */ \
        fnc(init_mtd74),  /* ehci-pci.ko */ \
        fnc(init_mtd75),  /* ehci-platform.ko */ \
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
        fnc(init_mtd92),  /* agpgart.ko */ \
        fnc(init_mtd93),  /* drm.ko */ \
        fnc(init_mtd94),  /* nvidia-agp.ko */ \
        fnc(init_mtd95),  /* nvidia.ko */ \
        fnc(init_mtd96),  /* nvidia-uvm.ko */ \
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
        fnc(init_mtd126),  /* iosfs.ko */ \
        fnc(init_mtd127),  /* fuse.ko */ \
        fnc(init_mtd128),  /* cuse.ko */ \
        fnc(init_mtd129),  /* jbd.ko */ \
        fnc(init_mtd130),  /* ext3.ko */ \
        fnc(init_mtd131),  /* fat.ko */ \
        fnc(init_mtd132),  /* vfat.ko */ \
        fnc(init_mtd133),  /* msdos.ko */ \
        fnc(init_mtd134),  /* twofish_common.ko */ \
        fnc(init_mtd135),  /* twofish_generic.ko */ \
        fnc(init_mtd136)  /* twofish-i586.ko */

#define macro_str(x)    #x
#define macro_name(x)   x

/**
 * Implement static enum to string map
 */
enum modules_e {
    none = 0,           // not found or not element
    INIT_CALLS(macro_name)
    ,end
};


/**
 * Task type or execution priority
 * async - needs to be execute in an asynchronized way
 * deferred - it can be execute at user initialization time
 */
typedef enum { asynchronized, deferred } task_type_t;

struct init_fn
{
  task_type_t type_;    // task type
  enum modules_e  id;
  enum modules_e child_id;
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
  static struct init_fn init_fn_##fnc __used \
  __attribute__((__section__(".async_initcall.init"))) = {asynchronized,modules_e::fnc,#__VA_ARGS__,fnc};

#define deferred_module_init(fnc, ... )  \
  static struct init_fn init_fn_##fnc __used \
  __attribute__((__section__(".async_initcall.init"))) = {deferred,modules_e::fnc,#__VA_ARGS__,fnc};

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
