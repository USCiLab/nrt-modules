/*
 * libwebsockets-test-server - libwebsockets test implementation
 *
 * Copyright (C) 2010-2011 Andy Green <andy@warmcat.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation:
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/time.h>

#include "libs/libwebsockets/libwebsockets.h"

enum nrt_protocols {
	/* always first */
	PROTOCOL_HTTP = 0,

	PROTOCOL_NRT_WS,

	/* always last */
	DEMO_PROTOCOL_COUNT
};

#define LOCAL_RESOURCE_PATH "/lab/rand/workspace/nrt-modules/Utilities/DesignerServer/files"

/* this protocol server (always the first one) just knows how to do HTTP */
static int callback_http(struct libwebsocket_context *context,
		struct libwebsocket *wsi, enum libwebsocket_callback_reasons reason, void *user, void *in, size_t len)
{
	char client_name[128];
	char client_ip[128];
  char filenamebuffer[1024];
  char *input = (char*)in;

	switch (reason) {
	case LWS_CALLBACK_HTTP:
    printf("Request [%s]\n", input);

    if(input[0] == '/' && input[1] == 0)
      sprintf(filenamebuffer, LOCAL_RESOURCE_PATH"/index.html");
    else
      snprintf(filenamebuffer, 1024, LOCAL_RESOURCE_PATH"%s", input);

		printf("serving HTTP URI %s\n", filenamebuffer);

		if (in && strcmp(input, "/favicon.ico") == 0) {
			if (libwebsockets_serve_http_file(wsi,
			     LOCAL_RESOURCE_PATH"/favicon.ico", "image/x-icon"))
				fprintf(stderr, "Failed to send favicon\n");
			break;
		}

		if (libwebsockets_serve_http_file(wsi, filenamebuffer, ""))
			fprintf(stderr, "Failed to send HTTP file\n");
		break;

  default: break;
	}

	return 0;
}

struct per_session_data__nrt_ws {
	int number;
};

static int
callback_nrt_ws(struct libwebsocket_context *context,
			struct libwebsocket *wsi,
			enum libwebsocket_callback_reasons reason,
					       void *user, void *in, size_t len)
{
	int n;
	unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 512 +
						  LWS_SEND_BUFFER_POST_PADDING];
	unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
	per_session_data__nrt_ws *pss = static_cast<per_session_data__nrt_ws*>(user);

  printf("nrt_ws callback\n");

	switch (reason) {

	case LWS_CALLBACK_ESTABLISHED:
		fprintf(stderr, "callback_dumb_increment: "
						 "LWS_CALLBACK_ESTABLISHED\n");
		break;

	/*
	 * in this protocol, we just use the broadcast action as the chance to
	 * send our own connection-specific data and ignore the broadcast info
	 * that is available in the 'in' parameter
	 */

	case LWS_CALLBACK_BROADCAST:
		n = sprintf((char *)p, "%s", "hi");
		n = libwebsocket_write(wsi, p, n, LWS_WRITE_TEXT);
		if (n < 0) {
			fprintf(stderr, "ERROR writing to socket");
			return 1;
		}
		break;

	case LWS_CALLBACK_RECEIVE:
    printf("Got Request of length %lu [%s]", len, (char*)in);
		break;

	default:
		break;
	}

	return 0;
}



/* list of supported protocols and callbacks */
static struct libwebsocket_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",		/* name */
		callback_http,		/* callback */
		0			/* per_session_data_size */
	},
  {
    "nrt-ws-protocol",
    callback_nrt_ws,
    sizeof(struct per_session_data__nrt_ws)
  },
	{
		NULL, NULL, 0		/* End of list */
	}
};

int main(int argc, char **argv)
{
  int port = 8080;
	unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 1024 + LWS_SEND_BUFFER_POST_PADDING];

  struct libwebsocket_context *context = libwebsocket_create_context(
      port, "", protocols, libwebsocket_internal_extensions, nullptr, nullptr, -1, -1, 0);

  if (context == NULL) 
  {
    fprintf(stderr, "libwebsocket init failed\n");
    return -1;
  }

	buf[LWS_SEND_BUFFER_PRE_PADDING] = 'x';

	int n = libwebsockets_fork_service_loop(context);
	if (n < 0) 
  {
		fprintf(stderr, "Unable to fork service loop %d\n", n);
		return 1;
	}

  while(true)
  {
    sleep(1);
		libwebsockets_broadcast(&protocols[PROTOCOL_NRT_WS],
					&buf[LWS_SEND_BUFFER_PRE_PADDING], 1);
  }

	libwebsocket_context_destroy(context);

  return 0;

}
