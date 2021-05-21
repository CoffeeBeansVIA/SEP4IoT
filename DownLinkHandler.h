#ifndef DOWNLINKHANDLER_H_
#define DOWNLINKHANDLER_H_
#include <lora_driver.h>
#include <status_leds.h>
#include "DownLinkHandler.h"


// Parameters for OTAA join - You have got these in a mail from IHA
#define LORA_appEUI "926F9B5931FCA94C"
#define LORA_appKEY "1D2EB57B831FBDEF807978AE930786E4"

void DL_handler_create();
void DL_receive_task(void *pvParameters);
void DL_update_task(void *pvParameters);
static void _lora_setup();

#endif