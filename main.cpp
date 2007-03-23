#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <gtk/gtk.h>
#include <string.h>
#include <vector>
#include <assert.h>

#include "CreateStashWizard.h"

class CryptPoint {
	private:
	char *crypt_dir, *mount_dir;
	bool isAvailable, isMounted;
	public:
	CryptPoint (const char *crypt_dir, const char *mount_dir) {
		this->crypt_dir = strdup (crypt_dir);
		this->mount_dir = strdup (mount_dir);
	}
	virtual ~CryptPoint () {
//		free (crypt_dir);
//		free (mount_dir);
	}
	const char *GetMountDir () { return mount_dir; }
	const char *GetCryptDir () { return crypt_dir; }
	bool GetIsMounted () { return isMounted; }
	void SetIsMounted (bool b) { isMounted = b; }
	bool GetIsAvailable () { return isAvailable; }
	void SetIsAvailable (bool b) { isAvailable = b; }
};

void write_config ();

static GtkStatusIcon *sico;
static std::vector<CryptPoint> cryptPoints;
static CreateStashWizard *create_stash_wizard;

static void spawn_filemanager (const char *dir)
{
	char buf[256];
	snprintf (buf, sizeof (buf), "thunar %s", dir);
	system (buf);
}

bool make_new_encfs_stash (const char *mount_dir, const char *password)
{
	int fd[2];
	char stash_dir[256];

	char *dirname = strdup (mount_dir);
	{
		char *basename = strrchr (dirname, '/');
		*basename = '\0';
		basename++;
		snprintf (stash_dir, sizeof (stash_dir), "%s/.%s_encfs", dirname, basename);
		printf ("Making shit in %s\n", stash_dir);
	}

	assert (pipe (fd) == 0);

	int pid = fork ();

	mkdir (stash_dir, 0700);
	mkdir (mount_dir, 0700);

	if (pid == 0) {
		dup2 (fd[0], 0);
		close (fd[1]);
		execlp ("encfs", "encfs", "-S", stash_dir, mount_dir, NULL);
		exit (0);
	}
	close (fd[0]);
	// paranoid default setup mode
	//write (fd[1], "y\n", 2);
	//write (fd[1], "y\n", 2);
	write (fd[1], "p\n", 2);
	write (fd[1], password, strlen (password));
	write (fd[1], "\n", 1);
	close (fd[1]);
	cryptPoints.push_back (CryptPoint (stash_dir, mount_dir));
	write_config ();
	spawn_filemanager (mount_dir);
	return true;
}

// returns true on success
static bool unmount_cryptpoint (int idx)
{
	CryptPoint *cp = &cryptPoints[idx];
	
	if (cp->GetIsMounted () == false) return true;

	// unmount
	int pid = fork ();
	if (pid == 0) {
		execlp ("fusermount", "fusermount", "-u", cp->GetMountDir (), NULL);
		exit (0);
	}
	waitpid (pid, NULL, 0);
	return (rmdir (cp->GetMountDir ()) == 0);
}

