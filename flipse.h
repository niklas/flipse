#ifndef __FLIPSE_H__
#define __FLIPSE_H__

#include <gtk/gtk.h>

typedef struct _dockapp {
 GtkSocket *s;
 GdkNativeWindow i;
 gchar *name;
 gchar *cmd;
} DockappNode;

typedef struct {
    GList *dapps;
    GtkWidget *box;
    GtkWidget *ebox;
    GtkWidget *align;
} Flipse;

void scanAllWindows();
void embedWindow( Window win );
void checkIfIsDockapp( Window win );

#endif
