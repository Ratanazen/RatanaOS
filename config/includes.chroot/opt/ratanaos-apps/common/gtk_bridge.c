#include <gtk/gtk.h>

static void activate(GtkApplication *app, gpointer user_data) {
  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "RatanaOS GTK Bridge");
  gtk_window_set_default_size(GTK_WINDOW(window), 480, 320);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
  gtk_widget_set_margin_top(box, 24);
  gtk_widget_set_margin_bottom(box, 24);
  gtk_widget_set_margin_start(box, 24);
  gtk_widget_set_margin_end(box, 24);

  GtkWidget *title = gtk_label_new("GTK4 surface aligned with RatanaOS branding");
  gtk_widget_add_css_class(title, "title-2");
  GtkWidget *body = gtk_label_new("Use this optional bridge target to validate GTK applications against the same color system used by the Qt desktop shell.");
  gtk_label_set_wrap(GTK_LABEL(body), TRUE);

  gtk_box_append(GTK_BOX(box), title);
  gtk_box_append(GTK_BOX(box), body);
  gtk_window_set_child(GTK_WINDOW(window), box);
  gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
  GtkApplication *app = gtk_application_new("os.ratana.bridge", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  const int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;
}
