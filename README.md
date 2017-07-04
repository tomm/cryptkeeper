Cryptkeeper is a Linux system tray applet that manages EncFS encrypted folders.

To build from source you will need:

* libgtk+ >= 2.8
* gconf 2.0
* encfs and fuse (fuse 2.6.3 works)

On Ubuntu you can install EncFS like this:

```
sudo aptitude install encfs
sudo echo "fuse" >> /etc/modules
sudo modprobe fuse
sudo addgroup <your username> fuse
```

Then you must log off and back on again.

# Release Information

* Changes from 0.9.4 to 0.9.5: Polish, Italian and Spanish translations. 'Allow other' mount option and improvement of password dialog (it lets you try again if you get it wrong...)
* Changes from 0.9.3 to 0.9.4: Fixed a wee bug in dealing with mount points that are symbolic links.
* Changes from 0.9.2 to 0.9.3: g++ 4.3 compile fix, turkish translation, importing of encfs >= 1.4.2 stashes support.

# Binary packages

If you are using Ubuntu then Cryptkeeper is in the [universe repository](http://archive.ubuntu.com/ubuntu/pool/universe/c/cryptkeeper/).

If you are using Debian Sid/Lenny then Cryptkeeper is in the [main repository](http://ftp.debian.org/debian/pool/main/c/cryptkeeper/).

# License

Cryptkeeper is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3, as published by the Free Software Foundation.

# Frequently asked questions

* How do I delete an encrypted folder?
  * Click on the Cryptkeeper applet to bring up the list of encrypted folders. Right-click on the folder you want to delete and a popup menu will appear giving you the options "delete encrypted folder", "change password", and "information".