static void moan_cant_unmount ()
{
	GtkWidget *dialog = gtk_message_dialog_new (NULL,
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_ERROR,
			GTK_BUTTONS_CANCEL,
			"The stash couldn't be unmounted.\nPerhaps it is in use.");
	gtk_window_set_title (GTK_WINDOW (dialog), "Error");
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

static void moan_cant_mount ()
{
	GtkWidget *dialog = gtk_message_dialog_new (NULL,
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_ERROR,
			GTK_BUTTONS_CANCEL,
			"The stash couldn't be mounted.\nMaybe invalid password.");
	gtk_window_set_title (GTK_WINDOW (dialog), "Error");
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

static void on_mount_check_item_toggled (GtkCheckMenuItem *mi, int idx)
{
	printf ("%d\n", idx);
	CryptPoint *cp = &cryptPoints[idx];

	if (cp->GetIsMounted ()) {
		if (!unmount_cryptpoint (idx)) moan_cant_unmount ();
	} else {
		mkdir (cp->GetMountDir (), 0700);
		int pid = fork ();
		if (pid == 0) {
			execlp ("encfs", "encfs", "--extpass=./cryptkeeper_password", cp->GetCryptDir (), cp->GetMountDir (), NULL);
			exit (0);
		}
		waitpid (pid, NULL, 0);
		// will succeed if the dude fucked up his password, so dandy
		if (rmdir (cp->GetMountDir ()) == -1) {
			spawn_filemanager (cp->GetMountDir ());
		} else {
			moan_cant_mount ();
		}
	}
}

static void on_create_new_stash_clicked (GtkWidget *blah)
{
	create_stash_wizard->Show ();
}

static GtkWidget *stashes_popup_menu;

static bool on_dostuff_menu_destroy ()
{
	gtk_widget_destroy (stashes_popup_menu);
	return FALSE;
}

gboolean on_click_delete_stash (GtkMenuItem *mi, gpointer data)
{
	int idx = GPOINTER_TO_INT (data);

	GtkWidget *dialog = gtk_message_dialog_new (NULL,
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_QUESTION,
			GTK_BUTTONS_OK_CANCEL,
			"Are you sure you want to remove the stash at:\n%s",
			cryptPoints[idx].GetMountDir ());
	gtk_window_set_title (GTK_WINDOW (dialog), "Confirm");
	int result = gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
	if (result == GTK_RESPONSE_CANCEL) return FALSE;
	printf ("result %d\n", result);
	
	if (!unmount_cryptpoint (idx)) {
		// fuck. can't unmount
		moan_cant_unmount ();
	} else {
		GtkWidget *dialog = gtk_message_dialog_new (NULL,
				GTK_DIALOG_MODAL,
				GTK_MESSAGE_WARNING,
				GTK_BUTTONS_YES_NO,
				"Do you want to permanently erase the stash at:\n%s",
				cryptPoints[idx].GetMountDir ());
		gtk_window_set_title (GTK_WINDOW (dialog), "Confirm");
		int result = gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);

		if (result == GTK_RESPONSE_NO) {
			cryptPoints.erase (cryptPoints.begin () + idx);
		} else {
			// recursive delete is tedious to implement ;)
			char buf[1024];
			snprintf (buf, sizeof (buf), "rm -rf %s", cryptPoints[idx].GetCryptDir ());
			system (buf);
			cryptPoints.erase (cryptPoints.begin () + idx);
		}
		write_config ();
	}
	
	return FALSE;
}

gboolean on_button_release (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	int item_no = GPOINTER_TO_INT (data);

	if (event->button != 3) return FALSE;

	GtkWidget *menu = gtk_menu_new ();

	GtkWidget *mi = gtk_menu_item_new_with_label ("Delete stash");
	g_signal_connect (G_OBJECT (mi), "activate", G_CALLBACK (on_click_delete_stash), GINT_TO_POINTER (item_no));
	gtk_menu_append (menu,  mi);

	gtk_widget_show_all (menu);

	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, widget,
			0, event->time);
	g_signal_connect (G_OBJECT (menu), "hide", G_CALLBACK (on_dostuff_menu_destroy), NULL);

	return TRUE;
}

static const char *author_names[] = { "Tom Morton" };

void open_about_dialog ()
{
	GtkWidget *dialog = gtk_about_dialog_new ();
	gtk_about_dialog_set_name (GTK_ABOUT_DIALOG (dialog), "Cryptkeeper 0.1.666");
	gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (dialog), author_names);
	gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (dialog),
		"This program is free software; you can redistribute it and/or modify it\n"
		"under the terms of the GNU General Public License version 2, as published\n"
		"by the Free Software Foundation.");
	gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (dialog), "Hasta la victoria siempre!");
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

static void sico_right_button_activated ()
{
	GtkWidget *menu = gtk_menu_new ();

	GtkWidget *mi = gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT, NULL);
	g_signal_connect (G_OBJECT (mi), "activate", G_CALLBACK (open_about_dialog), NULL);
	gtk_menu_append (GTK_MENU (menu), mi);

	gtk_widget_show_all (menu);

	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, gtk_status_icon_position_menu, sico,
			0, gtk_get_current_event_time ());
}

