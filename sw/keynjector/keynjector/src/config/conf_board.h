/**
 * \file
 *
 * \brief User board configuration template
 *
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H

#define ACCESS_MEM_TO_RAM_ENABLED

/** Board oscillator settings */
#define BOARD_FREQ_SLCK_XTAL        (32768U)
#define BOARD_FREQ_SLCK_BYPASS      (32768U)
#define BOARD_FREQ_MAINCK_XTAL      (12000000U)
#define BOARD_FREQ_MAINCK_BYPASS    (12000000U)

/** board main clock xtal startup time */
#define BOARD_OSC_STARTUP_US   15625

#define TP_RESET    PIO_PA8_IDX
#define TP_READ		PIO_PA9_IDX
#define TP_WRITE	PIO_PA10_IDX
#define TP_DC		PIO_PA11_IDX
#define TP_CS		PIO_PA25_IDX
#define TP_DATA_PORT	IOPORT_PIOA
#define TP_DATA_MASK	0x000000FF

/** HSMCI pins definition. */
/*! Number of slot connected on HSMCI interface */
#define SD_MMC_HSMCI_MEM_CNT      1
#define SD_MMC_HSMCI_SLOT_0_SIZE  4
#define PINS_HSMCI   {0x3fUL << 26, PIOA, ID_PIOA, PIO_PERIPH_C, PIO_PULLUP}
/** HSMCI MCCDA pin definition. */
#define PIN_HSMCI_MCCDA_GPIO            (PIO_PA28_IDX)
#define PIN_HSMCI_MCCDA_FLAGS           (PIO_PERIPH_C | PIO_DEFAULT)
/** HSMCI MCCK pin definition. */
#define PIN_HSMCI_MCCK_GPIO             (PIO_PA29_IDX)
#define PIN_HSMCI_MCCK_FLAGS            (PIO_PERIPH_C | PIO_DEFAULT)
/** HSMCI MCDA0 pin definition. */
#define PIN_HSMCI_MCDA0_GPIO            (PIO_PA30_IDX)
#define PIN_HSMCI_MCDA0_FLAGS           (PIO_PERIPH_C | PIO_PULLUP)
/** HSMCI MCDA1 pin definition. */
#define PIN_HSMCI_MCDA1_GPIO            (PIO_PA31_IDX)
#define PIN_HSMCI_MCDA1_FLAGS           (PIO_PERIPH_C | PIO_PULLUP)
/** HSMCI MCDA2 pin definition. */
#define PIN_HSMCI_MCDA2_GPIO            (PIO_PA26_IDX)
#define PIN_HSMCI_MCDA2_FLAGS           (PIO_PERIPH_C | PIO_PULLUP)
/** HSMCI MCDA3 pin definition. */
#define PIN_HSMCI_MCDA3_GPIO            (PIO_PA27_IDX)
#define PIN_HSMCI_MCDA3_FLAGS           (PIO_PERIPH_C | PIO_PULLUP)
/** SD/MMC card detect pin definition. */

#define PIN_HSMCI_CD {PIO_PA16, PIOA, ID_PIOA, PIO_INPUT, PIO_PULLUP, PIO_DEBOUNCE}
#define SD_MMC_0_CD_GPIO            (PIO_PA16_IDX)
#define SD_MMC_0_CD_PIO_ID          ID_PIOA
#define SD_MMC_0_CD_FLAGS           (PIO_INPUT | PIO_PULLUP)
#define SD_MMC_0_CD_DETECT_VALUE    0

#define CONF_BOARD_SD_MMC_HSMCI

/** Touchscreen controller IRQ & Busy pin definition. */
#define BOARD_ADS7843_IRQ_GPIO      PIO_PA24_IDX
#define BOARD_ADS7843_IRQ_FLAGS     PIO_INPUT | PIO_PULLUP
#define BOARD_ADS7843_IRQ_WUP_ID    (1 << 15)
#define BOARD_ADS7843_BUSY_GPIO     PIO_PA15_IDX
#define BOARD_ADS7843_BUSY_FLAGS    PIO_INPUT | PIO_PULLUP

#define CONF_BOARD_ADS7843

/** Base address of SPI peripheral connected to the touchscreen controller. */
#define BOARD_ADS7843_SPI_BASE      SPI
/** Identifier of SPI peripheral connected to the touchscreen controller. */
#define BOARD_ADS7843_SPI_ID        ID_SPI
/** Chip select connected to the touchscreen controller. */
#define BOARD_ADS7843_SPI_NPCS      2

/** SPI MISO pin definition. */
#define SPI_MISO_GPIO             (PIO_PA12_IDX)
#define SPI_MISO_FLAGS       (PIO_PERIPH_A | PIO_DEFAULT)
/** SPI MOSI pin definition. */
#define SPI_MOSI_GPIO             (PIO_PA13_IDX)
#define SPI_MOSI_FLAGS       (PIO_PERIPH_A | PIO_DEFAULT)
/** SPI SPCK pin definition. */
#define SPI_SPCK_GPIO             (PIO_PA14_IDX)
#define SPI_SPCK_FLAGS       (PIO_PERIPH_A | PIO_DEFAULT)

#define SPI_NPCS2_PB2_GPIO        (PIO_PB2_IDX)
#define SPI_NPCS2_PB2_FLAGS       (PIO_PERIPH_B | PIO_DEFAULT)

#endif // CONF_BOARD_H
