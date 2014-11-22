/**
 * \file
 *
 * \brief User board initialization template
 *
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

#define IRQ_PRIOR_PIO    0

void board_init(void)
{
	
	pmc_enable_periph_clk(ID_PIOA);
	pmc_enable_periph_clk(ID_PIOB);
	
	pio_handler_set_priority(PIOA, PIOA_IRQn, IRQ_PRIOR_PIO);
	pio_handler_set_priority(PIOB, PIOB_IRQn, IRQ_PRIOR_PIO);
	
#if defined (CONF_BOARD_SD_MMC_HSMCI)
	/* Configure HSMCI pins */
	gpio_configure_pin(PIN_HSMCI_MCCDA_GPIO, PIN_HSMCI_MCCDA_FLAGS);
	gpio_configure_pin(PIN_HSMCI_MCCK_GPIO, PIN_HSMCI_MCCK_FLAGS);
	gpio_configure_pin(PIN_HSMCI_MCDA0_GPIO, PIN_HSMCI_MCDA0_FLAGS);
	gpio_configure_pin(PIN_HSMCI_MCDA1_GPIO, PIN_HSMCI_MCDA1_FLAGS);
	gpio_configure_pin(PIN_HSMCI_MCDA2_GPIO, PIN_HSMCI_MCDA2_FLAGS);
	gpio_configure_pin(PIN_HSMCI_MCDA3_GPIO, PIN_HSMCI_MCDA3_FLAGS);

	/* Configure SD/MMC card detect pin */
	gpio_configure_pin(SD_MMC_0_CD_GPIO, SD_MMC_0_CD_FLAGS);
#endif
	
#ifdef CONF_BOARD_ADS7843
	/* Configure Touchscreen SPI pins */
	gpio_configure_pin(BOARD_ADS7843_IRQ_GPIO,BOARD_ADS7843_IRQ_FLAGS);
	gpio_configure_pin(BOARD_ADS7843_BUSY_GPIO, BOARD_ADS7843_BUSY_FLAGS);
	gpio_configure_pin(SPI_MISO_GPIO, SPI_MISO_FLAGS);
	gpio_configure_pin(SPI_MOSI_GPIO, SPI_MOSI_FLAGS);
	gpio_configure_pin(SPI_SPCK_GPIO, SPI_SPCK_FLAGS);
	gpio_configure_pin(SPI_NPCS2_PB2_GPIO, SPI_NPCS2_PB2_FLAGS);
#endif
	
	//// Display
	ioport_init();
	// Reset pin
	ioport_set_pin_dir(TP_RESET, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(TP_RESET, true);
	// Read pin
	ioport_set_pin_dir(TP_READ, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(TP_READ, true);
	// Write pin
	ioport_set_pin_dir(TP_WRITE, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(TP_WRITE, false);
	// D/C pin
	ioport_set_pin_dir(TP_DC, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(TP_DC, false);
	// CS pin
	ioport_set_pin_dir(TP_CS, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(TP_CS, false);
	// data pins
	ioport_set_port_dir(TP_DATA_PORT, TP_DATA_MASK, IOPORT_DIR_OUTPUT);
	//ioport_set_port_mode(TP_DATA_PORT, TP_DATA_MASK, IOPORT_MODE_OPEN_DRAIN | IOPORT_MODE_PULLUP);
	
	ioport_set_pin_level(TP_RESET, false);
	delay_ms(50);
	ioport_set_pin_level(TP_RESET, true);
	delay_ms(50);
	
}
