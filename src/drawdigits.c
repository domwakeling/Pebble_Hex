#include <pebble.h>
#include "drawdigits.h"

/* Custom drawing function passing in GPathInfo; prevent repeating create/destory in each segment */
void custom_draw_gpathinfo(GContext *ctx, GPathInfo *pathinfo) {
	GPath *t_path = gpath_create(pathinfo);
	gpath_draw_filled(ctx, t_path);
	gpath_destroy(t_path);
}

/* Individual functions defining GPathInfo and passng to the custom_draw_gpathinfo */

void draw_top_bar(GContext *ctx, GColor fore) {
	GPathInfo TEMP = {
		.num_points = 4,
		.points = (GPoint[]) { {11,1}, {31,1}, {33,7}, {9,7} }
	};
	graphics_context_set_fill_color(ctx, fore);
	custom_draw_gpathinfo(ctx, &TEMP);
}

void draw_top_bar_outline(GContext *ctx, GColor back) {
	GPathInfo TEMP = {
		.num_points = 4,
		.points = (GPoint[]) { {10,0}, {32,0}, {34,8}, {8,8} }
	};
	graphics_context_set_fill_color(ctx, back);
	custom_draw_gpathinfo(ctx, &TEMP);
}

void draw_mid_bar(GContext *ctx, GColor fore) {
	GPathInfo TEMP = {
		.num_points = 6,
		.points = (GPoint[]) { {1,31}, {3,28}, {39,28}, {41,31}, {39,34}, {3,34} }
	};
	graphics_context_set_fill_color(ctx, fore);
	custom_draw_gpathinfo(ctx, &TEMP);
}

void draw_mid_bar_outline(GContext *ctx, GColor back) {
	GPathInfo TEMP = {
		.num_points = 6,
		.points = (GPoint[]) { {0,31}, {2,27}, {40,27}, {42,31}, {40,35}, {2,35} }
	};
	graphics_context_set_fill_color(ctx, back);
	custom_draw_gpathinfo(ctx, &TEMP);
}


void draw_bot_bar(GContext *ctx, GColor fore) {
	GPathInfo TEMP = {
		.num_points = 4,
		.points = (GPoint[]) { {9, 55}, {33,55}, {31,61}, {11,61} }
	};
	graphics_context_set_fill_color(ctx, fore);
	custom_draw_gpathinfo(ctx, &TEMP);
}

void draw_bot_bar_outline(GContext *ctx, GColor back) {
	GPathInfo TEMP = {
		.num_points = 4,
		.points = (GPoint[]) { {8, 54}, {34,54}, {32,62}, {10,62} }
	};
	graphics_context_set_fill_color(ctx, back);
	custom_draw_gpathinfo(ctx, &TEMP);
}

void draw_top_left(GContext *ctx, GColor fore) {
	GPathInfo TEMP = {
		.num_points = 4,
		.points = (GPoint[]) { {1,31}, {11,1}, {16,1}, {6,31} }
	};
	graphics_context_set_fill_color(ctx, fore);
	custom_draw_gpathinfo(ctx, &TEMP);
}

void draw_top_left_outline(GContext *ctx, GColor back) {
	GPathInfo TEMP = {
		.num_points = 4,
		.points = (GPoint[]) { {0,32}, {10,0}, {17,0}, {7,32} }
	};
	graphics_context_set_fill_color(ctx, back);
	custom_draw_gpathinfo(ctx, &TEMP);
}

void draw_top_right(GContext *ctx, GColor fore) {
	GPathInfo TEMP = {
		.num_points = 4,
		.points = (GPoint[]) { {41,31}, {31,1}, {26,1}, {36,31} }
	};
	graphics_context_set_fill_color(ctx, fore);
	custom_draw_gpathinfo(ctx, &TEMP);
}

