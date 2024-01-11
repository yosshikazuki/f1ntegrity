#include "graphics.h"
#include <gtk/gtk.h>

void on_activate(GApplication *app, gpointer user_data)
{
	GtkWidget *window;
	GtkWidget *label;

	window = gtk_window_new();
	gtk_window_set_application(GTK_WINDOW (window), GTK_APPLICATION (app));
	gtk_window_set_title(GTK_WINDOW (window), "f1ntegrity");
	gtk_window_set_default_size(GTK_WINDOW (window), 400, 300);
	label = gtk_label_new("This file has been modified!\n");
	gtk_window_set_child(GTK_WINDOW (window), label);
	gtk_window_present(GTK_WINDOW (window));
}

void init_window()
{
	GtkApplication *app;

	app = gtk_application_new(NULL, G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(G_OBJECT (app), "activate", G_CALLBACK (on_activate), NULL);
	g_application_run(G_APPLICATION (app), 0, NULL);
	g_object_unref(app);
}