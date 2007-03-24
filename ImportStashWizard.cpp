#include <gtk/gtk.h>
#include <sys/stat.h>
#include "ImportStashWizard.h"
#include "defines.h"
#include "cryptkeeper.h"
#include <string.h>
#include <stdlib.h>

static gboolean on_window_close (GtkWidget *window, GdkEvent *event, ImportStashWizard *wizard)
{
	wizard->Hide ();
	return TRUE;
}

static void on_cancel_clicked (GtkButton *button, ImportStashWizard *wizard)
{
	wizard->Hide ();
}

static void on_forward_clicked (GtkButton *button, ImportStashWizard *wizard)
{
	wizard->GoForward ();
}

static void on_back_clicked (GtkButton *button, ImportStashWizard *wizard)
{
	wizard->GoBack ();
}

ImportStashWizard::ImportStashWizard ()
{
	m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	g_signal_connect(G_OBJECT(m_window), "delete-event", G_CALLBACK(on_window_close), this);

	m_vbox = gtk_vbox_new (FALSE, UI_SPACING);
	gtk_container_add (GTK_CONTAINER (m_window), m_vbox);

	GtkWidget *buttonBox = gtk_hbutton_box_new ();
	gtk_box_pack_end (GTK_BOX (m_vbox), buttonBox, FALSE, FALSE, UI_SPACING);

	//GtkWidget *button = gtk_button_new_from_stock (GTK_STOCK_GO_BACK);
	//g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (on_back_clicked), this);
	//gtk_box_pack_end (GTK_BOX (buttonBox), button, FALSE, FALSE, UI_SPACING);

	m_button_forward = gtk_button_new_from_stock (GTK_STOCK_GO_FORWARD);
	g_signal_connect (G_OBJECT (m_button_forward), "clicked", G_CALLBACK (on_forward_clicked), this);
	gtk_box_pack_end (GTK_BOX (buttonBox), m_button_forward, FALSE, FALSE, UI_SPACING);

	m_button_cancel = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
	g_signal_connect (G_OBJECT (m_button_cancel), "clicked", G_CALLBACK (on_cancel_clicked), this);
	gtk_box_pack_end (GTK_BOX (buttonBox), m_button_cancel, FALSE, FALSE, UI_SPACING);

	m_contents = NULL;
	m_mount_dir = NULL;
	m_crypt_dir = NULL;

	Restart ();
}

void ImportStashWizard::UpdateStageUI ()
{
	if (m_contents != NULL) gtk_widget_destroy (m_contents);

	m_contents = gtk_vbox_new (FALSE, UI_SPACING);
	gtk_box_pack_start (GTK_BOX (m_vbox), m_contents, TRUE, TRUE, UI_SPACING);

	GtkWidget *w;

	switch (m_stage) {
		case WIZ_START:
			w = gtk_label_new ("Selecting an existing encrypted stash directory (eg ~/.crypt)");
			gtk_box_pack_start (GTK_BOX (m_contents), w, FALSE, FALSE, UI_SPACING);
			
			m_magic = gtk_file_chooser_widget_new (GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
			gtk_file_chooser_set_show_hidden (GTK_FILE_CHOOSER (m_magic), TRUE);
			gtk_box_pack_start (GTK_BOX (m_contents), m_magic, TRUE, TRUE, UI_SPACING);

			break;
		case WIZ_PASSWD:
			w = gtk_label_new ("Pick name and location at which to mount the stash");
			gtk_box_pack_start (GTK_BOX (m_contents), w, FALSE, FALSE, UI_SPACING);
			
			m_magic = gtk_file_chooser_widget_new (GTK_FILE_CHOOSER_ACTION_SAVE);
			gtk_box_pack_start (GTK_BOX (m_contents), m_magic, TRUE, TRUE, UI_SPACING);

			break;
		case WIZ_END:
			w = gtk_label_new ("Done!");
			gtk_box_pack_start (GTK_BOX (m_contents), w, FALSE, FALSE, UI_SPACING);
			break;
		default:
			break;
	}
	gtk_widget_show_all (m_contents);
}

ImportStashWizard::~ImportStashWizard ()
{
	gtk_widget_destroy (m_window);
}

void ImportStashWizard::Show ()
{
	gtk_widget_show_all (m_window);
}

void ImportStashWizard::Hide ()
{
	gtk_widget_hide (m_window);
	gtk_button_set_label (GTK_BUTTON (m_button_forward), GTK_STOCK_GO_FORWARD);
	gtk_widget_set_sensitive (m_button_cancel, TRUE);
	Restart ();
}

void ImportStashWizard::Restart ()
{
	m_stage = WIZ_START;
	if (m_crypt_dir) g_free (m_crypt_dir);
	m_crypt_dir = NULL;
	if (m_mount_dir) g_free (m_mount_dir);
	m_mount_dir = NULL;
	UpdateStageUI ();
}

void ImportStashWizard::GoForward ()
{
	if (m_stage == WIZ_START) {
		m_crypt_dir = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (m_magic));
		if (m_crypt_dir == NULL) {
			GtkWidget *dialog = 
				gtk_message_dialog_new (GTK_WINDOW (m_window),
						GTK_DIALOG_MODAL,
						GTK_MESSAGE_ERROR,
						GTK_BUTTONS_OK,
						"You need to select a directory");
			gtk_window_set_title (GTK_WINDOW (dialog), "Ooops!");
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
			return;
		}
		char buf[1024];
		snprintf (buf, sizeof (buf), "%s/.encfs5", m_crypt_dir);
		struct stat blah;
		if (stat (buf, &blah) == -1) {
			GtkWidget *dialog = 
				gtk_message_dialog_new (GTK_WINDOW (m_window),
						GTK_DIALOG_MODAL,
						GTK_MESSAGE_ERROR,
						GTK_BUTTONS_OK,
						"The selected directory is not an encfs stash");
			gtk_window_set_title (GTK_WINDOW (dialog), "Ooops!");
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
			g_free (m_crypt_dir);
			m_crypt_dir = NULL;
			return;
		}
	}
	if (m_stage == WIZ_PASSWD) {
		m_mount_dir = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (m_magic));
		if (m_mount_dir == NULL) {
			GtkWidget *dialog = 
				gtk_message_dialog_new (GTK_WINDOW (m_window),
						GTK_DIALOG_MODAL,
						GTK_MESSAGE_ERROR,
						GTK_BUTTONS_OK,
						"You need to enter a name");
			gtk_window_set_title (GTK_WINDOW (dialog), "Ooops!");
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
			return;
		}
	}
	m_stage++;
	if (m_stage == WIZ_END) {
		gtk_widget_set_sensitive (m_button_cancel, FALSE);
		gtk_button_set_label (GTK_BUTTON (m_button_forward), GTK_STOCK_OK);
		
		add_crypt_point (m_crypt_dir, m_mount_dir);

		g_free (m_crypt_dir);
		g_free (m_mount_dir);
		m_mount_dir = NULL;
		m_crypt_dir = NULL;
	}
	if (m_stage > WIZ_END) {
		Hide ();
	} else {
		UpdateStageUI ();
	}
}

void ImportStashWizard::GoBack ()
{
	m_stage--;
	UpdateStageUI ();
}
