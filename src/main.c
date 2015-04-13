#include <pebble.h>
#include "drawdigits.h"

#define DIGIT_WIDTH 41
#define DIGIT_HEIGHT 61
#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 168
#define ANIMATION_TIME 500

typedef struct AnimationData {
	Layer *layerPointer;
	int layerReference;
	GRect from;
	GRect to;
} AnimationData;
	
static Window *main_window;
static Layer *background_layer;
static Layer *digitsLayers[4];

static char time_buffer[] = "0000*";

static int layerXPos[4][2] = {{4,0 - DIGIT_WIDTH}, {36,36}, {67,67}, {99,SCREEN_WIDTH}};
static int layerYPos[4][2] = {{35,35}, {73,SCREEN_HEIGHT}, {35,0 - DIGIT_HEIGHT}, {73,73}};

static AnimationData animationData[4];
static PropertyAnimation *animations[4][2];
static bool drawDigits[4];


/* background update procedure - sets background to be black */
void background_update_proc(Layer *l, GContext *ctx) {
	/* make background black */
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, GRect(0, 0, 144, 168), 0, GCornerNone);
}


/* helper method to get the digitsLayers[] index from a layer */
int layer_ref_int(Layer *l) {
	for(int i = 0; i < 4; i++) {
		if( l == digitsLayers[i]) return i;
	}
	return -1;		// this statement should never be reached, and will crash the app
}


/* the "draw and animate in" method */
void animateAndIn(Animation *animation, bool finished, void *data) {
	/* get a temporary copy of the data, cast as AnimationData so we can read the properties */
	AnimationData *tempData = (AnimationData *)data;
	
	/* set drawDigits to true for this character so it will be drawn */
	drawDigits[tempData->layerReference] = true;
	
	/* mark the layer dirty to actually draw the characters */
	layer_mark_dirty(digitsLayers[tempData->layerReference]);
	
	/* destroy the old in animation, create new in animation */
	property_animation_destroy(animations[tempData->layerReference][1]);
	animations[tempData->layerReference][1] = property_animation_create_layer_frame(tempData->layerPointer, &tempData->to, &tempData->from);
	
	/* set the speed */
	animation_set_duration((Animation *)animations[tempData->layerReference][1], ANIMATION_TIME);
	
	/* schedule the in animation */
	animation_schedule((Animation *)animations[tempData->layerReference][1]);
}


/* the "animate out" method */
void animateOut(int layer_ref) {
	/* get a temporary copy of the animation data from the layer_ref */
	AnimationData *tempData = &animationData[layer_ref];
	
	/* destory old out animation, create new out animation */
	property_animation_destroy(animations[layer_ref][0]);
	animations[layer_ref][0] = property_animation_create_layer_frame(tempData->layerPointer, &tempData->from, &tempData->to);

	/* set the speed */
	animation_set_duration((Animation *)animations[layer_ref][0], ANIMATION_TIME);
	
	/* set the stopped handler to "draw and animate in" */
	animation_set_handlers((Animation *) animations[layer_ref][0], (AnimationHandlers) {
		.started = (AnimationStartedHandler) NULL,
		.stopped = (AnimationStoppedHandler) animateAndIn,
	}, tempData);
	
	/* schedule the out animation */
	animation_schedule((Animation *)animations[layer_ref][0]);
}


/* sets details for the animation (in animationData struct) before calling the first animation */
void animateOutAndIn(int layer_ref) {

	/* get the frames from and to */
	GRect frameFrom = GRect(layerXPos[layer_ref][0], layerYPos[layer_ref][0], DIGIT_WIDTH, DIGIT_HEIGHT);
	GRect frameTo = GRect(layerXPos[layer_ref][1], layerYPos[layer_ref][1], DIGIT_WIDTH, DIGIT_HEIGHT);

	/* set info for an AnimationData struct in array */
	animationData[layer_ref].layerPointer = digitsLayers[layer_ref];
	animationData[layer_ref].layerReference = layer_ref;
	animationData[layer_ref].from = frameFrom;
	animationData[layer_ref].to = frameTo;
	
	/* call to the first (out) animation */
	animateOut(layer_ref);
}

void digits_update_proc(Layer *l, GContext *ctx) {
	/* set context fill and stroke */
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_context_set_compositing_mode(ctx, GCompOpSet);

	/* get the int from time_buffer char */
	int layer_ref = layer_ref_int(l);

	/* get the number to be drawn */
	int val = (int)time_buffer[layer_ref] - 48;
	
	/* check whether bool tells us to draw the digit, if so draw it */
	if(drawDigits[layer_ref]) {
		drawdigit(ctx, val);
	}
}

void update_time() {
	/* Get a tm struct */
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
		
	/* get the time into the time_buffer; respects 12/24 hours */
	if (clock_is_24h_style()) {
		strftime(time_buffer, sizeof(time_buffer), "%H%M", tick_time);
	} else {
		strftime(time_buffer, sizeof(time_buffer), "%I%M", tick_time);
	}
	
	/* animate the digits */
	for(int i = 0; i < 4; i++) {
		animateOutAndIn(i);
	}
}


void tick_handler(struct tm * time_time, TimeUnits units_changed) {
	update_time();
}


void main_window_load() {
	/* Get a layer for the window to simplify adding layers */
	Layer *window_layer = window_get_root_layer(main_window);
	
	/* set drawDigits bools to false - don't draw digits before animating out for first time */
	for(int i = 0; i < 4; i++) {
		drawDigits[i] = false;
	}
	
	/* Create the layers */
	background_layer = layer_create(GRect(0, 0, 144, 168));
	
	for(int i = 0; i < 4; i++) {
		digitsLayers[i] = layer_create(GRect(layerXPos[i][0], layerYPos[i][0], DIGIT_WIDTH, DIGIT_HEIGHT));
	}
	
	/* Set update_proc for graphics layers */
	layer_set_update_proc(background_layer, background_update_proc);
	
	for(int i = 0; i < 4; i++) {
		layer_set_update_proc(digitsLayers[i], digits_update_proc);
	}
	
	/* Add all layers to the window */
	layer_add_child(window_layer, background_layer);

	for(int i = 0; i < 4; i++) {
		layer_add_child(window_layer, digitsLayers[i]);
	}
	
	/* Ensure correct time */
	//update_time();
}


void main_window_unload() {
	/* Remove all the layers */
	layer_destroy(background_layer);
	
	for(int i = 0; i < 4; i++) {
		layer_destroy(digitsLayers[i]);
	}
	
	/* destroy animations */
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 2; j++) {
			property_animation_destroy(animations[i][j]);
		}
	}
	
}


void init() {
	/* Create main window */
	main_window = window_create();
	
	/* Set WindowHandlers */
	window_set_window_handlers(main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload,
	});
	
	/* Subscribe to tick_timer */
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	
	/* Push to stack */
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
