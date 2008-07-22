#include <config.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Label.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <stdio.h>
#include <string.h>
#include "flipse.h"

#define DEFAULT_DOCKKAPP_WIDTH 64
#define DEFAULT_DOCKKAPP_HEIGHT 64
#define BORDER_WIDTH 20
#define SPACING 5

Display *display = NULL;
GtkWidget *window = NULL;
Flipse *flipse = NULL;

//static Widget toplevel, flipse;
XtAppContext app;

static void
on_destroy (GtkWidget * widget, gpointer data)
{
    gtk_main_quit ();
}

static Flipse* new_flipse() {
    flipse = g_new0 (Flipse, 1);
    flipse->dapps = NULL;
    flipse->ebox = gtk_event_box_new ();
    gtk_widget_show (GTK_WIDGET(flipse->ebox));
    flipse->align = gtk_alignment_new (0.5, 0.5, 0.0, 0.0);
    gtk_widget_show (GTK_WIDGET(flipse->align));
    gtk_container_add (GTK_CONTAINER(flipse->ebox), GTK_WIDGET(flipse->align));
    flipse->box = gtk_hbox_new(TRUE, SPACING);
    gtk_container_add (GTK_CONTAINER(flipse->align), GTK_WIDGET(flipse->box));

    return flipse;
}

void setSize() {
    gtk_window_set_default_size (GTK_WINDOW (window), 140, 70);
}

void printAllWindows() {
    int dummy_argc=0;
    int i;

    //Status ok;

    unsigned int children_count;
    Window root, parent, *children=NULL;

    if (children != (Window *) NULL)
        XFree(children);
    XQueryTree( display, GDK_ROOT_WINDOW(),
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

int main (int argc, char **argv) {
    gtk_init (&argc, &argv);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width (GTK_CONTAINER (window), BORDER_WIDTH);
    setSize();
    g_signal_connect (G_OBJECT (window), "destroy",
                  G_CALLBACK (on_destroy), NULL);
    flipse = new_flipse();
    gtk_container_add (GTK_CONTAINER(window), flipse->ebox);
    gtk_widget_show_all (window);
    display = GDK_DISPLAY();
    gtk_main ();
    printAllWindows();
    return(0);
}

