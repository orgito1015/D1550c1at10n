#ifndef DRIVER_H
#define DRIVER_H
typedef struct driver {
    const char* name;
    int (*probe)(void);
    int (*init)(void);
} driver_t;
#endif
