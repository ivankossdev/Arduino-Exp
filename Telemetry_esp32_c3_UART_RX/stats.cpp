#include "stats.h"

uint32_t g_lostPackets = 0;
uint32_t g_retries = 0;

void statsInit() {
  g_lostPackets = 0;
  g_retries = 0;
}