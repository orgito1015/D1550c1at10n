#ifndef INPUT_MOUSE_H
#define INPUT_MOUSE_H

typedef struct mouse_state {
    int x;
    int y;
    int left;
    int right;
    int middle;
} mouse_state_t;

void mouse_init(void);
int mouse_poll(mouse_state_t *state);

#endif
