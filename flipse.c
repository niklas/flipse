#include <config.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Label.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <stdio.h>
#include <string.h>
#include "flipse.h"

#define DEFAULT_DOCKAPP_WIDTH 64
#define DEFAULT_DOCKAPP_HEIGHT 64
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

static void
on_window_opened (GtkWidget * widget, gpointer data)
{
    printAllWindows();
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

    display = GDK_WINDOW_XDISPLAY(window->window);

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
            embedWindow(win);
            XFree(winName);
        }
    }

    return;
}

void embedWindow(Window win) {
    Status ok;
    char* winName;
    int width, height;
    DockappNode *dapp = NULL;
    XWindowAttributes attr;
    XWMHints *wmhints;

    wmhints = XGetWMHints(display, win);
    ok = XFetchName (display, win, &winName);
    if (0 != ok) {
        dapp = g_new0(DockappNode, 1);
        dapp->s = GTK_SOCKET(gtk_socket_new());

        if (wmhints->initial_state == WithdrawnState && wmhints->icon_window) {
            XUnmapWindow(
                    GDK_DISPLAY_XDISPLAY(gdk_display_get_default()),
                    win
                    );
            dapp->i = wmhints->icon_window;
   
        } else {
            dapp->i = win; //gdk_x11_drawable_get_xid( GDK_DRAWABLE(win));
        }

        if (!XGetWindowAttributes(display, dapp->i, &attr)) {
            width = DEFAULT_DOCKAPP_WIDTH;
            height = DEFAULT_DOCKAPP_HEIGHT;
        } else {
            width = attr.width;
            height = attr.height;
        }

        gtk_widget_set_size_request(GTK_WIDGET(dapp->s), width, height);
        dapp->name = g_strdup(winName);

        gtk_box_pack_start (GTK_BOX(flipse->box), GTK_WIDGET(dapp->s), FALSE, FALSE, 0);
        gtk_widget_realize (dapp->s);
        gtk_socket_add_id(dapp->s, dapp->i);
        gtk_widget_show(GTK_WIDGET(dapp->s));
        
        flipse->dapps=g_list_append(flipse->dapps, dapp);
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
    g_signal_connect(G_OBJECT (window), "map", 
            G_CALLBACK(on_window_opened), NULL);
    flipse = new_flipse();
    gtk_container_add (GTK_CONTAINER(window), flipse->ebox);
    gtk_widget_show_all (window);
    display = GDK_WINDOW_XDISPLAY(window->window);
    gtk_main ();
    return(0);
}

