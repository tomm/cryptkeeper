#ifndef _CONFIGDIALOG_H
#define _CONFIGDIALOG_H

class ConfigDialog {
	public:
	void Show ();
	void Hide ();
	ConfigDialog ();
	virtual ~ConfigDialog ();
	
	GtkWidget *m_window;

	private:
	GtkWidget *m_filemanager_entry;
	GtkWidget *m_idle_spinbutton;
	GtkWidget *m_keep_mountdir_checkbutton;
};

#endif /* _CONFIGDIALOG_H */
