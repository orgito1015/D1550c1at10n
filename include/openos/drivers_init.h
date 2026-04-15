#ifndef OPENOS_DRIVERS_INIT_H
#define OPENOS_DRIVERS_INIT_H

/*
 * openos_drivers_init()
 *
 * Registers all platform drivers with the driver manager and runs
 * probe_all() followed by init_all().  Call once during early kernel
 * boot after the HAL has been set up.
 *
 * Returns 0 on success, -1 if any probe or init step fails.
 */
int openos_drivers_init(void);

#endif /* OPENOS_DRIVERS_INIT_H */
