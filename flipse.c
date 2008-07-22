#include <X11/Intrinsic.h>
#include <X11/Xaw/Label.h>
#include <stdio.h>
#include <string.h>
#include "flipse.h"

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
        if (isDockapp (children[i]))
            embedWindow (children[i]);
    }
}

int isDockapp( Window win ) {
    Status ok;
    char* winName;
    XClassHint hint;
    XWMHints *wmhints;
    XSizeHints sizehints;
    int isDockapp = FALSE;
    long supplied_return;

    ok = XFetchName (display, win, &winName);
    if (0 != ok) {
        if (0 != strncmp (winName, "wm", 2)) {
            XFree(winName);
            return (FALSE);
        }
    }

    ok = XGetWMNormalHints (display, win, &sizehints, &supplied_return);
    if (0 != ok) {
        printf("Got WMSizeHints, %li, %li\n", supplied_return, sizehints.flags);
        printf("USSize: %i, %i\n", sizehints.width, sizehints.height);
        printf("PSize: %i, %i\n", sizehints.width, sizehints.height);
        printf("PMinSize: %i, %i\n", sizehints.min_width, sizehints.min_height);
        printf("PMaxSize: %i, %i\n", sizehints.max_width, sizehints.max_height);
        printf("PBaseSize: %i, %i\n", sizehints.base_width, sizehints.base_height);
    }


    wmhints = XGetWMHints(display, win);
    if (NULL != wmhints) {
        printf("WmHints Flags: %li\n", wmhints->flags);
        if (wmhints->flags == (WindowGroupHint | StateHint | IconWindowHint))
            printf("Found approriate flags\n");
        XFree(wmhints);
        if (isDockapp) return (TRUE);
    }

    ok = XGetClassHint (display, win, &hint );
    if (0 != ok) {
        printf("Hint name:%s, class:%s\n",hint.res_name, hint.res_class);
        if (!strcmp(hint.res_class,"DockApp"))
            isDockapp = TRUE;
        XFree(hint.res_name);
        XFree(hint.res_class);
        if (isDockapp) return (TRUE);
    }


    return (isDockapp);
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

