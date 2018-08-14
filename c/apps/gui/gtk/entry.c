#include <gtk/gtk.h>

int main(int argc, char *argv[])
{
  GtkWidget *window;

  GtkWidget *table;

  GtkWidget *name_label;
  GtkWidget *email_label;
  GtkWidget *password_label;

  GtkWidget *name_entry;
  GtkWidget *email_entry;
  GtkWidget *password_entry;

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_title(GTK_WINDOW(window), "Register");
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);

  table = gtk_table_new(3, 2, FALSE);
  gtk_container_add(GTK_CONTAINER(window), table);

  name_label = gtk_label_new("Name");
  email_label = gtk_label_new("Email");
  password_label = gtk_label_new("Password");

  gtk_table_attach(GTK_TABLE(table), name_label, 0, 1, 0, 1,
		   GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
  gtk_table_attach(GTK_TABLE(table), email_label, 0, 1, 1, 2,
		   GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
  gtk_table_attach(GTK_TABLE(table), password_label, 0, 1, 2, 3,
		   GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);

  name_entry = gtk_entry_new();
  email_entry = gtk_entry_new();
  password_entry = gtk_entry_new();

  gtk_table_attach(GTK_TABLE(table), name_entry, 1, 2, 0, 1,
		   GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
  gtk_table_attach(GTK_TABLE(table), email_entry, 1, 2, 1, 2,
		   GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
  gtk_table_attach(GTK_TABLE(table), password_entry, 1, 2, 2, 3,
		   GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);

  gtk_widget_show_all(window);

  g_signal_connect(window, "destroy",
		   G_CALLBACK(gtk_main_quit), NULL);

  gtk_main();

  return 0;
}
