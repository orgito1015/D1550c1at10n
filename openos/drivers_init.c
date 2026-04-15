/*
 * openos/drivers_init.c
 *
 * Boot-time driver registration and initialisation.
 *
 * openos_drivers_init() is the single kernel entry point.  It registers
 * every driver with the driver manager in dependency order (framework,
 * storage, network, display, input), then runs probe_all() followed by
 * init_all().
 *
 * Driver wrappers are defined here for init callbacks that need an
 * adapter (e.g. vbe_init requires explicit dimensions, mouse_init
 * returns void).  Probe callbacks are set to NULL where PCI enumeration
 * is not yet granular enough to distinguish individual devices; the
 * driver manager treats a NULL probe as an unconditional pass.
 */

#include "openos/driver_manager.h"
#include "openos/drivers_init.h"

#include "storage/ata.h"
#include "storage/fat.h"
#include "storage/ramdisk.h"
#include "network/rtl8139.h"
#include "display/vbe.h"
#include "input/mouse.h"
#include "input/uart.h"
#include "framework/pci.h"

/* ------------------------------------------------------------------ */
/* Init adapters                                                       */
/* ------------------------------------------------------------------ */

/* VBE init with the default boot resolution (640 × 480). */
static int vbe_init_default(void)
{
    return vbe_init(640U, 480U);
}

/* mouse_init() returns void; wrap to satisfy int (*init)(void). */
static int mouse_init_wrapper(void)
{
    mouse_init();
    return 0;
}

/* ------------------------------------------------------------------ */
/* Probe helpers                                                       */
/* ------------------------------------------------------------------ */

/*
 * NIC probe: report success only when at least one PCI device is present.
 * A real implementation would additionally match vendor/device IDs.
 */
static int rtl8139_probe(void)
{
    return pci_scan() > 0U ? 0 : -1;
}

/* ------------------------------------------------------------------ */
/* Driver table                                                        */
/* ------------------------------------------------------------------ */

static const driver_t drv_ata      = { "ata",      NULL,          ata_init         };
static const driver_t drv_ramdisk  = { "ramdisk",  NULL,          ramdisk_init     };
static const driver_t drv_fat16    = { "fat16",    NULL,          fat16_mount      };
static const driver_t drv_rtl8139  = { "rtl8139",  rtl8139_probe, rtl8139_init     };
static const driver_t drv_vbe      = { "vbe",      NULL,          vbe_init_default };
static const driver_t drv_mouse    = { "mouse",    NULL,          mouse_init_wrapper };
static const driver_t drv_uart     = { "uart",     NULL,          uart_init        };

/* ------------------------------------------------------------------ */
/* Public boot entry point                                             */
/* ------------------------------------------------------------------ */

int openos_drivers_init(void)
{
    driver_manager_register(&drv_ata);
    driver_manager_register(&drv_ramdisk);
    driver_manager_register(&drv_fat16);
    driver_manager_register(&drv_rtl8139);
    driver_manager_register(&drv_vbe);
    driver_manager_register(&drv_mouse);
    driver_manager_register(&drv_uart);

    if (driver_manager_probe_all() != 0) {
        return -1;
    }
    return driver_manager_init_all();
}
