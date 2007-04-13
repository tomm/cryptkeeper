#include <gtk/gtk.h>
#include "CreateStashWizard.h"
#include "defines.h"
#include "cryptkeeper.h"
#include <string.h>
#include <stdlib.h>
#include "encfs_wrapper.h"

static gboolean on_window_close (GtkWidget *window, GdkEvent *event, CreateStashWizard *wizard)
{
	wizard->Hide ();
	return TRUE;
}

static void on_cancel_clicked (GtkButton *button, CreateStashWizard *wizard)
{
	wizard->Hide ();
}

static void on_forward_clicked (GtkButton *button, CreateStashWizard *wizard)
{
	wizard->GoForward ();
}

static void on_back_clicked (GtkButton *button, CreateStashWizard *wizard)
{
	wizard->GoBack ();
}

CreateStashWizard::CreateStashWizard ()
{
	m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width (GTK_CONTAINER (m_window), UI_WINDOW_BORDER);
	gtk_window_set_title (GTK_WINDOW (m_window), "Create new encrypted directory");

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

	Restart ();
}

void CreateStashWizard::UpdateStageUI ()
{
	if (m_contents != NULL) gtk_widget_destroy (m_contents);

	m_contents = gtk_vbox_new (FALSE, UI_SPACING);
	gtk_box_pack_start (GTK_BOX (m_vbox), m_contents, TRUE, TRUE, UI_SPACING);

	GtkWidget *w;

	switch (m_stage) {
		case WIZ_START:
			w = gtk_label_new ("Pick name and location of your new stash");
			gtk_box_pack_start (GTK_BOX (m_contents), w, FALSE, FALSE, UI_SPACING);
			
			m_magic = gtk_file_chooser_widget_new (GTK_FILE_CHOOSER_ACTION_SAVE);
			gtk_box_pack_start (GTK_BOX (m_contents), m_magic, TRUE, TRUE, UI_SPACING);

			break;
		case WIZ_PASSWD:
			w = gtk_label_new ("Enter a password for your new stash");
			gtk_box_pack_start (GTK_BOX (m_contents), w, FALSE, FALSE, UI_SPACING);
		
			m_magic = gtk_entry_new ();
			gtk_entry_set_visibility (GTK_ENTRY (m_magic), FALSE);
			gtk_box_pack_start (GTK_BOX (m_contents), m_magic, FALSE, FALSE, UI_SPACING);

			w = gtk_label_new ("Enter the password again");
			gtk_box_pack_start (GTK_BOX (m_contents), w, FALSE, FALSE, UI_SPACING);
		
			m_magic2 = gtk_entry_new ();
			gtk_entry_set_visibility (GTK_ENTRY (m_magic2), FALSE);
			gtk_box_pack_start (GTK_BOX (m_contents), m_magic2, FALSE, FALSE, UI_SPACING);
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

CreateStashWizard::~CreateStashWizard ()
{
	gtk_widget_destroy (m_window);
}

void CreateStashWizard::Show ()
{
	gtk_widget_show_all (m_window);
}

void CreateStashWizard::Hide ()
{
	gtk_widget_hide (m_window);
	gtk_button_set_label (GTK_BUTTON (m_button_forward), GTK_STOCK_GO_FORWARD);
	gtk_widget_set_sensitive (m_button_cancel, TRUE);
	Restart ();
}

void CreateStashWizard::Restart ()
{
	m_stage = WIZ_START;
	if (m_mount_dir) g_free (m_mount_dir);
	m_mount_dir = NULL;
	UpdateStageUI ();
}

void CreateStashWizard::GoForward ()
{
	if (m_stage == WIZ_START) {
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
	if (m_stage == WIZ_PASSWD) {
		const char *pwd1 = gtk_entry_get_text (GTK_ENTRY (m_magic));
		const char *pwd2 = gtk_entry_get_text (GTK_ENTRY (m_magic2));

		if (strcmp (pwd1, pwd2) != 0) {
			gtk_entry_set_text (GTK_ENTRY (m_magic), "");
			gtk_entry_set_text (GTK_ENTRY (m_magic2), "");
			GtkWidget *dialog = 
				gtk_message_dialog_new (GTK_WINDOW (m_window),
						GTK_DIALOG_MODAL,
						GTK_MESSAGE_ERROR,
						GTK_BUTTONS_OK,
						"The passwords do not match\nTry again");
			gtk_window_set_title (GTK_WINDOW (dialog), "Ooops!");
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
			return;
		}
		else if (strlen (pwd1) == 0) {
			GtkWidget *dialog = 
				gtk_message_dialog_new (GTK_WINDOW (m_window),
						GTK_DIALOG_MODAL,
						GTK_MESSAGE_ERROR,
						GTK_BUTTONS_OK,
						"You need to enter a password");
			gtk_window_set_title (GTK_WINDOW (dialog), "Ooops!");
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
			return;
		}
	}
	m_stage++;
	if (m_stage == WIZ_END) {
		char crypt_dir[1024];
		char *dirname = strdup (m_mount_dir);
		{
			char *basename = strrchr (dirname, '/');
			*basename = '\0';
			basename++;
			snprintf (crypt_dir, sizeof (crypt_dir), "%s/.%s_encfs", dirname, basename);
		}
		free (dirname);
		// non-zero indicates a terrible error
		if (encfs_stash_new (crypt_dir, m_mount_dir, gtk_entry_get_text (GTK_ENTRY (m_magic)))) {
			GtkWidget *dialog = 
				gtk_message_dialog_new (GTK_WINDOW (m_window),
						GTK_DIALOG_MODAL,
						GTK_MESSAGE_ERROR,
						GTK_BUTTONS_CANCEL,
						"Failed to create encfs stash -- SOMETHING went wrong...");
			gtk_window_set_title (GTK_WINDOW (dialog), "Error");
			gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
		} else {
			add_crypt_point (crypt_dir, m_mount_dir);
			spawn_filemanager (m_mount_dir);
		}
		g_free (m_mount_dir);
		m_mount_dir = NULL;
	
		gtk_widget_set_sensitive (m_button_cancel, FALSE);
		gtk_button_set_label (GTK_BUTTON (m_button_forward), GTK_STOCK_OK);
	}
	if (m_stage > WIZ_END) {
		Hide ();
	} else {
		UpdateStageUI ();
	}
}

void CreateStashWizard::GoBack ()
{
	m_stage--;
	UpdateStageUI ();
}
