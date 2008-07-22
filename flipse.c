#include <config.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Label.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "flipse.h"

#define DEFAULT_DOCKKAPP_WIDTH 64
#define DEFAULT_DOCKKAPP_HEIGHT 64

static Display *display;

int main (int argc, char **argv) {
    printAllWindows();
    return(0);
}

void printAllWindows() {

    int dummy_argc=0;
    int i;

    //Status ok;
    Widget toplevel, flipse;
    XtAppContext app;

    unsigned int children_count;
    Window root, parent, *children=NULL;

    toplevel = XtAppInitialize (
            &app, "Flipse", NULL, 0, 
            &dummy_argc, NULL, NULL, NULL, 0);
    flipse = XtCreateManagedWidget (
            "Flipse", labelWidgetClass, toplevel, NULL, 0);

    XtRealizeWidget (toplevel);
    XtMapWidget (toplevel);
    XtMapWidget (flipse);
    XSync( XtDisplay (toplevel), FALSE);

    display = XtDisplay (flipse);

    if (children != (Window *) NULL)
        XFree(children);
    XQueryTree( XtDisplay (flipse), RootWindowOfScreen( XtScreen (flipse) ),
            &root, &parent, &children, &children_count);
    for(i = 0; i < children_count; i++) {
        checkIfIsDockapp (children[i]);
    }
}

void checkIfIsDockapp( Window win ) {
    Status ok;
    char* winName;
    XWMHints *wmhints;

    wmhints = XGetWMHints(display, win);
    if (!wmhints) return;

    if (  wmhints->initial_state == WithdrawnState   ||
          wmhints->flags == (WindowGroupHint | StateHint | IconWindowHint)
       ) {
        // Seems to be a Dockapp
        ok = XFetchName (display, win, &winName);
        if (0 != ok) {
            if (0 != strncmp (winName, "wm", 2)) {
            }
            printf("New Dockapp: %s\n", winName);
            XFree(winName);
        }
    }

    return;
}

void embedWindow(Window win) {
    Status ok;
    char* winName;

    ok = XFetchName (display, win, &winName);
    if (0 != ok) {
        printf("Embed Window '%s'\n", winName);
        XFree(winName);
    }
}

