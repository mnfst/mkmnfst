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
struct sig_res *
sign(char *text, char *keyid)
{
	gpgme_ctx_t	 ctx;
	gpgme_error_t	 error;
	gpgme_key_t	 key, nkey;
	gpgme_data_t	 sig, text_data;
	struct sig_res	*signing;
	int		 has_seen_key;
	int		 ret, nread;
	char		 *nbuf;
	size_t	 	 nbytes;

	nbytes = 1024;
	nread = 0;
	has_seen_key = 0;
	nkey = NULL;

	if ((key = malloc(sizeof(gpgme_key_t))) == NULL)
		err(1, "malloc");
	key = NULL;

	gpgme_check_version(NULL);

	error = gpgme_new(&ctx);
	switch (error & GPG_ERR_CODE_MASK) {
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

	error = gpgme_op_keylist_start(ctx, keyid, 0);
	if ((error & GPG_ERR_CODE_MASK) == GPG_ERR_INV_VALUE)
		errx(EX_SOFTWARE,
		    "passed an invalid context to gpgme_op_keylist_start");

	do {
		error = gpgme_op_keylist_next(ctx, &nkey);
		switch (error & GPG_ERR_CODE_MASK) {
		case GPG_ERR_EOF:
			break;
		case GPG_ERR_NO_ERROR:
			if (nkey->revoked)
				break;
			if (nkey->expired)
				break;
			if (nkey->disabled)
				break;
			if (nkey->invalid)
				break;
			if (! nkey->can_sign)
				break;

			if (has_seen_key)
				errx(EX_USAGE, "too many matches; specify -r.");

			has_seen_key = 1;
			key = nkey;
			break;
		case GPG_ERR_INV_VALUE:
			errx(EX_SOFTWARE,
			    "passed an invalid pointer to gpgme_op_keylist_next");
			break;
		case GPG_ERR_ENOMEM:
			errx(EX_SOFTWARE, "could not allocate the context");
			break;
		default:
			errx(EX_SOFTWARE,
			    "mysterious error from gpgme_op_keylist_next: %i",
			    error);
			break;
		}
	} while ((error & GPG_ERR_CODE_MASK) == GPG_ERR_NO_ERROR);

	error = gpgme_op_keylist_end(ctx);
	switch (error & GPG_ERR_CODE_MASK) {
	case GPG_ERR_NO_ERROR:
		break;
	case GPG_ERR_INV_VALUE:
		errx(EX_SOFTWARE, "passed an invalid context to gpgme_op_keylist_end");
		break;
	case GPG_ERR_ENOMEM:
		errx(EX_SOFTWARE, "could not allocate the context");
		break;
	default:
		errx(EX_SOFTWARE, "mysterious error from gpgme_op_keylist_end: %i", error);
		break;
	}

	if (key == NULL)
		errx(EX_USAGE, "you must have a private PGP key");

	gpgme_signers_add(ctx, key);
	gpgme_key_release(key);

	error = gpgme_data_new_from_mem(&text_data, text, strlen(text), 1);
	switch (error & GPG_ERR_CODE_MASK) {
	case GPG_ERR_NO_ERROR:
		break;
	case GPG_ERR_INV_VALUE:
		errx(EX_SOFTWARE, "passed an invalid context to gpgme_data_new_from_mem");
		break;
	case GPG_ERR_ENOMEM:
		errx(EX_SOFTWARE, "could not allocate the data");
		break;
	default:
		errx(EX_SOFTWARE, "mysterious error from gpgme_data_new_from_mem: %i", error);
		break;
	}

	error = gpgme_data_new(&sig);
	switch (error & GPG_ERR_CODE_MASK) {
	case GPG_ERR_NO_ERROR:
		break;
	case GPG_ERR_INV_VALUE:
		errx(EX_SOFTWARE, "passed an invalid context to gpgme_data_new");
		break;
	case GPG_ERR_ENOMEM:
		errx(EX_SOFTWARE, "could not allocate the data");
		break;
	default:
		errx(EX_SOFTWARE, "mysterious error from gpgme_data_new: %i", error);
		break;
	}

	error = gpgme_op_sign(ctx, text_data, sig, GPGME_SIG_MODE_NORMAL);
	switch (error & GPG_ERR_CODE_MASK) {
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
	default:
		errx(EX_SOFTWARE, "mysterious error from gpgme_op_sign: %i", error);
		break;
	}

	if ((ret = gpgme_data_seek(sig, 0, SEEK_SET)) == -1)
		err(1, "gpgme_data_seek");

	if ((signing = calloc(1, sizeof(struct sig_res))) == NULL)
		err(1, "calloc");

	if ((signing->signature = calloc(nbytes, sizeof(char))) == NULL)
		err(1, "calloc");

	while ((nbytes = gpgme_data_read(sig, signing->signature + nread, nbytes)) > 0) {
		nread += nbytes;
		if ((nbuf = reallocarray(signing->signature, nread + nbytes, sizeof(char))) == NULL)
			err(1, "reallocarray");
		signing->signature = nbuf;
	}

	signing->signature[nread] = '\0';

	gpgme_release(ctx);

	return signing;
}
