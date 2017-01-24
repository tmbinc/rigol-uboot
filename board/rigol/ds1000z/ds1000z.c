/*
 * DENX M28 module
 *
 * Copyright (C) 2011 Marek Vasut <marek.vasut@gmail.com>
 * on behalf of DENX Software Engineering GmbH
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux-mx28.h>
#include <asm/arch/clock.h>
#include <asm/arch/sys_proto.h>
#include <linux/mii.h>
#include <miiphy.h>
#include <netdev.h>
#include <errno.h>

DECLARE_GLOBAL_DATA_PTR;

/*
 * Functions
 */
int board_early_init_f(void)
{
	/* IO0 clock at 480MHz */
	mxs_set_ioclk(MXC_IOCLK0, 480000);
	/* IO1 clock at 480MHz */
	mxs_set_ioclk(MXC_IOCLK1, 480000);

	/* SSP0 clock at 96MHz */
	mxs_set_sspclk(MXC_SSPCLK0, 96000, 0);
	/* SSP2 clock at 160MHz */
	mxs_set_sspclk(MXC_SSPCLK2, 160000, 0);

	/* network */
#if 0
	mxs_iomux_setup_pad(MX28_PAD_ENET0_MDC__ENET0_MDC);
	mxs_iomux_setup_pad(MX28_PAD_ENET0_MDIO__ENET0_MDIO);
	mxs_iomux_setup_pad(MX28_PAD_ENET0_RX_EN__ENET0_RX_EN);
	mxs_iomux_setup_pad(MX28_PAD_ENET0_RXD0__ENET0_RXD0);
	mxs_iomux_setup_pad(MX28_PAD_ENET0_RXD1__ENET0_RXD1);
	mxs_iomux_setup_pad(MX28_PAD_ENET0_TX_EN__ENET0_TX_EN);
	mxs_iomux_setup_pad(MX28_PAD_ENET0_TXD0__ENET0_TXD0);
	mxs_iomux_setup_pad(MX28_PAD_ENET0_TXD1__ENET0_TXD1);
	mxs_iomux_setup_pad(MX28_PAD_ENET_CLK__CLKCTRL_ENET);
#endif

	// HW_PINMUXESL_0..13

	*(volatile uint32_t*)0x80018100 = 0x00000000;
	*(volatile uint32_t*)0x80018110 = 0x0000fcfc;
	*(volatile uint32_t*)0x80018120 = 0x00000000;
	*(volatile uint32_t*)0x80018130 = 0xff550000;
	*(volatile uint32_t*)0x80018140 = 0xff000000;
	*(volatile uint32_t*)0x80018150 = 0x00ff0000;
	*(volatile uint32_t*)0x80018160 = 0xffffff00;
	*(volatile uint32_t*)0x80018170 = 0x0ff0ff3e | 0x30000000; // set LCD_RESET (BANK3_PIN30) to GPIO
	*(volatile uint32_t*)0x80018180 = 0xfffc0c00;
	*(volatile uint32_t*)0x80018190 = 0x00000000;
	*(volatile uint32_t*)0x800181a0 = 0x00000000;
	*(volatile uint32_t*)0x800181b0 = 0x00000000;
	*(volatile uint32_t*)0x800181c0 = 0x00000000;
	*(volatile uint32_t*)0x800181d0 = 0x00000000;


	// HW_PINCTRLDRIVE0..19

	*(volatile uint32_t*)0x80018300 = 0x66666666;
	*(volatile uint32_t*)0x80018310 = 0x00000000;
	*(volatile uint32_t*)0x80018320 = 0x44464446;
	*(volatile uint32_t*)0x80018330 = 0x00066666;
	*(volatile uint32_t*)0x80018340 = 0x44444444;
	*(volatile uint32_t*)0x80018350 = 0x44444444;
	*(volatile uint32_t*)0x80018360 = 0x44444444;
	*(volatile uint32_t*)0x80018370 = 0x44444444;
	*(volatile uint32_t*)0x80018380 = 0x55555555;
	*(volatile uint32_t*)0x80018390 = 0x44450555;
	*(volatile uint32_t*)0x800183a0 = 0x00445555;
	*(volatile uint32_t*)0x800183b0 = 0x00004444;
	*(volatile uint32_t*)0x800183c0 = 0x44464444;
	*(volatile uint32_t*)0x800183d0 = 0x44444444;
	*(volatile uint32_t*)0x800183e0 = 0x44440444;
	*(volatile uint32_t*)0x800183f0 = 0x04444466;

	*(volatile uint32_t*)0x80018400 = 0x55455555;
	*(volatile uint32_t*)0x80018410 = 0x44444445;
	*(volatile uint32_t*)0x80018420 = 0x00040005;
	*(volatile uint32_t*)0x80018430 = 0x00000000;

	// HW_PINCTRLPULL0..6
	*(volatile uint32_t*)0x80018600 = 0x00110000;
	*(volatile uint32_t*)0x80018610 = 0x00000000;
	*(volatile uint32_t*)0x80018620 = 0x000017ff;
	*(volatile uint32_t*)0x80018630 = 0x00000020;
	*(volatile uint32_t*)0x80018640 = 0x000101df;
	*(volatile uint32_t*)0x80018650 = 0x00000000;
	*(volatile uint32_t*)0x80018660 = 0x00000000;

	// HW_PINCTRL_DOUT0..4
	*(volatile uint32_t*)0x80018700 = 0x00000000;
	*(volatile uint32_t*)0x80018710 = 0x00000000;
	*(volatile uint32_t*)0x80018720 = 0x00000000;
	*(volatile uint32_t*)0x80018730 = 0x00000030 | (1<<30); // LCD_RESET=1
	*(volatile uint32_t*)0x80018740 = 0x00000000;

	// HW_PINCTRL_DEN0..4
	*(volatile uint32_t*)0x80018b00 = 0x00000000;
	*(volatile uint32_t*)0x80018b10 = 0x00000000;
	*(volatile uint32_t*)0x80018b20 = 0x00000000;
	*(volatile uint32_t*)0x80018b30 = 0x10000230 | (1<<30); // LCD_RESET output
	*(volatile uint32_t*)0x80018b40 = 0x00000000;

	*(volatile uint32_t*)0x80018738 = 1<<5;
	mdelay(20);
	*(volatile uint32_t*)0x80018734 = 1<<5;

	return 0;
}

