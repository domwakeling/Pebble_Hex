#pragma once

#include <pebble.h>
	
void drawdigit(GContext *ctx, int val, GColor fore, GColor back);

/* the only public api is drawdigit - provide a GContext, int (0-9) and fore/background colours
any other value will result in drawing an 8 */
