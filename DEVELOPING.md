Developing
==========

Principles
----------

- We follow the OpenBSD KNF.
- Always able to build and run on the latest OpenBSD, Debian, and OS X.
- Use the OS and C library first.

Details
-------

*OpenBSD KNF*. See style(9) for details, or [find it online][openbsd-knf]. This
is a way of thinking: explictness in an effort to understand what the software
does, but not too much explictness so that the idea is not muddled by the
syntax. Use these vim settings for the indentation format:

    set sw=0
    set ts=8
    set noet
    set cinoptions=:0,t0,+4,(4

[openbsd-knf]: http://www.openbsd.org/cgi-bin/man.cgi/OpenBSD-current/man9/style.9

*Build and run on OpenBSD, Debian, OS X*. These three OSes represent the
systems that we use, therefore they are the ones we are best able to support.
Our target audience uses them, too.

*Use libc first*. It is far too easy to reach for convenience functions in
extra libraries, but we should first use and practice what we have in our OS.
This helps keep the code focused and accessible.

Setup
-----

See the dependencies in the `README.md`.

To build from a fresh checkout:

    ./autogen.sh
    ./configure
    make

Release
-------

1. Update the version in `configure.ac`:

        AC_INIT([mkmnfst], [0.2], [support@mnfst.io])

2. Verify the tarball:

        make distclean
        ./autogen.sh
        ./configure
        make distcheck
        tar -ztf mkmnfst-0.2.tar.gz | less

3. Tag the repo:

        git checkout master
        git pull --rebase
        git push origin master
        git status
        git tag -u 0xFBAE90619F653118 -m "Release version 0.2" v0.2
        git push origin master --tags

4. Sign the tarball:

        gpg -sab -u 0xFBAE90619F653118 mkmnfst-0.2.tar.gz

5. Verify the signature:

        gpg --verify mkmnfst-0.2.tar.gz.asc mkmnfst-0.2.tar.gz

6. [Announce on GitHub][announce]

7. Update the installation instructions on [the about page][about].

[announce]: https://github.com/mnfst/mkmnfst/releases/new
[about]: https://github.com/mnfst/mnfst/blob/master/app/views/pages/about.html.erb
