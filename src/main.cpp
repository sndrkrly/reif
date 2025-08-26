#include <iostream>
#include "renderer/window.h"

int main() {
    window _window;
    _window.create("test", 800, 600);

    while (!_window.should_close()) {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
        _window.swap_buffers();
        _window.poll_events();
    }

    _window.destroy();
    return 0;
}