void draw_top_right_outline(GContext *ctx, GColor back) {
	GPathInfo TEMP = {
		.num_points = 4,
		.points = (GPoint[]) { {42,32}, {32,0}, {25,0}, {35,32} }
	};
	graphics_context_set_fill_color(ctx, back);
	custom_draw_gpathinfo(ctx, &TEMP);
}

void draw_bot_left(GContext *ctx, GColor fore) {
	GPathInfo TEMP = {
		.num_points = 4,
		.points = (GPoint[]) { {1,31}, {11,61}, {16,61}, {6,31} } 
	};
	graphics_context_set_fill_color(ctx, fore);
	custom_draw_gpathinfo(ctx, &TEMP);
}

void draw_bot_left_outline(GContext *ctx, GColor back) {
	GPathInfo TEMP = {
		.num_points = 4,
		.points = (GPoint[]) { {0,30}, {10,62}, {17,62}, {7,30} }
	};
	graphics_context_set_fill_color(ctx, back);
	custom_draw_gpathinfo(ctx, &TEMP);
}

void draw_bot_right(GContext *ctx, GColor fore) {
	GPathInfo TEMP = {
		.num_points = 4,
		.points = (GPoint[]) { {36,31}, {41,31}, {31,61}, {26,61} }
	};
	graphics_context_set_fill_color(ctx, fore);
	custom_draw_gpathinfo(ctx, &TEMP);
}

void draw_bot_right_outline(GContext *ctx, GColor back) {
	GPathInfo TEMP = {
		.num_points = 4,
		.points = (GPoint[]) { {35,30}, {42,30}, {32,62}, {25,62} }
	};
	graphics_context_set_fill_color(ctx, back);
	custom_draw_gpathinfo(ctx, &TEMP);
}

/* *** Functions to draw numbers from paths *** */

void draw_zero(GContext *ctx, GColor fore, GColor back) {
	draw_top_bar_outline(ctx, back);
	draw_bot_bar_outline(ctx, back);
	draw_top_left_outline(ctx, back);
	draw_top_right_outline(ctx, back);
	draw_bot_left_outline(ctx, back);
	draw_bot_right_outline(ctx, back);
	draw_top_bar(ctx, fore);
	draw_bot_bar(ctx, fore);
	draw_top_left(ctx, fore);
	draw_top_right(ctx, fore);
	draw_bot_left(ctx, fore);
	draw_bot_right(ctx, fore);
}

void draw_one(GContext *ctx, GColor fore, GColor back) {
	draw_top_right_outline(ctx, back);
	draw_bot_right_outline(ctx, back);
	draw_top_right(ctx, fore);
	draw_bot_right(ctx, fore);
}
															 
void draw_two(GContext *ctx, GColor fore, GColor back) {
	draw_top_bar_outline(ctx, back);
	draw_mid_bar_outline(ctx, back);
	draw_bot_bar_outline(ctx, back);
	draw_top_right_outline(ctx, back);
	draw_bot_left_outline(ctx, back);
	draw_top_bar(ctx, fore);
	draw_mid_bar(ctx, fore);
	draw_bot_bar(ctx, fore);
	draw_top_right(ctx, fore);
	draw_bot_left(ctx, fore);
}

void draw_three(GContext *ctx, GColor fore, GColor back) {
	draw_top_bar_outline(ctx, back);
	draw_mid_bar_outline(ctx, back);
	draw_bot_bar_outline(ctx, back);
	draw_top_right_outline(ctx, back);
	draw_bot_right_outline(ctx, back);
	draw_top_bar(ctx, fore);
	draw_mid_bar(ctx, fore);
	draw_bot_bar(ctx, fore);
	draw_top_right(ctx, fore);
	draw_bot_right(ctx, fore);
}

void draw_four(GContext *ctx, GColor fore, GColor back) {
	draw_mid_bar_outline(ctx, back);
	draw_top_left_outline(ctx, back);
	draw_top_right_outline(ctx, back);
	draw_bot_right_outline(ctx, back);
	draw_mid_bar(ctx, fore);
	draw_top_left(ctx, fore);
	draw_top_right(ctx, fore);
	draw_bot_right(ctx, fore);
}