static void sico_activated (GtkWidget *data)
{
	stashes_popup_menu = gtk_menu_new ();

	GtkWidget *mi = gtk_menu_item_new_with_label ("Encrypted stashes:");
	gtk_widget_set_sensitive (mi, FALSE);
	gtk_menu_append (stashes_popup_menu, mi);

	int i = 0;
	std::vector<CryptPoint>::iterator it;
	// find out which ones are mounted
	for (it = cryptPoints.begin (); it != cryptPoints.end (); ++it, i++) {
		struct stat s;

		(*it).SetIsMounted (false);
		(*it).SetIsAvailable (false);

		if (stat ((*it).GetCryptDir (), &s) != -1) {
			if (S_ISDIR (s.st_mode)) (*it).SetIsAvailable (true);
		}

		if (stat ((*it).GetMountDir (), &s) != -1) {
			if (S_ISDIR (s.st_mode)) (*it).SetIsMounted (true);
		}
	}
	
	i = 0;
	for (it = cryptPoints.begin (); it != cryptPoints.end (); ++it, i++) {
		mi = gtk_check_menu_item_new_with_label ((*it).GetMountDir ());
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (mi), (*it).GetIsMounted ());
		gtk_widget_set_sensitive (mi, (*it).GetIsAvailable ());
		gtk_menu_append (stashes_popup_menu, mi);
		g_signal_connect (G_OBJECT (mi), "toggled", G_CALLBACK (on_mount_check_item_toggled), GINT_TO_POINTER (i));
		g_signal_connect (G_OBJECT (mi), "button-release-event", G_CALLBACK (on_button_release), GINT_TO_POINTER (i));
	}

	mi = gtk_separator_menu_item_new ();
	gtk_menu_append (stashes_popup_menu, mi);

	mi = gtk_menu_item_new_with_label ("Create new stash");
	g_signal_connect (G_OBJECT (mi), "activate", G_CALLBACK (on_create_new_stash_clicked), NULL);
	gtk_menu_append (stashes_popup_menu, mi);
	
	gtk_widget_show_all (stashes_popup_menu);

	gtk_menu_popup (GTK_MENU (stashes_popup_menu), NULL, NULL, gtk_status_icon_position_menu, sico,
			0, gtk_get_current_event_time ());
}

char *config_loc;

void write_config ()
{
	char buf[1024];
	
	snprintf (buf, sizeof (buf), "%s/.config/cryptkeeper/stashes", getenv ("HOME"));
	FILE *f = fopen (buf, "w");

	if (f == NULL) {
		fprintf (stderr, "Cryptkeeper ERROR: Unable to write %s\n", buf);
		exit (0);
	}

	fprintf (f, "# Auto-generated by Cryptkeeper. it is dandy\n");

	std::vector<CryptPoint>::iterator it;
	// find out which ones are mounted
	for (it = cryptPoints.begin (); it != cryptPoints.end (); ++it) {
		fprintf (f, "\"%s\" \"%s\"\n", (*it).GetCryptDir (), (*it).GetMountDir ());
	}

	fclose (f);
}

void read_config ()
{
	char buf[1024];

	// make sure these directories exist
	snprintf (buf, sizeof (buf), "%s/.config", getenv ("HOME"));
	mkdir (buf, 0700);
	snprintf (buf, sizeof (buf), "%s/.config/cryptkeeper", getenv ("HOME"));
	mkdir (buf, 0700);

	snprintf (buf, sizeof (buf), "%s/.config/cryptkeeper/stashes", getenv ("HOME"));
	FILE *f = fopen (buf, "r");

	int line_no = 0;
	char *crypt_dir, *mount_dir, *pos;
	// stashes is plain text, # lines are comments, others should be in form:
	// "crypt directory" "mount directory"
	if (f != NULL) {
		while (fgets (buf, sizeof (buf), f)) {
			line_no++;
			pos = NULL;

			if (buf[0] == '#') continue;

			if (buf[0] == '\"') {
				crypt_dir = &buf[1];
				pos = strchr (crypt_dir, '\"');
				if (pos == NULL) goto bad;

				pos[0] = '\0';
				pos++;

				pos = strchr (pos, '\"');
				if (pos == NULL) goto bad;
				pos++;
				mount_dir = pos;
				pos = strchr (pos, '\"');
				if (pos == NULL) goto bad;

				*pos = '\0';
				cryptPoints.push_back (CryptPoint (crypt_dir, mount_dir));
				continue;
			}
bad:
			fprintf (stderr, "blah blah error line %d\n", line_no);
		}
	}
}

int main (int argc, char *argv[])
{
	read_config ();

	//cryptPoints.push_back (CryptPoint ("/home/tom/one", "/home/tom/two"));
	//cryptPoints.push_back (CryptPoint ("/home/tom/.crypt", "/home/tom/crypt"));
	//cryptPoints.push_back (CryptPoint ("/media/usbdisk/.crypt", "/media/usbdisk/crypt"));
	//cryptPoints.push_back (CryptPoint ("/media/disk/.crypt", "/media/disk/crypt"));

	gtk_init (&argc, &argv);

//	GtkWidget *win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
//	gtk_widget_show (win);

	sico = gtk_status_icon_new_from_stock (GTK_STOCK_DIALOG_AUTHENTICATION);

	g_signal_connect(G_OBJECT(sico), "activate", G_CALLBACK(sico_activated), NULL);
	g_signal_connect(G_OBJECT(sico), "popup-menu", G_CALLBACK(sico_right_button_activated), NULL);

	create_stash_wizard = new CreateStashWizard ();

	gtk_main ();
	
	//fusermount ("/home/tom/one", "/home/tom/two");
	return 0;
}
