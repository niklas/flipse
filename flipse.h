#ifndef __FLIPSE_H__
#define __FLIPSE_H__

#include <gtk/gtk.h>

typedef struct _dockapp {
 GtkSocket *s;
 GdkNativeWindow i;
 GdkNativeWindow original;
 gchar *name;
 gchar *cmd;
} DockappNode;

typedef struct {
    GList *dapps;
    GtkWidget *box;
    GtkWidget *ebox;
    GtkWidget *align;
} Flipse;

void setSize();
void scanAllWindows();
void addWindow( WnckWindow * win );
int addDapp (DockappNode *dapp);
void removeDapp (DockappNode *dapp);
void releaseDapp (DockappNode *dapp);
int isDockapp( WnckWindow * win );
gchar *dockappCommand(WnckWindow * w);

#endif
