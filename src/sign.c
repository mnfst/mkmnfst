#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include <gpgme.h>

#include "compat.h"

#include "sign.h"

/*
 * Generate an OpenPGP ASCII-armored signature for the text.
 *
 * This uses the first PGP private key we find, and errors if more than one key
 * exists. GPGME handles the passphrase.
 */
char *
sign(char *text)
{
	gpgme_ctx_t	 ctx;
	gpgme_error_t	 error;
	gpgme_key_t	 key, nkey;
	gpgme_data_t	 sig, text_data;
	int		 has_seen_key;
	int		 ret, nread;
	char		*buf, *nbuf;
	size_t	 	nbytes;

	nbytes = 1024;
	nread = 0;
	has_seen_key = 0;

	if ((key = malloc(sizeof(gpgme_key_t))) == NULL)
		err(1, "malloc");

	gpgme_check_version(NULL);

	switch (error = gpgme_new(&ctx)) {
	case GPG_ERR_NO_ERROR:
		break;
	case GPG_ERR_INV_VALUE:
		errx(EX_SOFTWARE, "made an invalid context");
		break;
	case GPG_ERR_ENOMEM:
		errx(EX_SOFTWARE, "could not allocate the context");
		break;
	case GPG_ERR_NOT_OPERATIONAL:
		errx(EX_SOFTWARE, "not operational; programmer error");
		break;
	case GPG_ERR_SELFTEST_FAILED:
		errx(EX_SOFTWARE, "failed self test");
		break;
	default:
		errx(EX_SOFTWARE, "mysterious error from gpgme_new: %i", error);
		break;
	}

	gpgme_set_armor(ctx, 1);

	if ((error = gpgme_op_keylist_start(ctx, NULL, 1)) == GPG_ERR_INV_VALUE)
		errx(EX_SOFTWARE, "made an invalid context");

	do {
		switch (error = gpgme_op_keylist_next(ctx, &nkey)) {
		case GPG_ERR_INV_VALUE:
			errx(EX_SOFTWARE, "made an invalid context");
			break;
		case GPG_ERR_ENOMEM:
			errx(EX_SOFTWARE, "could not allocate the context");
			break;
		case GPG_ERR_EOF:
			break;
		case GPG_ERR_NO_ERROR:
			if (has_seen_key)
				errx(EX_USAGE, "can not yet prompt for keys");
			has_seen_key = 1;
			key = nkey;
			break;
		}
	} while (error == GPG_ERR_NO_ERROR);

	switch (error = gpgme_op_keylist_end(ctx)) {
	case GPG_ERR_INV_VALUE:
		errx(EX_SOFTWARE, "made an invalid context");
		break;
	case GPG_ERR_ENOMEM:
		errx(EX_SOFTWARE, "could not allocate the context");
		break;
	}

	gpgme_signers_add(ctx, key);
	gpgme_key_release(key);

	error = gpgme_data_new_from_mem(&text_data, text, strlen(text), 1);
	switch (error) {
	case GPG_ERR_NO_ERROR:
		break;
	case GPG_ERR_INV_VALUE:
		errx(EX_SOFTWARE, "made an invalid context");
		break;
	case GPG_ERR_ENOMEM:
		errx(EX_SOFTWARE, "could not allocate the data");
		break;
	}

	error = gpgme_data_new(&sig);
	switch (error) {
	case GPG_ERR_NO_ERROR:
		break;
	case GPG_ERR_INV_VALUE:
		errx(EX_SOFTWARE, "made an invalid context");
		break;
	case GPG_ERR_ENOMEM:
		errx(EX_SOFTWARE, "could not allocate the data");
		break;
	}

	error = gpgme_op_sign(ctx, text_data, sig, GPGME_SIG_MODE_NORMAL);
	switch (error) {
	case GPG_ERR_NO_ERROR:
		break;
	case GPG_ERR_INV_VALUE:
		errx(EX_SOFTWARE, "invalid context, data, or sig");
		break;
	case GPG_ERR_NO_DATA:
		errx(EX_SOFTWARE, "could not create signature");
		break;
	case GPG_ERR_BAD_PASSPHRASE:
		errx(EX_SOFTWARE, "bad passphrase");
		break;
	case GPG_ERR_UNUSABLE_SECKEY:
		errx(EX_SOFTWARE, "invalid signers");
		break;
	case 117440523:
		errx(EX_SOFTWARE, "your gpg-agent is broken");
		break;
	default:
		errx(EX_SOFTWARE, "mysterious error from gpgme_op_sign: %i", error);
		break;
	}

	if ((ret = gpgme_data_seek(sig, 0, SEEK_SET)) == -1)
		err(1, "gpgme_data_seek");

	if ((buf = calloc(nbytes, sizeof(char))) == NULL)
		err(1, "calloc");

	while ((nbytes = gpgme_data_read(sig, buf, nbytes)) > 0) {
		nread += nbytes;
		if ((nbuf = reallocarray(buf, nread + nbytes, sizeof(char))) == NULL)
			err(1, "reallocarray");
		buf = nbuf;
	}

	gpgme_release(ctx);

	return buf;
}
