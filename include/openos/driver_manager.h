#ifndef OPENOS_DRIVER_MANAGER_H
#define OPENOS_DRIVER_MANAGER_H

/*
 * OpenOS driver manager
 *
 * Maintains an ordered table of driver_t entries.  At boot the kernel calls
 * driver_manager_probe_all() followed by driver_manager_init_all() to
 * discover and activate every registered driver.
 */

#include "framework/driver.h"

/*
 * Register a driver.  Drivers are probed and initialised in registration
 * order.  The pointer must remain valid for the lifetime of the system.
 *
 * At most 16 drivers may be registered (DRIVER_MANAGER_MAX_DRIVERS in
 * driver_manager.c).  Increase that constant if the platform requires
 * more entries.
 */
void driver_manager_register(const driver_t *drv);

/*
 * Call probe() on every registered driver.
 * Returns 0 if all probes succeed (or have no probe callback).
 * Returns -1 on the first failure and stops processing.
 */
int driver_manager_probe_all(void);

/*
 * Call init() on every registered driver.
 * Returns 0 if all inits succeed (or have no init callback).
 * Returns -1 on the first failure and stops processing.
 */
int driver_manager_init_all(void);

#endif /* OPENOS_DRIVER_MANAGER_H */
