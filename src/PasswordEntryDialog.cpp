/*
 * This file is part of Cryptkeeper.
 * Copyright (C) 2007 Tom Morton
 *
 * Cryptkeeper is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * Cryptkeeper is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <string>
#include "PasswordEntryDialog.h"
#include "defines.h"
#include "cryptkeeper.h"

PasswordEntryDialog::PasswordEntryDialog ()
{
	GtkWidget *w, *hbox;

	m_dialog = gtk_dialog_new();
	
	gtk_dialog_add_button(GTK_DIALOG(m_dialog), GTK_STOCK_OK, GTK_RESPONSE_ACCEPT);
	gtk_dialog_add_button(GTK_DIALOG(m_dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT);
	gtk_dialog_set_has_separator(GTK_DIALOG(m_dialog), FALSE);
	gtk_dialog_set_default_response(GTK_DIALOG(m_dialog), GTK_RESPONSE_ACCEPT);
	gtk_container_set_border_width(GTK_CONTAINER(m_dialog), UI_WINDOW_BORDER);

	GtkWidget *vbox = GTK_DIALOG(m_dialog)->vbox;
	w = gtk_label_new(_("Enter your password:"));
	gtk_box_pack_start(GTK_BOX(vbox), w, FALSE, FALSE, UI_SPACING);
	
	m_password_entry = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(m_password_entry), FALSE);
	gtk_entry_set_activates_default(GTK_ENTRY(m_password_entry), TRUE);
	gtk_box_pack_start(GTK_BOX(vbox), m_password_entry, FALSE, FALSE, UI_SPACING);
}

PasswordEntryDialog::~PasswordEntryDialog ()
{
	gtk_widget_destroy (m_dialog);
}

char *PasswordEntryDialog::Run()
{
	gtk_widget_show_all (m_dialog);
	if (gtk_dialog_run(GTK_DIALOG(m_dialog)) == GTK_RESPONSE_ACCEPT) {
		return strdup(gtk_entry_get_text(GTK_ENTRY(m_password_entry)));
	} else {
		return NULL;
	}
}
