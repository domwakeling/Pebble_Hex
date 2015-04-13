#include <pebble.h>
#include "drawdigits.h"

/* Custom drawing function passing in GPathInfo; prevent repeating create/destory in each segment */
void custom_draw_gpathinfo(GContext *ctx, GPathInfo *pathinfo) {
	GPath *t_path = gpath_create(pathinfo);
	gpath_draw_filled(ctx, t_path);
	gpath_destroy(t_path);
}

/* Individual functions defining GPathInfo and passng to the custom_draw_gpathinfo */

void draw_outline(GContext *ctx) {
	GPathInfo TEMP = {
		.num_points = 6,
		.points = (GPoint[]) { {10,0}, {30,0}, {40,30}, {30,60}, {10,60}, {0,30} }
	};
	custom_draw_gpathinfo(ctx, &TEMP);
}

void draw_top_bar(GContext *ctx) {
	GPathInfo TEMP = {
		.num_points = 4,
		.points = (GPoint[]) { {10,0}, {30,0}, {32,6}, {8,6} }
	};
	custom_draw_gpathinfo(ctx, &TEMP);
}

void draw_mid_bar(GContext *ctx) {
	GPathInfo TEMP = {
		.num_points = 6,
		.points = (GPoint[]) { {0,30}, {2,27}, {38,27}, {40,30}, {38,33}, {2,33} }
	};
	custom_draw_gpathinfo(ctx, &TEMP);
}

void draw_bot_bar(GContext *ctx) {
	GPathInfo TEMP = {
		.num_points = 4,
		.points = (GPoint[]) { {10,60}, {30,60}, {32,54}, {8, 54} }
	};
	custom_draw_gpathinfo(ctx, &TEMP);
}

void draw_top_left(GContext *ctx) {
	GPathInfo TEMP = {
		.num_points = 4,
		.points = (GPoint[]) { {0,30}, {10,0}, {15,0}, {5,30} }
	};
	custom_draw_gpathinfo(ctx, &TEMP);
}

void draw_top_right(GContext *ctx) {
	GPathInfo TEMP = {
		.num_points = 4,
		.points = (GPoint[]) { {40,30}, {30,0}, {25,0}, {35,30} }
	};
	custom_draw_gpathinfo(ctx, &TEMP);
}

void draw_bot_left(GContext *ctx) {
	GPathInfo TEMP = {
		.num_points = 4,
		.points = (GPoint[]) { {0,30}, {10,60}, {15,60}, {5,30} }
	};
	custom_draw_gpathinfo(ctx, &TEMP);
}

void draw_bot_right(GContext *ctx) {
	GPathInfo TEMP = {
		.num_points = 4,
		.points = (GPoint[]) { {40,30}, {30,60}, {25,60}, {35,30} }
	};
	custom_draw_gpathinfo(ctx, &TEMP);
}

/* *** Functions to draw numbers from paths *** */

void draw_zero(GContext *ctx) {
	draw_top_bar(ctx);
	draw_bot_bar(ctx);
	draw_top_left(ctx);
	draw_top_right(ctx);
	draw_bot_left(ctx);
	draw_bot_right(ctx);
}

void draw_one(GContext *ctx) {
	draw_top_right(ctx);
	draw_bot_right(ctx);
}
															 
void draw_two(GContext *ctx) {
	draw_top_bar(ctx);
	draw_mid_bar(ctx);
	draw_bot_bar(ctx);
	draw_top_right(ctx);
	draw_bot_left(ctx);
}

void draw_three(GContext *ctx) {
	draw_top_bar(ctx);
	draw_mid_bar(ctx);
	draw_bot_bar(ctx);
	draw_top_right(ctx);
	draw_bot_right(ctx);
}

void draw_four(GContext *ctx) {
	draw_mid_bar(ctx);
	draw_top_left(ctx);
	draw_top_right(ctx);
	draw_bot_right(ctx);
}

void draw_five(GContext *ctx) {
	draw_top_bar(ctx);
	draw_mid_bar(ctx);
	draw_bot_bar(ctx);
	draw_top_left(ctx);
	draw_bot_right(ctx);
}

void draw_six(GContext *ctx) {
	draw_top_bar(ctx);
	draw_mid_bar(ctx);
	draw_bot_bar(ctx);
	draw_top_left(ctx);
	draw_bot_left(ctx);
	draw_bot_right(ctx);
}

void draw_seven(GContext *ctx) {
	draw_top_bar(ctx);
	draw_top_right(ctx);
	draw_bot_right(ctx);
}

void draw_eight(GContext *ctx) {
	draw_top_bar(ctx);
	draw_mid_bar(ctx);
	draw_bot_bar(ctx);
	draw_top_left(ctx);
	draw_top_right(ctx);
	draw_bot_left(ctx);
	draw_bot_right(ctx);
}

void draw_nine(GContext *ctx) {
	draw_top_bar(ctx);
	draw_mid_bar(ctx);
	draw_bot_bar(ctx);
	draw_top_left(ctx);
	draw_top_right(ctx);
	draw_bot_right(ctx);
}


/* *** Public function to call relevant draw function *** */

void drawdigit(GContext *ctx, int val) {
	switch(val) {
		case 0:
			draw_zero(ctx);
			break;
		case 1:
			draw_one(ctx);
			break;
		case 2:
			draw_two(ctx);
			break;
		case 3:
			draw_three(ctx);
			break;
		case 4:
			draw_four(ctx);
			break;
		case 5:
			draw_five(ctx);
			break;
		case 6:
			draw_six(ctx);
			break;
		case 7:
			draw_seven(ctx);
			break;
		case 8:
			draw_eight(ctx);
			break;
		case 9:
			draw_nine(ctx);
			break;
		default:
			draw_eight(ctx);
			break;
	}
}