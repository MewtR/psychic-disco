#ifndef MAIN_H
#define MAIN_H
#include <iostream>
#include <unistd.h>
#include <xcb/xcb.h>
#include <xcb/composite.h>
#include <vector>
#include <algorithm>
#include <memory>
#include <Magick++.h>

std::string get_property(xcb_connection_t *, xcb_window_t, xcb_atom_t);
std::vector<int> get_children(xcb_connection_t *, xcb_window_t);
xcb_window_t get_child(xcb_connection_t*, xcb_window_t, xcb_atom_t, std::string);
std::shared_ptr<xcb_get_geometry_reply_t> get_geometry(xcb_connection_t *, xcb_window_t);
std::shared_ptr<xcb_get_image_reply_t> get_image(xcb_connection_t *, xcb_window_t, std::shared_ptr<xcb_get_geometry_reply_t>);
std::shared_ptr<xcb_composite_query_version_reply_t> get_composite_query_version(xcb_connection_t *, int, int);

#endif /* MAIN_H */
