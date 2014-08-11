mkmnfst
========

Post your PGP-signed manifesto to mnfst.

The mkmnfst utility posts a manifesto to mnfst.  This manifesto is signed
with your PGP key.  If a file is passed, that file is posted.  If no file
is passed it opens your editor, then posts the resulting file.
Abnormally terminating your editor cancels the program.

Examples
--------

To write some content in your favorite editor and post it to the main mnfst Web
site:

    mkmnfst

If you have written your opus in a file, `manifesto.txt`, and are now ready to
publish it:

    mkmnfst manifesto.txt

To publish to your private mnfst server, `hearmeout.xxx`:

    mkmnfst -s hearmeout.xxx manifesto.txt

Dependencies
------------

Depends on GPGME and libcurl.

OS support for OpenBSD, Debian, and OS X.

Copyright
---------

Copyright 2014 Mike Burns, mnfst
