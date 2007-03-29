#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "ConfigDialog.h"
#include "defines.h"
#include "cryptkeeper.h"

static gboolean on_window_close (GtkWidget *window, GdkEvent *event, ConfigDialog *w)
{
	w->Hide ();
	return TRUE;
}

static void on_close_clicked (GtkButton *button, ConfigDialog *w)
{
	w->Hide ();
}

ConfigDialog::ConfigDialog ()
{
	GtkWidget *w, *hbox;

	m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	g_signal_connect(G_OBJECT(m_window), "delete-event", G_CALLBACK(on_window_close), this);

	GtkWidget *vbox = gtk_vbox_new (FALSE, UI_SPACING);
	gtk_container_add (GTK_CONTAINER (m_window), vbox);

	{
		hbox = gtk_hbox_new (FALSE, UI_SPACING);
		gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, UI_SPACING);

		w = gtk_image_new_from_stock (GTK_STOCK_PREFERENCES, GTK_ICON_SIZE_DIALOG);
		gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, UI_SPACING);

		w = gtk_label_new (NULL);
		gtk_label_set_markup (GTK_LABEL (w), "<span weight=\"bold\" size=\"large\">Cryptkeeper Preferences</span>");
		gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, UI_SPACING);
	}

	hbox = gtk_hbox_new (FALSE, UI_SPACING);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, UI_SPACING);

	GtkWidget *label = gtk_label_new ("File browser:");
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, UI_SPACING);

	m_filemanager_entry = gtk_entry_new ();
	gtk_entry_set_text (GTK_ENTRY (m_filemanager_entry), config_filemanager);
	gtk_box_pack_start (GTK_BOX (hbox), m_filemanager_entry, FALSE, FALSE, UI_SPACING);

	GtkWidget *buttonBox = gtk_hbutton_box_new ();
	gtk_box_pack_end (GTK_BOX (vbox), buttonBox, FALSE, FALSE, UI_SPACING);

	GtkWidget *button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
	g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (on_close_clicked), this);
	gtk_box_pack_end (GTK_BOX (buttonBox), button, FALSE, FALSE, UI_SPACING);
}

ConfigDialog::~ConfigDialog ()
{
	gtk_widget_destroy (m_window);
}

void ConfigDialog::Hide ()
{
	free (config_filemanager);
	config_filemanager = strdup (gtk_entry_get_text (GTK_ENTRY (m_filemanager_entry)));
	write_config ();
	gtk_widget_hide (m_window);
}

void ConfigDialog::Show ()
{
	gtk_widget_show_all (m_window);
}
