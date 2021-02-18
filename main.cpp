#include "main.h"

int main()
{
    xcb_connection_t *c;
    xcb_screen_t *screen;
    xcb_window_t win;
    const xcb_setup_t* setup;

    // open the connection to the X server 
    c = xcb_connect(nullptr, nullptr);
    setup = xcb_get_setup(c);
    // Get the first screen 
    screen = xcb_setup_roots_iterator(setup).data;

    int firefox = get_child(c, screen->root, XCB_ATOM_WM_CLASS, "Navigator");
    std::cout << "Firefox window id is: "<< firefox << std::endl;
    std::shared_ptr<xcb_get_geometry_reply_t> geometry = get_geometry(c,firefox);
    std::cout << "Window's width: " << geometry->width << std::endl;
    std::cout << "Window's height: " << geometry->height << std::endl;
    std::cout << "Window's x: " << geometry->x << std::endl;
    std::cout << "Window's y: " << geometry->y << std::endl;
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

    if ((reply = xcb_get_property_reply(c, cookie, nullptr)))
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
    if((reply = xcb_query_tree_reply(c, cookie, nullptr)))
    {
        printf("root = 0x%08x\n", reply->root);
        printf("parent = 0x%08x\n", reply->parent);

        xcb_window_t *children = xcb_query_tree_children(reply);
        int len = xcb_query_tree_children_length(reply);
        for (int i = 0; i < len; i++)
        {
            offspring.push_back(children[i]);
        }
    }
    free(reply);
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
std::shared_ptr<xcb_get_geometry_reply_t> get_geometry(xcb_connection_t *connection, xcb_window_t window)
{
    /*
     * The assumption is that the parent window is basically root so I shouldn't need to translate
     * as described in this tutorial https://www.x.org/releases/current/doc/libxcb/tutorial/index.html#wingetinfo
     * Output seems to match that of xwininfo -root -all -int
     */
    //xcb_drawable_t and xcb_window_t are basically the same. Typedefs of uint32_t
    xcb_get_geometry_cookie_t cookie = xcb_get_geometry(connection,window); 
    //Construct a shared pointer with 'free' as the deleter
    return std::shared_ptr<xcb_get_geometry_reply_t>  (xcb_get_geometry_reply(connection,cookie, nullptr), free);
}
