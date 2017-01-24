#ifndef __SERIAL_MXS_AUART_H
#define __SERIAL_MXS_AUART_H

/* Information about a serial port */
struct mxs_uartapp_platdata {
	struct mxs_uartapp_regs *regs;  /* address of registers in physical memory */
};

#endif /* __SERIAL_MXS_AUART_H */
