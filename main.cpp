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

    xcb_window_t firefox = get_child(c, screen->root, XCB_ATOM_WM_CLASS, "Navigator");
    std::cout << "Firefox window id is: "<< firefox << std::endl;
    std::shared_ptr<xcb_get_geometry_reply_t> geometry = get_geometry(c,firefox);
    std::cout << "Window's width: " << geometry->width << std::endl;
    std::cout << "Window's height: " << geometry->height << std::endl;
    std::cout << "Window's x: " << geometry->x << std::endl;
    std::cout << "Window's y: " << geometry->y << std::endl;
    
    //xcb_composite_redirect_window(c, firefox, XCB_COMPOSITE_REDIRECT_AUTOMATIC);
    std::shared_ptr<xcb_composite_query_version_reply_t> version = get_composite_query_version(c, 1, 14);
    std::cout << "Version is " << version->major_version << "." << version->minor_version << std::endl;

    // create a pixmap
    //xcb_pixmap_t ff_pixmap = xcb_generate_id(c);
    //xcb_composite_name_window_pixmap(c, firefox, ff_pixmap);

    std::shared_ptr<xcb_get_image_reply_t> image = get_image(c, firefox, geometry);
    // image length
    int len = xcb_get_image_data_length(image.get());
    std::cout << "Length of image???? " << len << std::endl;
    // Image data. Not something that can be freed, lives on the stack apparently
    std::shared_ptr<u_int8_t> data (xcb_get_image_data(image.get()), [](uint8_t*){}); // empty deleter
    // Image end
    xcb_generic_iterator_t end = xcb_get_image_data_end(image.get());
    std::cout << "Printing out my pixels yall" << std::endl;
    
    uint8_t* it = data.get();
    while(it != end.data)
    {
        std::cout << static_cast<int>(*it) << " ";
        it++;
    }
    
    xcb_disconnect(c);

    return 0;
}

std::string get_property(xcb_connection_t *c, xcb_window_t window, xcb_atom_t property)
{
    std::string result;
    xcb_get_property_cookie_t cookie;
    std::shared_ptr<xcb_get_property_reply_t> reply;

    // These atoms are predefined in the X11 protocol. 
    //xcb_atom_t property = XCB_ATOM_WM_NAME;
    //xcb_atom_t property = XCB_ATOM_WM_CLASS;
    xcb_atom_t type = XCB_ATOM_STRING;

    cookie = xcb_get_property(c, 0, window, property, type, 0, 100);

    if ((reply = std::shared_ptr<xcb_get_property_reply_t>(xcb_get_property_reply(c, cookie, nullptr), free)))
    {
        int len = xcb_get_property_value_length(reply.get());
        if (len == 0 && reply->length == 0)
        {
            printf("Doesn't the property you're looking for \n");
            return result;
        }
        result = std::string((char*) xcb_get_property_value(reply.get()));
        printf("Property is %.*s\n", len, result.c_str());
    }
    return result;
}

std::vector<int> get_children(xcb_connection_t *c, xcb_window_t window)
{
    xcb_query_tree_cookie_t cookie;
    std::shared_ptr<xcb_query_tree_reply_t> reply;

    cookie = xcb_query_tree(c, window);
    std::vector<int> offspring;
    if((reply = std::shared_ptr<xcb_query_tree_reply_t>(xcb_query_tree_reply(c, cookie, nullptr), free)))
    {
        printf("root = 0x%08x\n", reply->root);
        printf("parent = 0x%08x\n", reply->parent);

        xcb_window_t *children = xcb_query_tree_children(reply.get());
        int len = xcb_query_tree_children_length(reply.get());
        for (int i = 0; i < len; i++)
        {
            offspring.push_back(children[i]);
        }
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

std::shared_ptr<xcb_get_image_reply_t> get_image(xcb_connection_t *connection, xcb_window_t window,std::shared_ptr<xcb_get_geometry_reply_t> geometry)
{
    xcb_get_image_cookie_t cookie = xcb_get_image(connection, XCB_IMAGE_FORMAT_Z_PIXMAP, window, 0, 0, geometry->width, geometry->height, static_cast<uint32_t>(~0));
    return std::shared_ptr<xcb_get_image_reply_t>(xcb_get_image_reply(connection, cookie, nullptr), free);
}

std::shared_ptr<xcb_composite_query_version_reply_t> get_composite_query_version(xcb_connection_t *connection, int major, int minor)
{
    
    xcb_composite_query_version_cookie_t comp_ver_cookie = xcb_composite_query_version(connection, major, minor);

    return std::shared_ptr<xcb_composite_query_version_reply_t>(xcb_composite_query_version_reply(connection, comp_ver_cookie, nullptr), free);

}
