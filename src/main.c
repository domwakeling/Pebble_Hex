#include <pebble.h>
#include "drawdigits.h"

/*****************************************************/
/************* DEFINE CONSTANTS & COLORS *************/	
/*****************************************************/
	
#define DIGIT_WIDTH 43
#define DIGIT_HEIGHT 63
#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 168
#define ANIMATION_TIME 750				// animation time must be no more than 1000 ms

/* function to return the "background" colour dependent on platform */
GColor background_colour() {
	#ifdef PBL_COLOR
		return GColorBlack;
	#endif
	return GColorBlack;
}

/* function to return the "path" colour dependent on platform */
GColor path_colour() {
	#ifdef PBL_COLOR
		return GColorDarkGray;
	#endif
	return GColorWhite;
}

/* function to return the "digit" colour dependent on platform and optionally hours/miutes */
GColor digit_colour(int layer_ref) {
	#ifdef PBL_COLOR
	if(layer_ref == 0 || layer_ref == 1) {
		// hours colour
		return GColorChromeYellow;
	} else {
		// minutes colour
		return GColorChromeYellow;
	}
	#endif
	return GColorWhite;
}

/* define AnimationData struct */
typedef struct AnimationData {
	Layer *layerPointer;
	int layerReference;
	GRect from;
	GRect to;
} AnimationData;

/* define objects */
static Window *main_window;
static Layer *background_layer;
static Layer *digitsLayers[4];

/* define char[] to hold time */
static char time_buffer[] = "0000*";

/* array of ints to define GPoints for the digit GRects - on and off screen */
static int layerXPos[4][2] = {{4,0 - DIGIT_WIDTH}, {36,36}, {67,67}, {99,SCREEN_WIDTH}};
static int layerYPos[4][2] = {{35,35}, {73,SCREEN_HEIGHT}, {35,0 - DIGIT_HEIGHT}, {73,73}};

/* arrays  for the animations including bool whether digits should be drawn */
static AnimationData animationData[4];
static PropertyAnimation *animations[4][2];
static bool drawDigits[4];


/*****************************************************/
/************* DRAWING PROC - BACKGROUND *************/	
/*****************************************************/

/* background update procedure - sets background to be black */
void background_update_proc(Layer *l, GContext *ctx) {
	/* fill with the background colour */
	graphics_context_set_fill_color(ctx, background_colour());
	graphics_fill_rect(ctx, GRect(0, 0, 144, 168), 0, GCornerNone);
	
	/* draw rectangles in path colour*/
	graphics_context_set_stroke_color(ctx, path_colour());
	for(int i = 0; i <24; i++) {
		for(int j = 0; j < 28; j++ )
			graphics_draw_rect(ctx, GRect(6 * i + 1, 6 * j + 1, 4, 4));
	}
}

/*****************************************************/
/********** DRAWING DIGITS - HELPER METHODS **********/	
/*****************************************************/

/* helper method to get the digitsLayers[] index from a layer */
int layer_ref_int(Layer *l) {
	for(int i = 0; i < 4; i++) {
		if( l == digitsLayers[i]) return i;
	}
	return -1;		// this statement should never be reached, and will crash the app
}


/*****************************************************/
/************ DRAWING DIGITS - ANIMATING *************/	
/*****************************************************/

/* the "draw and animate in" callback method  */
void animateAndIn(Animation *animation, bool finished, void *data) {
	/* get a temporary copy of the data, cast as AnimationData so we can read the properties */
	AnimationData *tempData = (AnimationData *)data;
	
	/* set drawDigits to true for this character so it will be drawn */
	drawDigits[tempData->layerReference] = true;
	
	/* mark the layer dirty to actually draw the characters */
	layer_mark_dirty(digitsLayers[tempData->layerReference]);
	
	/* destroy the old in animation if using Aplite, create new "in"" animation */
	#ifdef PBL_BW
		if(animations[tempData->layerReference][1] != NULL) {
			property_animation_destroy(animations[tempData->layerReference][1]);
		}
	#endif
	animations[tempData->layerReference][1] = property_animation_create_layer_frame(tempData->layerPointer, &tempData->to, &tempData->from);
	
	/* set the speed and schedule animation */
	animation_set_duration((Animation *)animations[tempData->layerReference][1], ANIMATION_TIME);
	animation_schedule((Animation *)animations[tempData->layerReference][1]);
}


/* the "animate out" method */
void animateOut(int layer_ref) {
	/* get a temporary copy of the animation data from the layer_ref */
	AnimationData *tempData = &animationData[layer_ref];
	
	/* destroy old out animation if using Aplite, create new out animation */
	#ifdef PBL_BW
	if(animations[layer_ref][0] != NULL) {
		property_animation_destroy(animations[layer_ref][0]);
	}
	#endif
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


/*****************************************************/
/*********** DRAWING DIGITS - UPDATE PROC ************/	
/*****************************************************/

/* update proc for digits */
void digits_update_proc(Layer *l, GContext *ctx) {
	/* set compositing mode "in case" */
	graphics_context_set_compositing_mode(ctx, GCompOpSet);

	/* get the int from time_buffer char */
	int layer_ref = layer_ref_int(l);

	/* get the number to be drawn */
	int val = (int)time_buffer[layer_ref] - 48;
	
	/* check whether bool tells us to draw the digit, if so draw it */
	if(drawDigits[layer_ref]) {
		drawdigit(ctx, val, digit_colour(layer_ref_int(l)), background_colour());
	}
}


/*****************************************************/
/************* DEAL WITH CHANGING TIME ***************/	
/*****************************************************/

/* update the time in time_buffer - called in tick handler */
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
}

/* tick handler - call animation and/or update time_buffer as appropriate*/
void tick_handler(struct tm * tick_time, TimeUnits units_changed) {
	/* if secs == 59, queue animation so we're out at secs == 0 */
	if(tick_time->tm_sec == 59) {
		
		/* pause for balance so animate in starts at 00 - why ANIMATION_TIME must be no more than 1000ms */
		psleep(1000 - ANIMATION_TIME);
		
		/* start animating the digits */
		for(int i = 0; i < 4; i++) {
			animateOutAndIn(i);
		}
	} else if(tick_time->tm_sec == 0) {
		/* otherwise, if we're at the second, update time_buffer so time is drawn correctly when animating in */
		update_time();
	}
}


/*****************************************************/
/************* WINDOW HANDLER CALLBACKS **************/	
/*****************************************************/

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
	
	/* Ensure correct time - needed because update_time not called by tick timer unless tm_secs==0 */
	update_time();
	for(int i = 0; i < 4; i++) {
			animateOutAndIn(i);
	}
}


void main_window_unload() {
	/* Remove all the layers */
	layer_destroy(background_layer);
	
	for(int i = 0; i < 4; i++) {
		layer_destroy(digitsLayers[i]);
	}
	
	/* destroy animations if we're using Aplite */
	#ifdef PBL_BW
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 2; j++) {
			property_animation_destroy(animations[i][j]);
		}
	}
	#endif
}


/*****************************************************/
/*************** INIT, DEINIT & MAIN *****************/	
/*****************************************************/

void init() {
	/* Create main window */
	main_window = window_create();
	
	/* Set WindowHandlers */
	window_set_window_handlers(main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload,
	});
	
	/* Subscribe to tick_timer */
	tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
	
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
