#include "global_declarations.h"
#include <lora_driver.h>
#include <status_leds.h>
#include "SensorDataPackage.h"
#include "upLinkHandler.h"

#ifndef UPLINKHANDLER_H_
#define UPLINKHANDLER_H_

// Parameters for OTAA join - You have got these in a mail from IHA
#define LORA_appEUI "926F9B5931FCA94C"
#define LORA_appKEY "1D2EB57B831FBDEF807978AE930786E4"

void UL_handler_create(MessageBufferHandle_t _uplinkMessageBuffer );
void UL_handler_receive( void *pvParameters );
static void _lora_setup();

#endif 