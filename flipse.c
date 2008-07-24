#include <config.h>
#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Label.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#include <sys/signal.h>
#include "flipse.h"

#define DEFAULT_DOCKAPP_WIDTH 64
#define DEFAULT_DOCKAPP_HEIGHT 64
#define BORDER_WIDTH 0
#define SPACING 0
#define BUF_MAX 4096

Display *display = NULL;
GtkWidget *window = NULL;
WnckScreen *screen = NULL;
Flipse *flipse = NULL;

//static Widget toplevel, flipse;
XtAppContext app;

static void
on_destroy (GtkWidget * widget, gpointer data)
{
    g_list_foreach(flipse->dapps, (GFunc)releaseDapp, NULL);
    gtk_widget_destroy(GTK_WIDGET(flipse->box));
    gtk_widget_destroy(GTK_WIDGET(flipse->align));
    gtk_widget_destroy(GTK_WIDGET(flipse->ebox));
    g_list_free(flipse->dapps);
    g_free(flipse);
    gtk_main_quit ();
}

static void
on_window_opened (WnckScreen *  thescreen, WnckWindow * win, gpointer user_data) {
    if (isDockapp(win)) {
        addWindow(win);
        setSize();
    }
}

static void
on_dapp_closed (GtkSocket *socket, DockappNode *dapp) {
    removeDapp(dapp);
    setSize();
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
    gint height, width;
    gint appcount;
    if (!flipse) return;
    appcount = g_list_length(flipse->dapps);
    gtk_widget_set_size_request (GTK_WIDGET(window), (appcount * DEFAULT_DOCKAPP_WIDTH) + (2*BORDER_WIDTH) + 1, DEFAULT_DOCKAPP_HEIGHT);
}

int isDockapp( WnckWindow * win ) {
    Status ok;
    char* winName;
    XWMHints *wmhints;

    wmhints = XGetWMHints(display, wnck_window_get_xid (win));
    if (!wmhints) return;

    if (  wmhints->initial_state == WithdrawnState   ||
          wmhints->flags == (WindowGroupHint | StateHint | IconWindowHint)
       ) {
        return(TRUE);
    }

    return(FALSE);
}

/* add a Window. please check before if it is a Dockapp */
void addWindow(WnckWindow * win) {
    Status ok;
    char* winName;
    gchar* command = NULL;
    int width, height;
    DockappNode *dapp = NULL;
    XWindowAttributes attr;
    XWMHints *wmhints;
    gulong xid;

    xid = wnck_window_get_xid(win);
    wmhints = XGetWMHints(display, xid);
    ok = XFetchName (display, xid, &winName);
    if (0 != ok) {
        command = dockappCommand(win);
        if (!command) {
            XFree(wmhints);
            return;
        }
        dapp = g_new0(DockappNode, 1);
        dapp->s = GTK_SOCKET(gtk_socket_new());

        if (wmhints->initial_state == WithdrawnState && wmhints->icon_window) {
            XUnmapWindow(
                    GDK_DISPLAY_XDISPLAY(gdk_display_get_default()),
                    xid
                    );
            dapp->i = wmhints->icon_window;
   
        } else {
            dapp->i = xid; //gdk_x11_drawable_get_xid( GDK_DRAWABLE(win));
        }

        dapp->original = xid;

        if (!XGetWindowAttributes(display, dapp->i, &attr)) {
            width = DEFAULT_DOCKAPP_WIDTH;
            height = DEFAULT_DOCKAPP_HEIGHT;
        } else {
            width = attr.width;
            height = attr.height;
        }

        if (width > DEFAULT_DOCKAPP_WIDTH || height > DEFAULT_DOCKAPP_HEIGHT) {
            /* This is no dockapp, bigger than 64 pix */
            removeDapp(dapp);
            XFree(wmhints);
            return;
        }

        gtk_widget_set_size_request(GTK_WIDGET(dapp->s), width, height);
        dapp->name = g_strdup(winName);

        if (!addDapp (dapp)) 
            removeDapp (dapp);

        XFree(winName);
    }

}

int addDapp (DockappNode * dapp) {
    gtk_box_pack_start (GTK_BOX(flipse->box), GTK_WIDGET(dapp->s), FALSE, FALSE, 0);
    gtk_widget_realize (GTK_WIDGET(dapp->s));
    gtk_socket_add_id(dapp->s, dapp->i);
    gtk_widget_show(GTK_WIDGET(dapp->s));

    g_signal_connect(dapp->s, "plug-removed", G_CALLBACK(on_dapp_closed), dapp);
    
    flipse->dapps=g_list_append(flipse->dapps, dapp);
    return 1;
}

void removeDapp (DockappNode *dapp) {
    flipse->dapps = g_list_remove_all(flipse->dapps, dapp);
    gtk_widget_destroy(GTK_WIDGET(dapp->s));
    g_free(dapp->name);
    g_free(dapp->cmd);
    g_free(dapp);
}

void releaseDapp (DockappNode *dapp) {
    XReparentWindow (GDK_DISPLAY(), dapp->i, GDK_ROOT_WINDOW(), 0,0);
    XMapWindow(display, dapp->original);
}


/* find the command to (re)start the dockapp 
 * stolen from wmdock
 * */
gchar *dockappCommand(WnckWindow * w)
{
 gchar *cmd = NULL;
 int wpid = 0;
 int argc = 0;
 int fcnt, i;
 char **argv;
 FILE *procfp = NULL;
 char buf[BUF_MAX];

 XGetCommand(display, wnck_window_get_xid(w), &argv, &argc);
 if (argc > 0) {
    argv = (char **) realloc(argv, sizeof(char *) * (argc + 1));
    argv[argc] = NULL;
    cmd = g_strjoinv (" ", argv);
    XFreeStringList(argv);
 } else {
    /* Try to get the command line from the proc fs. */
    wpid = wnck_window_get_pid (w);

    if(wpid) {
       sprintf(buf, "/proc/%d/cmdline", wpid);

       procfp = fopen(buf, "r");
       
       if (procfp) {
          fcnt = read(fileno(procfp), buf, BUF_MAX);

          cmd = g_malloc(fcnt+2);
          if (!cmd) return (NULL);

          for (i = 0; i < fcnt; i++) {
           if (buf[i] == 0)
            *(cmd+i) = ' ';
           else
            *(cmd+i) = buf[i];
          }
          *(cmd+(i-1)) = 0;

          fclose(procfp);
       }
    }
 }
 
 if (!cmd) {
  /* If nothing helps fallback to the window name. */
  cmd = g_strdup(wnck_window_get_name(w));
 }

 return(cmd);
}

int main (int argc, char **argv) {
    gtk_init (&argc, &argv);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width (GTK_CONTAINER (window), BORDER_WIDTH);
    signal(SIGINT, on_destroy);
    g_signal_connect (G_OBJECT (window), "destroy",
            G_CALLBACK (on_destroy), NULL);
    flipse = new_flipse();
    gtk_container_add (GTK_CONTAINER(window), flipse->ebox);
    gtk_window_set_resizable (GTK_WINDOW(window), FALSE);
    gtk_widget_show_all (window);
    gtk_window_set_default_size (GTK_WINDOW (window), 140, 70);
    display = GDK_WINDOW_XDISPLAY(window->window);
    // FIXME find correct screen for main window
    screen = wnck_screen_get(0);
    g_signal_connect(G_OBJECT(screen), "window_opened",
            G_CALLBACK(on_window_opened), NULL);

    gtk_main ();
    return(0);
}

