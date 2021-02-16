#include "main.h"

int main()
{
    xcb_connection_t *c;
    xcb_screen_t *screen;
    xcb_window_t win;
    const xcb_setup_t* setup;

    /* open the connection to the X server */
    c = xcb_connect(NULL, NULL);
    setup = xcb_get_setup(c);
    /* Get the first screen */
    screen = xcb_setup_roots_iterator(setup).data;

    /* Ask for our window's Id*/
    win = xcb_generate_id(c);
    /* Create the window */
    xcb_create_window(c,
                    XCB_COPY_FROM_PARENT,
                    win,
                    screen->root,
                    0,0,
                    150,150,
                    10,
                    XCB_WINDOW_CLASS_INPUT_OUTPUT,
                    screen->root_visual,
                    0, NULL);
    /* Map the window on the screen (this is what makes it show up)*/
    xcb_map_window(c,win);
    /* Make sure commands are sent before we pause, so window is shown */
    xcb_flush(c);

    pause(); // hold client until Ctrl-C

    return 0;
}
