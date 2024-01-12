#include <gtk/gtk.h>
#include "graphics.h"
#include "fparam.h"

static void clicked(GtkButton *button, GtkWindow *window)
{
	gtk_window_destroy(window);
}

static void file_open(GApplication *app, GFile **files, int n_files, char *hint)
{
	GtkWidget *window2;
	GtkWidget *scrolled_window;
	GtkWidget *text_viewer;
	GtkTextBuffer *text_buffer;
	char *contents;
	gsize length;
	char *filename;
	GError *err = NULL;

	window2 = gtk_application_window_new(GTK_APPLICATION(app));
	gtk_window_set_default_size(GTK_WINDOW(window2), 400, 300);

	scrolled_window = gtk_scrolled_window_new();
	gtk_window_set_child(GTK_WINDOW(window2), scrolled_window);

	text_viewer = gtk_text_view_new();
	text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_viewer));
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_viewer), GTK_WRAP_WORD_CHAR);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(text_viewer), FALSE);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), text_viewer);

	if (g_file_load_contents(files[0], NULL, &contents, &length, NULL, &err))
	{
		gtk_text_buffer_set_text(text_buffer, contents, length);
		g_free(contents);
		if ((filename = g_file_get_basename(files[0])) != NULL)
		{
			gtk_window_set_title(GTK_WINDOW(window2), filename);
			g_free(filename);
		}
		gtk_window_present(GTK_WINDOW(window2));
	}
	else
	{
		g_printerr("%s.\n", err->message);
		g_error_free(err);
		gtk_window_destroy(GTK_WINDOW(window2));
	}
}

static void on_activate(GApplication *app, gpointer user_data)
{
	GtkWidget *window1;
	GtkWidget *box;
	GtkWidget *label;
	GtkWidget *button1;
	GtkWidget *button2;

	window1 = gtk_application_window_new(GTK_APPLICATION(app));
	gtk_window_set_title(GTK_WINDOW(window1), "f1ntegrity");
	gtk_window_set_default_size(GTK_WINDOW(window1), 400, 300);
	
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_box_set_homogeneous(GTK_BOX(box), TRUE);
	gtk_window_set_child(GTK_WINDOW(window1), box);

	label = gtk_label_new(NULL);
	gtk_label_set_text(GTK_LABEL(label), g_strdup_printf("%s\n", MODMSG));

	button1 = gtk_button_new_with_label("Show File");
	g_signal_connect(button1, "clicked", G_CALLBACK(clicked), window1);
	
	button2 = gtk_button_new_with_label ("View Log");
	g_signal_connect(button2, "clicked", G_CALLBACK(clicked), window1);

	gtk_box_append(GTK_BOX(box), label);
	gtk_box_append(GTK_BOX(box), button1);
	gtk_box_append(GTK_BOX(box), button2);

	gtk_window_present(GTK_WINDOW(window1));

}

void init_window(int argc, char **argv)
{
	GtkApplication *app;

	app = gtk_application_new(NULL, G_APPLICATION_HANDLES_OPEN);
	g_signal_connect(G_OBJECT(app), "activate", G_CALLBACK(on_activate), NULL);
	g_signal_connect(G_OBJECT(app), "open", G_CALLBACK(file_open), NULL);
	g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);
}