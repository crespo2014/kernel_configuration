
obj-$(CONFIG_RTL8168)	+= r8168.o

r8168-objs := r8168_n.o r8168_asf.o rtl_eeprom.o rtltool.o

obj-$(CONFIG_ENABLE_REALWOW_SUPPORT) += r8168_realwow.o
obj-$(CONFIG_ENABLE_DASH_SUPPORT) += r8168_dash.o

ccflags-$(CONFIG_ENABLE_REALWOW_SUPPORT) += -DENABLE_REALWOW_SUPPORT
ccflags-$(CONFIG_ENABLE_DASH_SUPPORT) += -DENABLE_DASH_SUPPORT
ccflags-$(CONFIG_CONFIG_DOWN_SPEED_100) += -DCONFIG_DOWN_SPEED_100
ccflags-$(CONFIG_CONFIG_ASPM) += -DCONFIG_ASPM
ccflags-$(CONFIG_ENABLE_S5WOL) += -DENABLE_S5WOL
ccflags-$(CONFIG_ENABLE_EEE) += -DENABLE_EEE

ccflags-y += -DCONFIG_R8168_NAPI -DCONFIG_R8168_VLAN


#fw-shipped-$(CONFIG_RTL8168) += iwlwifi-3160-9.ucode iwlwifi-3160-10.ucode iwlwifi-3160-8.ucode iwlwifi-3160-7.ucode ibt-hw-37.7.10-fw-1.0.2.3.d.bseq