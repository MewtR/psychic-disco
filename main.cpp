#include "main.h"
#include <xcb/composite.h>
#include <xcb/xproto.h>

int main(int argc, char **argv)
{
    xcb_connection_t *c;
    xcb_screen_t *screen;
    xcb_window_t win;
    const xcb_setup_t* setup;

    // 1) Time it to see how long it takes
    // 2) Can it take scrot of ff on a different workspace
    // Initialize ImageMagick lib
    Magick::InitializeMagick(*argv);
    // open the connection to the X server 
    c = xcb_connect(nullptr, nullptr);
    setup = xcb_get_setup(c);
    // Get the first screen 
    screen = xcb_setup_roots_iterator(setup).data;

    xcb_window_t firefox = get_child(c, screen->root, XCB_ATOM_WM_CLASS, "Navigator");
    // request window redirect (draw offscreen)
    xcb_composite_redirect_window(c, firefox, XCB_COMPOSITE_REDIRECT_AUTOMATIC);
    std::cout << "Firefox window id is: "<< firefox << std::endl;
    std::shared_ptr<xcb_get_geometry_reply_t> geometry = get_geometry(c,firefox);
    std::cout << "Window's x: " << geometry->x << std::endl;
    std::cout << "Window's y: " << geometry->y << std::endl;

    
    //xcb_composite_redirect_window(c, firefox, XCB_COMPOSITE_REDIRECT_AUTOMATIC);
    std::shared_ptr<xcb_composite_query_version_reply_t> version = get_composite_query_version(c, 1, 14);
    std::cout << "Version is " << version->major_version << "." << version->minor_version << std::endl;

    // create a pixmap
    xcb_pixmap_t ff_pixmap = xcb_generate_id(c);
    xcb_composite_name_window_pixmap(c, firefox, ff_pixmap);

    std::shared_ptr<xcb_get_image_reply_t> image = get_image(c, ff_pixmap, geometry);
    // image length
    int len = xcb_get_image_data_length(image.get());
    // Image data. Not something that can be freed, lives on the stack apparently
    std::shared_ptr<u_int8_t> data (xcb_get_image_data(image.get()), [](uint8_t*){}); // empty deleter
    // Image end
    xcb_generic_iterator_t end = xcb_get_image_data_end(image.get());
    //std::cout << "Printing out my pixels yall" << std::endl;
    
    /*
    uint8_t* it = data.get();
    while(it != end.data)
    {
        std::cout << static_cast<int>(*it) << " ";
        it++;
    }
    std::cout << std::endl;
    
    std::cout << "Window's width: " << geometry->width << std::endl;
    std::cout << "Window's height: " << geometry->height << std::endl;
    std::cout << "Length of image " << len << std::endl;
    */

    Magick::Image final_image;
    final_image.read(geometry->width, geometry->height, "BGRA", Magick::CharPixel, data.get()); // Can't use 'it' here because I incremented it to print
    final_image.write("firefox.png");

    xcb_free_pixmap(c, ff_pixmap);

    xcb_disconnect(c);

    return 0;
}

