#include <X11/Xlib.h>
#include <stdio.h>

int main() {
    Display *display;
    Window window;
    XEvent event;
    int isRunning

    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        return 1;
    }

    int screen = DefaultScreen(display);
    window = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, 800, 600, 1,
                                 BlackPixel(display, screen), WhitePixel(display, screen));

    XSelectInput(display, window, ExposureMask | KeyPressMask | PointerMotionMask);
    XMapWindow(display, window);

    GC gc = XCreateGC(display, window, 0, NULL);
    XSetForeground(display, gc, BlackPixel(display, screen));

    while (isRunning) {
        XNextEvent(display, &event);
        switch (event.type) {
            case MotionNotify:
                XDrawPoint(display, window, gc, event.xmotion.x, event.xmotion.y);
                XFlush(display);
                break;
            case KeyPress:
                isRunning = 0;
                break;
        }
    }

    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}
