#ifndef _PASSWORDCHANGEDIALOG_H
#define _PASSWORDCHANGEDIALOG_H

class PasswordChangeDialog {
	public:
	PasswordChangeDialog (const char *crypt_dir, const char *mount_dir);
	virtual ~PasswordChangeDialog ();
	void Show ();
	void Hide ();
	bool Apply ();

	private:
	char *m_cryptDir;
	GtkWidget *m_window;
	// gtk entries
	GtkWidget *m_oldPass, *m_newPass1, *m_newPass2;
};

#endif /* _PASSWORDCHANGEDIALOG_H */
