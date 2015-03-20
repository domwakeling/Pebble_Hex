#include <pebble.h>
#include "drawdigits.h"
	
static Window *main_window;
static Layer *background_layer;
static Layer *hours1_layer, *hours2_layer, *mins1_layer, *mins2_layer;
static char time_buffer[] = "1234";

void background_update_proc(Layer *l, GContext *ctx) {
	// make background black
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, GRect(0, 0, 144, 168), 0, GCornerNone);
}

int layer_ref_int(Layer *l) {
	// get layer ref, used as lookup in time_buffer char
	if(l == hours1_layer) {
		return 0;
	} else if(l == hours2_layer) {
		return 1;
	} else if(l == mins1_layer) {
		return 2;
	} else {
		return 3;
	}
}

void digits_update_proc(Layer *l, GContext *ctx) {
	// set context fill and stroke
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_context_set_compositing_mode(ctx, GCompOpSet);

	// get the int from time_buffer char and draw
	int layer_ref = layer_ref_int(l);

	// get integer value; convert from ascii to numeric
	int val = (int)time_buffer[layer_ref] - 48;
	
	// call drawdigit() to actually draw the characters
	drawdigit(ctx, val);
}

void update_time() {
	// Get a tm struct
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	
	// get the time into the time_buffer; respects 12/24 hours
	if (clock_is_24h_style()) {
		strftime(time_buffer, sizeof(time_buffer), "%H%M", tick_time);
	} else {
		strftime(time_buffer, sizeof(time_buffer), "%I%M", tick_time);
	}
	
	// mark the graphics layers dirty
	layer_mark_dirty(hours1_layer);
	layer_mark_dirty(hours2_layer);
	layer_mark_dirty(mins1_layer);
	layer_mark_dirty(mins2_layer);
}

void tick_handler(struct tm * time_time, TimeUnits units_changed) {
	update_time();
}

void main_window_load() {
	// Get a layer for the window to simplify adding layers
	Layer *window_layer = window_get_root_layer(main_window);
	
	// Create the layers
	background_layer = layer_create(GRect(0, 0, 144, 168));
	hours1_layer = layer_create(GRect(4, 35, 41, 61));
	hours2_layer = layer_create(GRect(36, 73, 41, 61));
	mins1_layer = layer_create(GRect(67, 35, 41, 61));  
	mins2_layer = layer_create(GRect(99, 73, 41, 61));
	
	// Set update_proc for graphics layers
	layer_set_update_proc(hours1_layer, digits_update_proc);
	layer_set_update_proc(hours2_layer, digits_update_proc);
	layer_set_update_proc(mins1_layer, digits_update_proc);
	layer_set_update_proc(mins2_layer, digits_update_proc);
	layer_set_update_proc(background_layer, background_update_proc);
		
	// Add all layers to the window
	layer_add_child(window_layer, background_layer);
	layer_add_child(window_layer, hours1_layer);
	layer_add_child(window_layer, hours2_layer);
	layer_add_child(window_layer, mins1_layer);
	layer_add_child(window_layer, mins2_layer);
	
	// Ensure correct time
	update_time();
}

void main_window_unload() {
	// Remove all the layers
	layer_destroy(hours1_layer);
	layer_destroy(hours2_layer);
	layer_destroy(mins1_layer);
	layer_destroy(mins2_layer);
	layer_destroy(background_layer);
}

void init() {
	// Create main window
	main_window = window_create();
	
	// Set WindowHandlers
	window_set_window_handlers(main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload,
	});
	
	// Subscribe to tick_timer
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	
	// Push to stack
	window_stack_push(main_window, true);
}

void deinit() {
	window_destroy(main_window);
	tick_timer_service_unsubscribe();
}

int main() {
	init();
	app_event_loop();
	deinit();
}