void draw_five(GContext *ctx, GColor fore, GColor back) {
	draw_top_bar_outline(ctx, back);
	draw_mid_bar_outline(ctx, back);
	draw_bot_bar_outline(ctx, back);
	draw_top_left_outline(ctx, back);
	draw_bot_right_outline(ctx, back);
	draw_top_bar(ctx, fore);
	draw_mid_bar(ctx, fore);
	draw_bot_bar(ctx, fore);
	draw_top_left(ctx, fore);
	draw_bot_right(ctx, fore);
}

void draw_six(GContext *ctx, GColor fore, GColor back) {
	draw_top_bar_outline(ctx, back);
	draw_mid_bar_outline(ctx, back);
	draw_bot_bar_outline(ctx, back);
	draw_top_left_outline(ctx, back);
	draw_bot_left_outline(ctx, back);
	draw_bot_right_outline(ctx, back);
	draw_top_bar(ctx, fore);
	draw_mid_bar(ctx, fore);
	draw_bot_bar(ctx, fore);
	draw_top_left(ctx, fore);
	draw_bot_left(ctx, fore);
	draw_bot_right(ctx, fore);
}

void draw_seven(GContext *ctx, GColor fore, GColor back) {
	draw_top_bar_outline(ctx, back);
	draw_top_right_outline(ctx, back);
	draw_bot_right_outline(ctx, back);
	draw_top_bar(ctx, fore);
	draw_top_right(ctx, fore);
	draw_bot_right(ctx, fore);
}

void draw_eight(GContext *ctx, GColor fore, GColor back) {
	draw_top_bar_outline(ctx, back);
	draw_mid_bar_outline(ctx, back);
	draw_bot_bar_outline(ctx, back);
	draw_top_left_outline(ctx, back);
	draw_top_right_outline(ctx, back);
	draw_bot_left_outline(ctx, back);
	draw_bot_right_outline(ctx, back);
	draw_top_bar(ctx, fore);
	draw_mid_bar(ctx, fore);
	draw_bot_bar(ctx, fore);
	draw_top_left(ctx, fore);
	draw_top_right(ctx, fore);
	draw_bot_left(ctx, fore);
	draw_bot_right(ctx, fore);
}

void draw_nine(GContext *ctx, GColor fore, GColor back) {
	draw_top_bar_outline(ctx, back);
	draw_mid_bar_outline(ctx, back);
	draw_bot_bar_outline(ctx, back);
	draw_top_left_outline(ctx, back);
	draw_top_right_outline(ctx, back);
	draw_bot_right_outline(ctx, back);
	draw_top_bar(ctx, fore);
	draw_mid_bar(ctx, fore);
	draw_bot_bar(ctx, fore);
	draw_top_left(ctx, fore);
	draw_top_right(ctx, fore);
	draw_bot_right(ctx, fore);
}


/* *** Public function to call relevant draw function *** */

void drawdigit(GContext *ctx, int val, GColor fore, GColor back) {
	switch(val) {
		case 0:
			draw_zero(ctx, fore, back);
			break;
		case 1:
			draw_one(ctx, fore, back);
			break;
		case 2:
			draw_two(ctx, fore, back);
			break;
		case 3:
			draw_three(ctx, fore, back);
			break;
		case 4:
			draw_four(ctx, fore, back);
			break;
		case 5:
			draw_five(ctx, fore, back);
			break;
		case 6:
			draw_six(ctx, fore, back);
			break;
		case 7:
			draw_seven(ctx, fore, back);
			break;
		case 8:
			draw_eight(ctx, fore, back);
			break;
		case 9:
			draw_nine(ctx, fore, back);
			break;
		default:
			draw_eight(ctx, fore, back);
			break;
	}
}