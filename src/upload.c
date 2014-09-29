#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <err.h>
#include <string.h>
#include <stdlib.h>

#include <curl/curl.h>

#include "upload.h"
#include "replace_str.h"
#include "mem_str.h"
#include "compat.h"

static size_t	set_location(char *, size_t, size_t, char **);
static size_t	set_body(char *, size_t, size_t, struct mem_str *);

/*
 * POST the JSON to the new status URL, with a Content-Type of
 * application/json. Do not send the Expect: header, as this fails.
 *
 * First it escapes newlines in the JSON.
 */
char *
upload(char *json, int use_https, char *server_name)
{
	CURL			*handle;
	struct curl_slist	*headers;
	struct mem_str		*body_mem;
	char			*esc_json, errbuf[CURL_ERROR_SIZE];
	char			*content_type, *url, *location;
	int			 len;
	long			 code;

	headers = NULL;
	content_type = "Content-Type: application/json";
	location = NULL;
	body_mem = new_mem_str();

	if (server_name == NULL)
		server_name = DEFAULT_SERVER_NAME;

	/* protocol + "://" + domain + "/" + statuses" + \0 */
	len = (use_https ? 5 : 4) + 3 + strlen(server_name) +
	    1 + strlen(POST_STATUS) + 1;

	if ((url = calloc(len, sizeof(char))) == NULL)
		err(1, "calloc");

	snprintf(url, len,  "%s://%s/%s", use_https ? "https" : "http",
	    server_name, POST_STATUS);

	if ((headers = curl_slist_append(headers, content_type)) == NULL)
		errx(1, "curl_slist_append");

	if ((headers = curl_slist_append(headers, "Expect:")) == NULL)
		errx(1, "curl_slist_append");

	if ((handle = curl_easy_init()) == NULL)
		errx(1, "curl_easy_init failed");

	esc_json = replace_str(json, "\n", "\\n");

	curl_easy_setopt(handle, CURLOPT_URL, url);
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, esc_json);
	curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, &errbuf);
	curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, set_location);
	curl_easy_setopt(handle, CURLOPT_HEADERDATA, &location);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, set_body);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, body_mem);
	curl_easy_setopt(handle, CURLOPT_VERBOSE, 0);

	if (curl_easy_perform(handle) != 0)
		errx(2, "curl: %s", errbuf);

	if (curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &code) != CURLE_OK)
		errx(2, "curl_easy_getinfo");

	free(url);
	free(esc_json);
	curl_easy_cleanup(handle);
	curl_slist_free_all(headers);

	switch (code / 100) {
	case 2:
		free_mem_str(body_mem);
		return location;
		break;
	case 4:
		errx(4, "%s", body_mem->str);
		free_mem_str(body_mem);
		break;
	default:
		free_mem_str(body_mem);
		errx(5, "Server error; contact the server admin for assistance");
		break;
	}
}

/*
 * Called by curl on each header line with a full line, 0-terminated. Passed a
 * pointer to a string into which we write the value of the Location header.
 */
static size_t
set_location(char *buf, size_t size, size_t nitems, char **location)
{
	char	 key[] = "Location: ";
	int	 len, key_len = 10;

	len = nitems - key_len;

	if (strncmp(buf, key, key_len) != 0)
		return nitems;

	if ((*location = calloc(nitems - key_len, size)) == NULL)
		err(1, "calloc");

	strlcpy(*location, buf + key_len, len);

	return nitems;
}

/*
 * Called by curl with an arbitrary amount of characters from the body of the
 * HTTP response. Write it into the memory pointed to by m.
 */
static size_t
set_body(char *buf, size_t size, size_t nitems, struct mem_str *m)
{
	char	*newp;
	size_t	 newsize, fullsize;

	fullsize = size * nitems;
	newsize = m->size + fullsize + 1;

	if (nitems == 0)
		return nitems;

	if ((newp = realloc(m->str, newsize)) == NULL)
		err(1, "realloc");

	m->str = newp;

	memcpy(&(m->str[m->size]), buf, fullsize);
	m->size += fullsize;
	m->str[m->size] = 0;

	return nitems;
}
