#ifndef _PASSWORDENTRYDIALOG_H
#define _PASSWORDENTRYDIALOG_H

class PasswordEntryDialog {
	public:
	PasswordEntryDialog ();
	virtual ~PasswordEntryDialog ();
	// if not null returned value must be freed
	char *Run();
	
	GtkWidget *m_dialog;

	private:
	GtkWidget *m_password_entry;
};

#endif /* _PASSWORDENTRYDIALOG_H */
