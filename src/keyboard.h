#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__


#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define N_ROWS 2
#define N_COLS 13

G_BEGIN_DECLS

#define KEYBOARD_TYPE            (keyboard_get_type ())
#define KEYBOARD(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), KEYBOARD_TYPE, Keyboard))
#define KEYBOARD_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), KEYBOARD_TYPE, KeyboardClass))
#define IS_KEYBOARD(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KEYBOARD_TYPE))
#define IS_KEYBOARD_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), KEYBOARD_TYPE))


typedef struct _Keyboard       Keyboard;
typedef struct _KeyboardClass  KeyboardClass;

struct _Keyboard
{
  GtkTable parent;
  GtkWidget *button[N_ROWS][N_COLS];
  /*private*/
};

struct _KeyboardClass
{
  GtkTableClass parent_class;

  void (* key_clicked) (Keyboard *keyboard, const gchar *key_name);
};

/* For the user. */
GType          keyboard_get_type        (void);
GtkWidget*     keyboard_new             (void);
void           keyboard_set_sensitive   (Keyboard *keyboard, gboolean sensitive);

G_END_DECLS

#endif /* __KEYBOARD_H__ */
