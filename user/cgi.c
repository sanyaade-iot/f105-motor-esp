/*
Some random cgi routines. Used in the LED example and the page that returns the entire
flash as a binary. Also handles the hit counter on the main page.
*/

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain
 * this notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */


#include <esp8266.h>
#include "cgi.h"
#include "uptime.h"

//Template code for the counter on the index page.
int ICACHE_FLASH_ATTR tplHome(HttpdConnData *connData, char *token, void **arg)
{
	struct station_config stconf;
	char buff[128];
	u8 mac[6];

	// empty string if no token matches
	buff[0] = 0;

	if (token == NULL) return HTTPD_CGI_DONE;

	if (strcmp(token, "uptime") == 0) {
		// Uptime
		uptime_str(buff);

	} else if (strcmp(token, "heap") == 0) {
		// Free heap
		sprintf(buff, "%u", system_get_free_heap_size());

	} else if (strcmp(token, "wifiMode") == 0) {
		// WiFi mode
		switch (wifi_get_opmode()) {
			case STATION_MODE:   strcpy(buff, "Client"); break;
			case SOFTAP_MODE:    strcpy(buff, "SoftAP"); break;
			case STATIONAP_MODE: strcpy(buff, "STA+AP"); break;
			default: strcpy(buff, "Unknown");
		}

	} else if (strcmp(token, "staSSID") == 0) {
		// Station SSID (if in station mode)
		int opmode = wifi_get_opmode();
		if (opmode != STATION_MODE && opmode != STATIONAP_MODE) {
			strcpy(buff, "N/A"); // no SSID in AP-only mode
		} else {
			wifi_station_get_config(&stconf);
			strcpy(buff, (char*)stconf.ssid);
		}

	} else if (strcmp(token, "staRSSI") == 0) {
		// Signal strength if in Station mode
		int rssi = wifi_station_get_rssi();
		sprintf(buff, "%d", rssi);

	} else if (strcmp(token, "staMAC") == 0) {
		// Station MAC addr
		wifi_get_macaddr(STATION_IF, mac);
		sprintf(buff, MACSTR, MAC2STR(mac));

	} else if (strcmp(token, "apMAC") == 0) {
		// SoftAP MAC addr
		wifi_get_macaddr(SOFTAP_IF, mac);
		sprintf(buff, MACSTR, MAC2STR(mac));

	} else if (strcmp(token, "chipID") == 0) {
		// Chip serial number
		sprintf(buff, "%08x", system_get_chip_id());
	}

	httpdSend(connData, buff, -1);

	return HTTPD_CGI_DONE;
}


// ----

typedef struct {
	uint32_t count_remain;
} RandomNumberState;


//Template code for the counter on the index page.
int ICACHE_FLASH_ATTR tplMultipart(HttpdConnData *connData, char *token, void **arg)
{
	if (token == NULL) {
		if (*arg != NULL) {
			free(*arg);
			*arg = NULL; // mark as already freed
		}
		return HTTPD_CGI_DONE; // cleanup
	}

	if (os_strcmp(token, "numbers") == 0) {
		RandomNumberState *rns = *arg;
		char buff[20];

		if (rns == NULL) {
			//First call to this cgi. Open the file so we can read it.
			rns=(RandomNumberState *)malloc(sizeof(RandomNumberState));
			*arg=rns;

			// parse count
			uint32_t count = 1;
			int len = httpdFindArg(connData->getArgs, "count", buff, sizeof(buff));
			if (len==-1) {
				// no such get arg
			} else {
				count = (uint32_t)atoi(buff);
			}
			rns->count_remain = count;

			info("User wants %d numbers.", count);
		}

		for (int i = 0; i < 100; i++) {
			os_sprintf(buff, "<li>%lu\n", os_random());
			httpdSend(connData, buff, -1);

			if (--rns->count_remain == 0) {
				break;
			}
		}

		if (rns->count_remain == 0) {
			free(rns);
			*arg = NULL; // mark as already freed
			return HTTPD_CGI_DONE;
		}

		return HTTPD_CGI_MORE;
	}

	return HTTPD_CGI_DONE;
}

/*
// better to put it in the fs...

int FLASH_FN cgiRandomNumbers(HttpdConnData *connData) {
	RandomNumberState *rns=connData->cgiData;
	char buff[128];

	if (connData->conn == NULL) {
		//Connection aborted. Clean up.
		if (rns != NULL) free(rns);
		return HTTPD_CGI_DONE;
	}

	if (rns == NULL) {
		//First call to this cgi. Open the file so we can read it.
		rns=(RandomNumberState *)malloc(sizeof(RandomNumberState));
		connData->cgiData=rns;

		// parse count
		uint32_t count = 1;
		int len = httpdFindArg(connData->getArgs, "count", buff, sizeof(buff));
		if (len==-1) {
			// no such get arg
		} else {
			count = (uint32_t)atoi(buff);
		}
		rns->count_remain = count;

		printf("User wants %d numbers.", count);

		httpdStartResponse(connData, 200);
		httpdHeader(connData, "Content-Type", "text/html");
		httpdEndHeaders(connData);

		// start the page

		httpdSend(connData, "<!DOCTYPE html>"
							"<html>"
							"<head>"
							"	<title>Generated page.</title>"
							"	<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\">"
							"</head>"
							"<body>"
							"<div id=\"main\">"
							"<h1>Random numbers:</h1>"
							"<ul>", -1);

		return HTTPD_CGI_MORE;
	}

	// send end of the page
	if (rns->count_remain == 0) {
		httpdSend(connData, "</ul></body></html>", -1);

		free(rns);
		return HTTPD_CGI_DONE;
	}


	// print chunk of data
	for (int i = 0; i < 100; i++) {
		os_sprintf(buff, "<li>%lu\n", os_random());
		httpdSend(connData, buff, -1);

		if (--rns->count_remain == 0) {
			break;
		}
	}

	return HTTPD_CGI_MORE;
}
*/
