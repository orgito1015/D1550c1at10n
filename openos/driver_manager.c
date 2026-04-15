#include "openos/driver_manager.h"

#include <stddef.h>

#define DRIVER_MANAGER_MAX_DRIVERS 16U

static const driver_t *s_drivers[DRIVER_MANAGER_MAX_DRIVERS];
static unsigned        s_count;

void driver_manager_register(const driver_t *drv)
{
    if (drv != NULL && s_count < DRIVER_MANAGER_MAX_DRIVERS) {
        s_drivers[s_count++] = drv;
    }
}

int driver_manager_probe_all(void)
{
    unsigned i;
    for (i = 0; i < s_count; ++i) {
        if (s_drivers[i]->probe != NULL && s_drivers[i]->probe() != 0) {
            return -1;
        }
    }
    return 0;
}

int driver_manager_init_all(void)
{
    unsigned i;
    for (i = 0; i < s_count; ++i) {
        if (s_drivers[i]->init != NULL && s_drivers[i]->init() != 0) {
            return -1;
        }
    }
    return 0;
}
