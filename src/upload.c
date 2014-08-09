#include <err.h>
#include <stdlib.h>

#include <curl/curl.h>

#include "upload.h"
#include "replace_str.h"

/*
 * POST the JSON to the new status URL, with a Content-Type of
 * application/json. Do not send the Expect: header, as this fails.
 *
 * First it escapes newlines in the JSON.
 */
void
upload(char *json)
{
	CURL			*handle;
	struct curl_slist	*headers;
	char			*esc_json, errbuf[CURL_ERROR_SIZE];
	char			*content_type;

	headers = NULL;
	content_type = "Content-Type: application/json";

	if ((headers = curl_slist_append(headers, content_type)) == NULL)
		errx(1, "curl_slist_append");

	if ((headers = curl_slist_append(headers, "Expect:")) == NULL)
		errx(1, "curl_slist_append");

	if ((handle = curl_easy_init()) == NULL)
		errx(1, "curl_easy_init failed");

	esc_json = replace_str(json, "\n", "\\n");

	curl_easy_setopt(handle, CURLOPT_URL, CREATE_STATUS);
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, esc_json);
	curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, &errbuf);
	curl_easy_setopt(handle, CURLOPT_VERBOSE, 0);

	if (curl_easy_perform(handle) != 0)
		errx(2, errbuf);

	free(esc_json);
	curl_easy_cleanup(handle);
	curl_slist_free_all(headers);
}
