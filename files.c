#include <gtk/gtk.h>
#include <gio/gio.h>
#include <glib.h>
#define  FILMS_FILE "films.txt"

int main (int argc,
          char *argv[])
{

   GFile *film_file;
   GDataInputStream *stream;
   GFileInputStream *fstream;
   GError *error = NULL;
   char *line= NULL;
   gsize length;
 
   g_type_init();
   film_file = g_file_new_for_path (FILMS_FILE);
   fstream = g_file_read (film_file, NULL, &error);
   /*
   if (error != NULL)
   {
      g_print (error->message);
      g_clear_error (error);
      g_error_free (error);
   }*/

   stream = g_data_input_stream_new (G_INPUT_STREAM(fstream));
   
   while ((line = g_data_input_stream_read_line (stream, &length, NULL, &error)) != NULL){
      g_print("%s\n", line);
   }
  
   return 0;
}     
