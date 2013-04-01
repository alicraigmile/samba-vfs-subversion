samba-vfs-subversion
====================README for Samba Virtual File System (VFS) Example
===================================================

This directory contains a Samba VFS module.  When used,
this module simply passes all requests back to the disk functions
(i.e it operates as a passthrough filter) except then it detects
an SVN working copy. In that case, it'll SVN commit any changes.

To build:

./configure  --with-samba-source=../samba-3.5.4/source3 --enable-developer
make
make test
sudo checkinstall
