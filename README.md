mkmnfst
========

Post your PGP-signed manifesto to mnfst.

The mkmnfst utility posts a manifesto to mnfst.  This manifesto is signed
with your PGP key.  If a file is passed, that file is posted.  If no file
is passed it opens your editor, then posts the resulting file.
Abnormally terminating your editor cancels the program.

Dependencies
------------

Depends on GPGME and libcurl.

So far this only runs on OpenBSD 5.6.

Copyright
---------

Copyright 2014 Mike Burns, mnfst
