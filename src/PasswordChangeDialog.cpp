#include <gtk/gtk.h>
#include "PasswordChangeDialog.h"
#include "defines.h"
#include "cryptkeeper.h"
#include <string.h>
#include <stdlib.h>
#include "encfs_wrapper.h"

static gboolean on_window_close (GtkWidget *window, GdkEvent *event, PasswordChangeDialog *blah)
{
	delete blah;
	return TRUE;
}

static void on_cancel_clicked (GtkButton *b, PasswordChangeDialog *blah)
{
	delete blah;
}

static void on_ok_clicked (GtkButton *b, PasswordChangeDialog *self)
{
	if (self->Apply ()) delete self;
}

bool PasswordChangeDialog::Apply ()
{
	const char *old_pass = gtk_entry_get_text (GTK_ENTRY (m_oldPass));
	const char *new_pass1 = gtk_entry_get_text (GTK_ENTRY (m_newPass1));
	const char *new_pass2 = gtk_entry_get_text (GTK_ENTRY (m_newPass2));

	if (strcmp (new_pass1, new_pass2) != 0) {
		GtkWidget *dialog = 
			gtk_message_dialog_new (GTK_WINDOW (m_window),
					GTK_DIALOG_MODAL,
					GTK_MESSAGE_ERROR,
					GTK_BUTTONS_OK,
					_("The new passwords do not match\nTry again"));
		gtk_window_set_title (GTK_WINDOW (dialog), _("Ooops!"));
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
		return false;
	}

	char *output;
	int status = encfs_stash_change_password (m_cryptDir, old_pass, new_pass1, &output);
	if (status) {
		char buf2[1024];
		snprintf (buf2, sizeof (buf2), _("Error setting password:\n%s"), output);
		GtkWidget *dialog = gtk_message_dialog_new (NULL,
				GTK_DIALOG_MODAL,
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_CANCEL,
				buf2);
		gtk_window_set_title (GTK_WINDOW (dialog), _("Error"));
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
		free (output);
		return false;
	} else {
		GtkWidget *dialog = gtk_message_dialog_new (NULL,
				GTK_DIALOG_MODAL,
				GTK_MESSAGE_INFO,
				GTK_BUTTONS_CLOSE,
				_("Password changed"));
		gtk_window_set_title (GTK_WINDOW (dialog), _("Information"));
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
		free (output);
		return true;
	}
}

PasswordChangeDialog::PasswordChangeDialog (const char *crypt_dir, const char *mount_dir)
{
	GtkWidget *w;
	m_cryptDir = strdup (crypt_dir);

	m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (m_window), _("Change password"));
	gtk_container_set_border_width (GTK_CONTAINER (m_window), UI_WINDOW_BORDER);
	g_signal_connect (G_OBJECT (m_window), "delete-event", G_CALLBACK (on_window_close), this);

	GtkWidget *vbox = gtk_vbox_new (FALSE, UI_SPACING);
	gtk_container_add (GTK_CONTAINER (m_window), vbox);

	GtkWidget *buttonBox = gtk_hbutton_box_new ();
	gtk_box_pack_end (GTK_BOX (vbox), buttonBox, FALSE, FALSE, UI_SPACING);

		char buf[512];
		snprintf (buf, sizeof (buf), _("Enter the current password for %s:"), mount_dir);
		w = gtk_label_new (buf);
		gtk_box_pack_start (GTK_BOX (vbox), w, FALSE, FALSE, UI_SPACING);
		
		m_oldPass = gtk_entry_new ();
		gtk_entry_set_visibility (GTK_ENTRY (m_oldPass), FALSE);
		gtk_box_pack_start (GTK_BOX (vbox), m_oldPass, FALSE, FALSE, UI_SPACING);

		w = gtk_label_new (_("Enter a new password"));
		gtk_box_pack_start (GTK_BOX (vbox), w, FALSE, FALSE, UI_SPACING);
		
		m_newPass1 = gtk_entry_new ();
		gtk_entry_set_visibility (GTK_ENTRY (m_newPass1), FALSE);
		gtk_box_pack_start (GTK_BOX (vbox), m_newPass1, FALSE, FALSE, UI_SPACING);

		w = gtk_label_new (_("Enter the new password again to confirm"));
		gtk_box_pack_start (GTK_BOX (vbox), w, FALSE, FALSE, UI_SPACING);
		
		m_newPass2 = gtk_entry_new ();
		gtk_entry_set_visibility (GTK_ENTRY (m_newPass2), FALSE);
		gtk_box_pack_start (GTK_BOX (vbox), m_newPass2,  FALSE, FALSE, UI_SPACING);

	w = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
	g_signal_connect (G_OBJECT (w), "clicked", G_CALLBACK (on_cancel_clicked), this);
	gtk_box_pack_end (GTK_BOX (buttonBox), w, FALSE, FALSE, UI_SPACING);

	w = gtk_button_new_from_stock (GTK_STOCK_OK);
	g_signal_connect (G_OBJECT (w), "clicked", G_CALLBACK (on_ok_clicked), this);
	gtk_box_pack_end (GTK_BOX (buttonBox), w, FALSE, FALSE, UI_SPACING);

}

void PasswordChangeDialog::Show ()
{
	gtk_widget_show_all (m_window);
}

void PasswordChangeDialog::Hide ()
{
	gtk_widget_hide (m_window);
}

PasswordChangeDialog::~PasswordChangeDialog ()
{
	if (m_window) gtk_widget_destroy (m_window);
	free (m_cryptDir);
}
