#pragma once
	
void drawdigit(GContext *ctx, int val);

/* the only public api is drawdigit - provide a GContext and int (0-9)
any other value will result in drawing an 8 */
