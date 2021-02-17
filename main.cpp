#include "main.h"

int main()
{
    xcb_connection_t *c;
    xcb_screen_t *screen;
    xcb_window_t win;
    const xcb_setup_t* setup;

    // open the connection to the X server 
    c = xcb_connect(NULL, NULL);
    setup = xcb_get_setup(c);
    // Get the first screen 
    screen = xcb_setup_roots_iterator(setup).data;

    int firefox = get_child(c, screen->root, XCB_ATOM_WM_CLASS, "Navigator");
    std::cout << "Firefox window id is: "<< firefox << std::endl;
    /*
    std::vector<int> children = get_children(c, screen->root);
    std::for_each(begin(children),end(children), [c](int child){
            std::cout << "child window = " << child << std::endl;
            get_property(c, child, XCB_ATOM_WM_CLASS);
            });
            */
    //print_wm_name(c, screen->root);
    /*
    // Ask for our window's Id
    win = xcb_generate_id(c);
    // Create the window 
    xcb_create_window(c,
                    XCB_COPY_FROM_PARENT,
                    win,
                    screen->root, // gives the root I suppose
                    0,0,
                    150,150,
                    10,
                    XCB_WINDOW_CLASS_INPUT_OUTPUT,
                    screen->root_visual,
                    0, NULL);
    // Map the window on the screen (this is what makes it show up)
    xcb_map_window(c,win);
    // Make sure commands are sent before we pause, so window is shown 
    xcb_flush(c);

    pause(); // hold client until Ctrl-C
    */

    xcb_disconnect(c);
    return 0;
}

std::string get_property(xcb_connection_t *c, xcb_window_t window, xcb_atom_t property)
{
    std::string result;
    xcb_get_property_cookie_t cookie;
    xcb_get_property_reply_t* reply;

    // These atoms are predefined in the X11 protocol. 
    //xcb_atom_t property = XCB_ATOM_WM_NAME;
    //xcb_atom_t property = XCB_ATOM_WM_CLASS;
    xcb_atom_t type = XCB_ATOM_STRING;

    cookie = xcb_get_property(c, 0, window, property, type, 0, 100);

    if ((reply = xcb_get_property_reply(c, cookie, NULL)))
    {
        int len = xcb_get_property_value_length(reply);
        if (len == 0 && reply->length == 0)
        {
            printf("Doesn't the property you're looking for \n");
            free(reply);
            return result;
        }
        result = std::string((char*) xcb_get_property_value(reply));
        printf("Property is %.*s\n", len, result.c_str());
    }
    free(reply);
    return result;
}

std::vector<int> get_children(xcb_connection_t *c, xcb_window_t window)
{
    xcb_query_tree_cookie_t cookie;
    xcb_query_tree_reply_t* reply;

    cookie = xcb_query_tree(c, window);
    std::vector<int> offspring;
    if((reply = xcb_query_tree_reply(c, cookie, NULL)))
    {
        printf("root = 0x%08x\n", reply->root);
        printf("parent = 0x%08x\n", reply->parent);

        xcb_window_t *children = xcb_query_tree_children(reply);
        for (int i = 0; i < xcb_query_tree_children_length(reply); i++)
        {
            offspring.push_back(children[i]);
        }
        free(reply);
    }
    return offspring;
}
xcb_window_t get_child(xcb_connection_t *connection, xcb_window_t window, xcb_atom_t property, std::string match)
{
    xcb_window_t child;
    std::string m;
    std::vector<int> children = get_children(connection, window);
    std::for_each(begin(children),end(children), [connection, property, &m, match, &child](int c){
            std::cout << "child window = " << c << std::endl;
            m = get_property(connection, c, property);
            if(m == match){
            child = c;
            return;
            }
            });

    return child;
}
