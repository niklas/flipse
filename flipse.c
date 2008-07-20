#include <X11/Intrinsic.h>
#include <X11/Xaw/Label.h>
#include <stdio.h>
#include <string.h>
#include "flipse.h"

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

    if (children != (Window *) NULL)
        XFree(children);
    XQueryTree( XtDisplay (flipse), RootWindowOfScreen( XtScreen (flipse) ),
            &root, &parent, &children, &children_count);
    for(i = 0; i < children_count; i++) {
        foundWindow ( XtDisplay (flipse), children[i]);
    }
}

void foundWindow( Display *display, Window win) {
    Status ok;
    char* winName;
    XClassHint hint;

    ok = XFetchName (display, win, &winName);
    if (0 != ok) {
        printf("Found Window '%s'", winName);
        XFree(winName);

        ok = XGetClassHint (display, win, &hint );
        if (0 != ok) {
            printf(", Hint: %s, %s\n", hint.res_name, hint.res_class);
            XFree(hint.res_name);
            XFree(hint.res_class);
        } else {
            printf("\n");
        }
    }
}

