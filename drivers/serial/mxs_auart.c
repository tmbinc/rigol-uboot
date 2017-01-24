/*
 * Freescale i.MX23/i.MX28 AUART driver
 *
 * Copyright (C) 2013 Andreas Wass <andreas.wass@dalelven.com>
 *
 * Based on the MXC serial driver:
 *
 * (c) 2007 Sascha Hauer <s.hauer@pengutronix.de>
 *
 * Further based on the Linux mxs-auart.c driver:
 *
 * Freescale STMP37XX/STMP38X Application UART drkiver
 * Copyright 2008-2010 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <asm/io.h>
#include <serial.h>
#include <linux/compiler.h>
#include <dm/device.h>
#include <dm/platform_data/serial_mxs_auart.h>
#include <asm/arch/regs-base.h>
#include <asm/arch/regs-uartapp.h>
#include <asm/arch/sys_proto.h>

DECLARE_GLOBAL_DATA_PTR;

/* AUART clock always supplied by XTAL and always 24MHz */
#define MXS_AUART_CLK 24000000

/**
 * Sets the baud rate and settings.
 * The settings are: 8 data bits, no parit and 1 stop bit.
 */
static int mxs_auart_setbrg(struct udevice *dev, int baudrate)
{
	struct mxs_uartapp_platdata *plat = dev->platdata;
	struct mxs_uartapp_regs *regs = plat->regs;
	u32 div;
	u32 linectrl = 0;

	/*
	 * From i.MX28 datasheet:
	 * div is calculated by calculating UARTCLK*32/baudrate, rounded to int
	 * div must be between 0xEC and 0x003FFFC0 inclusive
	 * Lowest 6 bits of div goes in BAUD_DIVFRAC part of LINECTRL register
	 * Next 16 bits goes in BAUD_DIVINT part of LINECTRL register
	 */
	div = (MXS_AUART_CLK * 32) / baudrate;
	if (div < 0xEC || div > 0x003FFFC0)
		return -EINVAL;

	linectrl |= ((div & UARTAPP_LINECTRL_EXTRACT_BAUD_DIVFRAC_MASK) <<
		UARTAPP_LINECTRL_BAUD_DIVFRAC_OFFSET) &
		UARTAPP_LINECTRL_BAUD_DIVFRAC_MASK;
	linectrl |= ((div >> UARTAPP_LINECTRL_EXTRACT_BAUD_DIVINT_OFFSET) <<
		UARTAPP_LINECTRL_BAUD_DIVINT_OFFSET) &
		UARTAPP_LINECTRL_BAUD_DIVINT_MASK;

	/* Word length: 8 bits */
	linectrl |= UARTAPP_LINECTRL_WLEN_8BITS;

	/* Enable FIFOs. */
	linectrl |= UARTAPP_LINECTRL_FEN_MASK;

	/* Write above settings, no parity, 1 stop bit */
	writel(linectrl, &regs->hw_uartapp_linectrl);

	return 0;
}

static int mxs_auart_putc(struct udevice *dev, const char ch)
{
	struct mxs_uartapp_platdata *plat = dev->platdata;
	struct mxs_uartapp_regs *regs = plat->regs;
	if (readl(&regs->hw_uartapp_stat) & UARTAPP_STAT_TXFF_MASK)
		return -EAGAIN;

	writel(ch, &regs->hw_uartapp_data);
}

static int mxs_auart_pending(struct udevice *dev, bool input)
{
	struct mxs_uartapp_platdata *plat = dev->platdata;
	struct mxs_uartapp_regs *regs = plat->regs;
	if (input)
	{
		/* Checks if receive FIFO is empty */
		return !(readl(&regs->hw_uartapp_stat) & UARTAPP_STAT_RXFE_MASK);
	} else
	{
		/* Checks if transmit FIFO is full */
		return !(readl(&regs->hw_uartapp_stat) & UARTAPP_STAT_TXFF_MASK);
	}
}

static int mxs_auart_probe(struct udevice *dev)
{
	struct mxs_uartapp_platdata *plat = dev->platdata;
	struct mxs_uartapp_regs *regs = plat->regs;

	/* Reset everything */
	mxs_reset_block(&regs->hw_uartapp_ctrl0_reg);
	/* Disable interrupts */
	writel(0, &regs->hw_uartapp_intr);
	/* Set baud rate and settings */
	serial_setbrg();
	/* Disable RTS and CTS, ignore LINECTRL2 register */
	writel(UARTAPP_CTRL2_RTSEN_MASK |
			UARTAPP_CTRL2_CTSEN_MASK |
			UARTAPP_CTRL2_USE_LCR2_MASK,
			&regs->hw_uartapp_ctrl2_clr);
	/* Enable receiver, transmitter and UART */
	writel(UARTAPP_CTRL2_RXE_MASK |
			UARTAPP_CTRL2_TXE_MASK |
			UARTAPP_CTRL2_UARTEN_MASK,
			&regs->hw_uartapp_ctrl2_set);

	return 0;
}

static int mxs_auart_getc(struct udevice *dev)
{
	struct mxs_uartapp_platdata *plat = dev->platdata;
	struct mxs_uartapp_regs *regs = plat->regs;

	if (readl(&regs->hw_uartapp_stat) & UARTAPP_STAT_RXFE_MASK)
		return -EAGAIN;

	return readl(&regs->hw_uartapp_data) & 0xFF;
}

static const struct dm_serial_ops mxs_auart_ops = {
	.putc = mxs_auart_putc,
	.pending = mxs_auart_pending,
	.getc = mxs_auart_getc,
	.setbrg = mxs_auart_setbrg,
};

#if CONFIG_IS_ENABLED(OF_CONTROL)
static int mxs_auart_ofdata_to_platdata(struct udevice *dev)
{
	struct mxs_uartapp_platdata *plat = dev->platdata;
	struct mxs_uartapp_regs *regs = plat->regs;
	fdt_addr_t addr;

	addr = dev_get_addr(dev);
	if (addr == FDT_ADDR_T_NONE)
		return -EINVAL;

	plat->regs = (struct mxs_uartapp_regs *)addr;
	return 0;
}


static const struct udevice_id mxs_auart_ids[] = {
        {
                .compatible = "fsl,imx28-auart",
        }, {
                .compatible = "fsl,imx23-auart",
        }, { /* sentinel */ }
};
#endif

U_BOOT_DRIVER(mxs_auart) = {
	.name	= "mxs_auart_serial",
	.id	= UCLASS_SERIAL,
#if CONFIG_IS_ENABLED(OF_CONTROL)
	.of_match = mxs_auart_ids,
	.ofdata_to_platdata = mxs_auart_ofdata_to_platdata,
	.platdata_auto_alloc_size = sizeof(struct mxs_uartapp_platdata),
#endif
	.probe = mxs_auart_probe,
	.ops	= &mxs_auart_ops,
	.flags = DM_FLAG_PRE_RELOC,
};
