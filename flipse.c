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

    Widget toplevel, flipse;
    XtAppContext app;

    unsigned int children_count;
    Window root, parent, *children=NULL;
    char *windowname;
    

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
    XQueryTree( XtDisplay (flipse), RootWindowOfScreen( XtScreen(flipse)),
            &root, &parent, &children, &children_count);
    for(i = 0; i < children_count; i++) {
        if (0 != XFetchName (XtDisplay (flipse), children[i], &windowname)) {
            printf("Found Window '%s'\n", windowname);
            XFree(windowname);
        }
    }
}