int board_eth_init(bd_t *bis);

int board_init(void)
{
	/* Adress of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM_1 + 0x100;

	board_eth_init((struct bd_t*)gd);

	return 0;
}

int dram_init(void)
{
//	return mxs_dram_init();
	gd->ram_size = 0x4000000;
	return 0;
}

#ifdef	CONFIG_CMD_NET

#define	MII_OPMODE_STRAP_OVERRIDE	0x16
#define	MII_PHY_CTRL1			0x1e
#define	MII_PHY_CTRL2			0x1f

int fecmxc_mii_postcall(int phy)
{
	/* KZ8031 PHY on old boards. */
	const uint32_t freq = 0x0080;

	// DUNNO ABOUT THESE...
	miiphy_write("FEC1", phy, MII_BMCR, 0x9000);
	miiphy_write("FEC1", phy, MII_OPMODE_STRAP_OVERRIDE, 0x0202);

	/// >>>> THIS IS ACTUALLY REQUIRED <<<<
	if (phy == 3)
		miiphy_write("FEC1", 3, MII_PHY_CTRL2, 0x8100 | freq);
	return 0;
}

int board_eth_init(bd_t *bis)
{
	struct mxs_clkctrl_regs *clkctrl_regs =
		(struct mxs_clkctrl_regs *)MXS_CLKCTRL_BASE;
	struct eth_device *dev;
	int ret;

	ret = cpu_eth_init(bis);
	if (ret)
		return ret;

	clrsetbits_le32(&clkctrl_regs->hw_clkctrl_enet,
		CLKCTRL_ENET_TIME_SEL_MASK | CLKCTRL_ENET_CLK_OUT_EN,
		CLKCTRL_ENET_TIME_SEL_RMII_CLK);

	writel(0x00140000, &clkctrl_regs->hw_clkctrl_enet);

	/* Reset the new PHY */
	gpio_direction_output(MX28_PAD_AUART1_TX__GPIO_3_5, 0);
	udelay(10000);
	gpio_set_value(MX28_PAD_AUART1_TX__GPIO_3_5, 1);
	udelay(10000);
	return ret;
}

#endif
