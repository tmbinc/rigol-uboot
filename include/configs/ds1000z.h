/*
 * (C) Copyright 2016 Savoir-faire Linux Inc.
 *
 * Author: Sebastien Bourdelin <sebastien.bourdelin@savoirfairelinux.com>
 *
 * Derived from MX28EVK code by
 *   Fabio Estevam <fabio.estevam@freescale.com>
 *   Freescale Semiconductor, Inc.
 *
 * Configuration settings for the TS4600 Board
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#ifndef __CONFIGS_DS1000Z_H__
#define __CONFIGS_DS1000Z_H__

/* System configurations */
#define CONFIG_MX28				/* i.MX28 SoC */

/* Memory configuration */
#define CONFIG_NR_DRAM_BANKS		1		/* 1 bank of DRAM */
#define PHYS_SDRAM_1			0x40000000	/* Base address */
#define PHYS_SDRAM_1_SIZE		0x04000000	/* 64MB */
#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1

/* net */

#define CONFIG_FEC_MXC_PHYADDR          5
#define CONFIG_PHY_MICREL               1
#define CONFIG_MX28_FEC_MAC_IN_OCOTP    1

/* Environment */
#define CONFIG_ENV_SIZE			(8 * 1024)

/* Environment is in MMC */
#define CONFIG_ENV_IS_NOWHERE           1

#if defined(CONFIG_CMD_MMC) && defined(CONFIG_ENV_IS_IN_MMC)
#define CONFIG_ENV_OFFSET		(256 * 1024)
#define CONFIG_SYS_MMC_ENV_DEV		0
#endif

/* USB */
#ifdef CONFIG_CMD_USB
#define CONFIG_EHCI_MXS_PORT0
#define CONFIG_EHCI_MXS_PORT1
#define CONFIG_USB_MAX_CONTROLLER_COUNT	2
#endif

/* Boot Linux */
#define CONFIG_LOADADDR		0x42000000
#define CONFIG_SYS_LOAD_ADDR	CONFIG_LOADADDR

#define CONFIG_EXTRA_ENV_SETTINGS \
	"videomode=video=ctfb:x:800,y:480,depth:24,mode:0,pclk:29851," \
	"le:35,ri:211,up:0,lo:24,hs:10,vs:21,sync:0,vmode:0 "

#define CONFIG_BOOTCOMMAND \
	"dhcp; " \
	"setenv fdt_addr 0x41000000;" \
	"tftpboot ${fdt_addr} 10.0.120.53:/tftpboot/imx28-ds1000z.dtb;" \
	"tftpboot 0x42000000 10.0.120.53:/tftpboot/zImage;" \
	"setenv bootargs root=/dev/sda1 rootwait rw;" \
	"bootz ${loadaddr} - ${fdt_addr}; "

/* console=ttyAPP0,115200  */

/* The rest of the configuration is shared */
#include <configs/mxs.h>

#endif /* __CONFIGS_DS1000Z_H__ */
