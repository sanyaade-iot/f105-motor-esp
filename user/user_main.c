/*
 * Based on an example project for ESP-HTTPD.
 *
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain
 * this notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

// library headers
#include <esp8266.h>
#include "httpd.h"
#include "espfs.h"
#include "webpages-espfs.h"
#include "captdns.h"

#include "serial.h"
#include "io.h"
#include "datalink.h"
#include "uptime.h"
#include "routes.h"
#include "fw_version.h"

#include "pers_cfg.h"
#include "wificontrol.h"

extern HttpdBuiltInUrl builtInUrls[];

static ETSTimer prSecondTimer;

/** Timer called each second */
static void FLASH_FN prSecondTimerCb(void *arg)
{
	(void)arg;

	static u8 cnt = 0;
	static u32 last = 0;

	if (++cnt == 3) {
		cnt = 0;
		u32 heap = system_get_free_heap_size();
		dbg("Heap: %u (~ %d)", heap, (heap-last));
		last = heap;
	}

	// we will also try to set up a SBMP connection
	if (sbmp_ep_handshake_status(dlnk_ep) != SBMP_HSK_SUCCESS) {
		sbmp_ep_start_handshake(dlnk_ep);
	}
}


// Some stuff for alternative ESPFS storage methods
#ifdef ESPFS_POS
CgiUploadFlashDef uploadParams = {
	.type = CGIFLASH_TYPE_ESPFS,
	.fw1Pos = ESPFS_POS,
	.fw2Pos = 0,
	.fwSize = ESPFS_SIZE,
};
#define INCLUDE_FLASH_FNS
#endif

#ifdef OTA_FLASH_SIZE_K
CgiUploadFlashDef uploadParams = {
	.type = CGIFLASH_TYPE_FW,
	.fw1Pos = 0x1000,
	.fw2Pos = ((OTA_FLASH_SIZE_K * 1024) / 2) + 0x1000,
	.fwSize = ((OTA_FLASH_SIZE_K * 1024) / 2) - 0x1000,
	.tagName = OTA_TAGNAME
};
#define INCLUDE_FLASH_FNS
#endif


/**
 * Main routine. Initialize stdout, the I/O, filesystem and the webserver and we're done.
 */
void user_init(void)
{
	// set up the debuging output
	serialInit();
	uptime_timer_init();

	banner("*** WiFi Demo - Stepper motor control ***");
	banner_info("(c) Ondrej Hruska, 2016");
	banner_info("Katedra mereni FEL CVUT");
	banner_info("");
	banner_info("Version "FIRMWARE_VERSION", built "__DATE__" at "__TIME__);
	banner_info("HTTPD v."HTTPDVER", SBMP v."SBMP_VER", IoT SDK v."STR(ESP_SDK_VERSION));
	printf(LOG_EOL);

	// reset button etc
	ioInit();

	struct softap_config wificfg;
	wifi_softap_get_config(&wificfg);
	sprintf((char*)wificfg.ssid, "k338_motor_demo");
	wificfg.ssid_len = strlen((char*)wificfg.ssid);
	wifi_softap_set_config(&wificfg);

	// set up SBMP
	datalinkInit();

	// Start the captive portal
	captdnsInit();

	// 0x40200000 is the base address for spi flash memory mapping, ESPFS_POS is the position
	// where image is written in flash that is defined in Makefile.
#ifdef ESPFS_POS
	espFsInit((void*)(0x40200000 + ESPFS_POS));
#else
	espFsInit((void*)(webpages_espfs_start));
#endif

	/* --- Initialize the webserver --- */

	httpdInit(builtInUrls, 80);

	persistent_cfg_load();

	printf(LOG_EOL);
	banner_info("Ready");
	printf(LOG_EOL);

	// don't send updates
	//wificontrol_init();

	os_timer_disarm(&prSecondTimer);
	os_timer_setfn(&prSecondTimer, prSecondTimerCb, NULL);
	os_timer_arm(&prSecondTimer, 1000, 1);
}


// ---- unused funcs removed from sdk to save space ---

// вызывается из phy_chip_v6.o
void ICACHE_FLASH_ATTR chip_v6_set_sense(void)
{
	// ret.n
}

// вызывается из phy_chip_v6.o
int ICACHE_FLASH_ATTR chip_v6_unset_chanfreq(void)
{
	return 0;
}
