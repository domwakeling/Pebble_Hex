#include <pebble.h>
#include "drawdigits.h"
#include "colourconversion.h"
#include "persistboollog.h"

/*** DEBUG MODE ***/
//#define DEBUG

/*** KEY VALUES THROUGH DEFINES ***/									// key values for communication with settings screen via Javascript

#define KEY_SHOW_BLUETOOTH 0
#define KEY_HOUR_COLOUR 1
#define KEY_MINUTE_COLOUR 2
#define KEY_BACKGROUND_COLOUR 3
#define KEY_PATTERN_COLOUR 4
#define KEY_SHOW_PATTERN 5
#define KEY_INVERT_BW 6

/*** KEY VALUES THROUGH DECLARATIONS ***/						// key values for persistant storage

const uint32_t STORAGE_BLUETOOTH_VISIBLE = 1;
const uint32_t STORAGE_HOUR_COLOUR_RED = 2;
const uint32_t STORAGE_HOUR_COLOUR_GREEN = 3;
const uint32_t STORAGE_HOUR_COLOUR_BLUE = 4;
const uint32_t STORAGE_MINUTE_COLOUR_RED = 5;
const uint32_t STORAGE_MINUTE_COLOUR_GREEN = 6;
const uint32_t STORAGE_MINUTE_COLOUR_BLUE = 7;
const uint32_t STORAGE_BG_COLOUR_RED = 8;
const uint32_t STORAGE_BG_COLOUR_GREEN = 9;
const uint32_t STORAGE_BG_COLOUR_BLUE = 10;
const uint32_t STORAGE_PATTERN_COLOUR_RED = 11;
const uint32_t STORAGE_PATTERN_COLOUR_GREEN = 12;
const uint32_t STORAGE_PATTERN_COLOUR_BLUE = 13;
const uint32_t STORAGE_PATTERN_VISIBLE = 14;
const uint32_t STORAGE_INVERT_BW = 15;
const uint32_t STORAGE_FIRST_RUN = 16;

/*** CONSTANTS THROUGH DEFINES - DIGITS ***/	

#define DIGIT_WIDTH 43
#define DIGIT_HEIGHT 63
#define D0_X_OFF -67
#define D1_X_OFF -35
#define D2_X_OFF -4
#define D3_X_OFF 28
#define D_02_Y_OFF -49
#define D_13_Y_OFF -11

/*** CONSTANTS THROUGH DEFINES - "PATH" ***/

#define PATH_SIZE 4
#define PATH_PAD 1

/*** CONSTANTS THROUGH DEFINES - BLUETOOTH LOGO ***/

#define BT_HEIGHT 4
#define BT_WIDTH 4
#define VERT_OFF 4
#define HORZ_OFF 4
#define VERT_OFF_RD 8
#define LEFT_OFF 1

/*** CONSTANTS THROUGH DEFINES - ANIMATION ***/

#define ANIMATION_TIME 750														// animation time must be no more than 1000 ms

/*** CONSTANTS THROUGH DEFINES - MESSAGE SIZES ***/

#define MSG_INBOX 64
#define MSG_OUTBOX 64

/*** DUMMY DECLARATION OF FUNCTIONS ***/
int screen_h();
int screen_w();

/*** DEFINITION OF ANIMATIONDATA STRUCT */

typedef struct AnimationData {
	Layer *layerPointer;
	int layerReference;
	GRect from;
	GRect to;
} AnimationData;

/*** VARIABLES & OBJECTS VIA DECLARATION ***/

// persistent data information
int colours[4][3];
bool bluetoothactive;
bool inverted;
bool patterned;

bool bluetooth_connected = false;

static char time_buffer[] = "0000*";

// arrays  for the animations including bool whether digits should be drawn
static AnimationData animationData[4];
static PropertyAnimation *animations[4][2];
static bool drawDigits[4];

// int to store returned values when persistant logging bools
int bool_log_return;

// char pointer for dealing with colour conversion from hex
char *colour_hex_buffer_pointer;
static Window *main_window;
static Layer *background_layer;
static Layer *digitsLayers[4];
static Layer *bluetooth_layer;
	
/*** HELPER FUNCTIONS ***/

void populate_locals_from_persists() {
	colours[0][0] = persist_read_to_255(persist_read_int(STORAGE_HOUR_COLOUR_RED));
	colours[0][1] = persist_read_to_255(persist_read_int(STORAGE_HOUR_COLOUR_GREEN));
	colours[0][2] = persist_read_to_255(persist_read_int(STORAGE_HOUR_COLOUR_BLUE));
	colours[1][0] = persist_read_to_255(persist_read_int(STORAGE_MINUTE_COLOUR_RED));
	colours[1][1] = persist_read_to_255(persist_read_int(STORAGE_MINUTE_COLOUR_GREEN));
	colours[1][2] = persist_read_to_255(persist_read_int(STORAGE_MINUTE_COLOUR_BLUE));
	colours[2][0] = persist_read_to_255(persist_read_int(STORAGE_BG_COLOUR_RED));
	colours[2][1] = persist_read_to_255(persist_read_int(STORAGE_BG_COLOUR_GREEN));
	colours[2][2] = persist_read_to_255(persist_read_int(STORAGE_BG_COLOUR_BLUE));
	colours[3][0] = persist_read_to_255(persist_read_int(STORAGE_PATTERN_COLOUR_RED));
	colours[3][1] = persist_read_to_255(persist_read_int(STORAGE_PATTERN_COLOUR_GREEN));
	colours[3][2] = persist_read_to_255(persist_read_int(STORAGE_PATTERN_COLOUR_BLUE));
	bluetoothactive = persist_read_bool(STORAGE_BLUETOOTH_VISIBLE);
	inverted = persist_read_bool(STORAGE_INVERT_BW);
	patterned = persist_read_bool(STORAGE_PATTERN_VISIBLE);
}

// function to return the "background" colour dependent on platform
GColor background_colour() {
	#ifdef PBL_COLOR
		// read the current persistant RGB (which will be 0,1,2,3) and convert to 0-255
		int redval = colours[2][0];
		int greenval = colours[2][1];
		int blueval = colours[2][2];
		// generate a temporary colour and return it
		GColor tempColour = GColorFromRGB(redval, greenval, blueval);
		return tempColour;
	#else
		if(inverted) {
			return GColorWhite;
		} else {
			return GColorBlack;
		}
	#endif
}

// function to return the "path" (contrast background) colour dependent on platform
GColor path_colour() {
	#ifdef PBL_COLOR
		// read the current persistant RGB (which will be 0,1,2,3) and convert to 0-255
		int redval = colours[3][0];
		int greenval = colours[3][1];
		int blueval = colours[3][2];
		// generate a temporary colour and return it
		GColor tempColour = GColorFromRGB(redval, greenval, blueval);
		return tempColour;
	#else
		if(patterned && inverted) {
			return GColorBlack;
		} else if(patterned) {
			return GColorWhite;
		} else {
			return background_colour();
		}
	#endif
}

// function to return the "shadow" colour of digits - which is hte same as background colour
GColor stroke_colour() {
	return background_colour();
}

// function to return the "digit" colour dependent on platform and optionally hours/minutes
GColor digit_colour(int layer_ref) {
	#ifdef PBL_COLOR
		if(layer_ref == 0 || layer_ref == 1) {
			// read the current persistant RGB (which will be 0,1,2,3) and convert to 0-255
			int redval = colours[0][0];
			int greenval = colours[0][1];
			int blueval = colours[0][2];
			// generate a temporary colour and return it
			GColor tempColour = GColorFromRGB(redval, greenval, blueval);
			return tempColour;
		} else {
			// read the current persistant RGB (which will be 0,1,2,3) and convert to 0-255
			int redval = colours[1][0];
			int greenval = colours[1][1];
			int blueval = colours[1][2];
			// generate a temporary colour and return it
			GColor tempColour = GColorFromRGB(redval, greenval, blueval);
			return tempColour;
		}	
	#endif
		if(inverted) {
			return GColorBlack;
		} else {
			return GColorWhite;
		}
}

// function to return the screen height
int screen_h() {
	GRect bounds = layer_get_bounds(window_get_root_layer(main_window));
	return bounds.size.h;
}

// function to return the screen width
int screen_w() {
	GRect bounds = layer_get_bounds(window_get_root_layer(main_window));
	return bounds.size.w;
}

// function to return the xPos of layer in either "on" or "animated off" position
int layer_xPos(int digit, int on_off) {
	
	int xMid = screen_w() / 2;
	
	switch(digit) {
		
		case 0:
			if(on_off == 0) {
				return xMid + D0_X_OFF;
			} else if(on_off == 1) {
				return 0 - DIGIT_WIDTH;
			} else {
				return 0;
			}
			break;
		
		case 1:
			return xMid + D1_X_OFF;
			break;
		
		case 2:
			return xMid + D2_X_OFF;
			break;
		
		case 3:
			if(on_off == 0) {
				return xMid + D3_X_OFF;
			} else if(on_off == 1) {
				return screen_w();;
			} else {
				return 0;
			}
			break;
		
		default:
			return 0;
			break;
		
	}
	
}

// function to return the yPos of layer in either "on" or "animated off" position
int layer_yPos(int digit, int on_off) {
	
	int yMid = screen_h() / 2;
	
	switch(digit) {
	
		case 0:
			return yMid + D_02_Y_OFF;
			break;
		
		case 1:
			if(on_off == 0) {
				return yMid + D_13_Y_OFF;
			} else if(on_off == 1) {
				return screen_h();
			} else {
				return 0;
			}
			break;
		
		case 2:
			if(on_off == 0) {
				return yMid + D_02_Y_OFF;
			} else if(on_off == 1) {
				return 0 - DIGIT_HEIGHT;
			} else {
				return 0;
			}
			break;
		
		case 3:
			return yMid + D_13_Y_OFF;
			break;
		
		default:
			return 0;
			break;
		
	}
	
}

// function to get the digitsLayers[] index from a layer
int layer_ref_int(Layer *l) {
	for(int i = 0; i < 4; i++) {
		if( l == digitsLayers[i]) return i;
	}
	return -1;		// this statement should never be reached, and will crash the app
}

/*** DIGIT ANIMATION ***/	

// the "draw and animate in" callback method
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

// the "animate out" method
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

// set details for the animation in an animationData struct before calling the first animation
void animateOutAndIn(int layer_ref) {

	/* get the frames from and to */
	GRect frameFrom = GRect(layer_xPos(layer_ref,0), layer_yPos(layer_ref,0), DIGIT_WIDTH, DIGIT_HEIGHT);
	GRect frameTo = GRect(layer_xPos(layer_ref,1), layer_yPos(layer_ref,1), DIGIT_WIDTH, DIGIT_HEIGHT);

	/* set info for an AnimationData struct in array */
	animationData[layer_ref].layerPointer = digitsLayers[layer_ref];
	animationData[layer_ref].layerReference = layer_ref;
	animationData[layer_ref].from = frameFrom;
	animationData[layer_ref].to = frameTo;
	
	/* call to the first (out) animation */
	animateOut(layer_ref);
}

/*** LAYER UPDATE PROCS ***/	

void background_update_proc(Layer *l, GContext *ctx) {
	/* fill with the background colour */
	graphics_context_set_fill_color(ctx, background_colour());
	graphics_fill_rect(ctx, GRect(0, 0, screen_w(), screen_h()), 0, GCornerNone);
	
	/* draw rectangles in path colour*/
	graphics_context_set_stroke_color(ctx, path_colour());
	int path_dim = PATH_SIZE + 2 * PATH_PAD;
	int across = screen_w() / path_dim;
	int down = screen_h() / path_dim;
	for(int i = 0; i < across; i++) {
		for(int j = 0; j <  down; j++ )
			graphics_draw_rect(ctx, GRect(path_dim * i + 1, path_dim * j + 1, PATH_SIZE, PATH_SIZE));
	}
}

void digits_update_proc(Layer *l, GContext *ctx) {
	/* set compositing mode "in case" */
	graphics_context_set_compositing_mode(ctx, GCompOpSet);

	/* get the int from time_buffer char */
	int layer_ref = layer_ref_int(l);

	/* get the number to be drawn */
	int val = (int)time_buffer[layer_ref] - 48;
	
	/* check whether bool tells us to draw the digit, if so draw it */
	if(drawDigits[layer_ref]) {
		drawdigit(ctx, val, digit_colour(layer_ref_int(l)), stroke_colour());
	}
}

void bluetooth_update_proc(Layer *l, GContext *ctx) {
	
	// check whether we are configured to draw bluetooth
	bool drawing_bluetooth = persist_read_bool(STORAGE_BLUETOOTH_VISIBLE);
	
	if (drawing_bluetooth && bluetooth_connected) {
		
		// "stroke" the background
		graphics_context_set_fill_color(ctx, background_colour());
		graphics_context_set_stroke_color(ctx, background_colour());
	
		// stroke an outline ...
		graphics_fill_rect(ctx, GRect(LEFT_OFF,0,2 * (BT_WIDTH + HORZ_OFF) - LEFT_OFF, 4 * BT_HEIGHT + 2 * VERT_OFF), 5, GCornersAll);
		
		// set context fill and stroke to hour colour
		graphics_context_set_fill_color(ctx, digit_colour(0));
		graphics_context_set_stroke_color(ctx, digit_colour(0));

		// stroke bluetooth
		graphics_draw_line(ctx, GPoint(BT_WIDTH + HORZ_OFF, VERT_OFF), GPoint(2 * BT_WIDTH + HORZ_OFF, BT_HEIGHT + VERT_OFF));
		graphics_draw_line(ctx, GPoint(2 * BT_WIDTH + HORZ_OFF, BT_HEIGHT + VERT_OFF), GPoint(BT_WIDTH + HORZ_OFF, 2 * BT_HEIGHT + VERT_OFF));
		graphics_draw_line(ctx, GPoint(BT_WIDTH + HORZ_OFF, 2 * BT_HEIGHT + VERT_OFF), GPoint(2 * BT_WIDTH + HORZ_OFF, 3 * BT_HEIGHT + VERT_OFF));
		graphics_draw_line(ctx, GPoint(2 * BT_WIDTH + HORZ_OFF, 3 * BT_HEIGHT + VERT_OFF), GPoint( BT_WIDTH + HORZ_OFF, 4 * BT_HEIGHT + VERT_OFF));
		graphics_draw_line(ctx, GPoint(BT_WIDTH + HORZ_OFF, VERT_OFF), GPoint(BT_WIDTH + HORZ_OFF, 4 * BT_HEIGHT + VERT_OFF));
		graphics_draw_line(ctx, GPoint(BT_WIDTH + HORZ_OFF, 2 * BT_HEIGHT + VERT_OFF), GPoint(HORZ_OFF , BT_HEIGHT + VERT_OFF));
		graphics_draw_line(ctx, GPoint(BT_WIDTH + HORZ_OFF, 2 * BT_HEIGHT + VERT_OFF), GPoint(HORZ_OFF, 3 * BT_HEIGHT + VERT_OFF));
	}
	
}

/*** DEAL WITH CHANGING TIME ***/	

// update the time in time_buffer, called from tick_handler - putting it on screen comes naturally elsewhere
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

// tick handler - call animation and/or update time_buffer as appropriate
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

/*** WINDOW HANDLERS ***/	

void main_window_load() {
	/* Get a layer for the window to simplify adding layers */
	Layer *window_layer = window_get_root_layer(main_window);
	
	/* set drawDigits bools to false - don't draw digits before animating out for first time */
	for(int i = 0; i < 4; i++) {
		drawDigits[i] = false;
	}
	
	/* if this is the first time our watchface has loaded, set some configuration options */
	if(persist_read_bool(STORAGE_FIRST_RUN) == false) {
		// default options for all configurations ...
		persist_write_bool(STORAGE_BLUETOOTH_VISIBLE, false);
		persist_write_int(STORAGE_HOUR_COLOUR_RED, 0);
		persist_write_int(STORAGE_HOUR_COLOUR_GREEN, 0);
		persist_write_int(STORAGE_HOUR_COLOUR_BLUE, 0);
		persist_write_int(STORAGE_MINUTE_COLOUR_RED, 0);
		persist_write_int(STORAGE_MINUTE_COLOUR_GREEN, 0);
		persist_write_int(STORAGE_MINUTE_COLOUR_BLUE, 0);
		persist_write_int(STORAGE_BG_COLOUR_RED, 3);
		persist_write_int(STORAGE_BG_COLOUR_GREEN, 3);
		persist_write_int(STORAGE_BG_COLOUR_BLUE, 3);
		persist_write_int(STORAGE_PATTERN_COLOUR_RED, 2);
		persist_write_int(STORAGE_PATTERN_COLOUR_GREEN, 2);
		persist_write_int(STORAGE_PATTERN_COLOUR_BLUE,2);
		persist_write_bool(STORAGE_PATTERN_VISIBLE, true);
		persist_write_bool(STORAGE_INVERT_BW, false);
		// change the persist to true so we don't do this again
		persist_write_bool(STORAGE_FIRST_RUN, true);
	}
	
	#ifdef DEBUG
		persist_write_bool(STORAGE_BLUETOOTH_VISIBLE, true);
	#endif
	
	/* having made sure our persists are correct, read them into local store */
	populate_locals_from_persists();
	
	/* Create the layers */
	background_layer = layer_create(GRect(0, 0, screen_w(), screen_h()));
	#ifdef PBL_ROUND
	bluetooth_layer = layer_create(GRect(screen_w() / 2 + D2_X_OFF - 2 * BT_WIDTH - 4 * HORZ_OFF, VERT_OFF_RD, 2*(BT_WIDTH + HORZ_OFF), 4*BT_HEIGHT + 2*VERT_OFF));
	#else
	bluetooth_layer = layer_create(GRect(screen_w() - 2 * BT_WIDTH - 3 * HORZ_OFF, VERT_OFF, 2*(BT_WIDTH + HORZ_OFF), 4*BT_HEIGHT + 2*VERT_OFF));
	#endif
	for(int i = 0; i < 4; i++) {
		digitsLayers[i] = layer_create(GRect(layer_xPos(i,0), layer_yPos(i,0), DIGIT_WIDTH, DIGIT_HEIGHT));
	}
	
	/* Set update_proc for graphics layers */
	layer_set_update_proc(background_layer, background_update_proc);
	layer_set_update_proc(bluetooth_layer, bluetooth_update_proc);
	for(int i = 0; i < 4; i++) {
		layer_set_update_proc(digitsLayers[i], digits_update_proc);
	}
	
	/* Add all layers to the window */
	layer_add_child(window_layer, background_layer);
	layer_add_child(window_layer, bluetooth_layer);
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
	layer_destroy(bluetooth_layer);
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

/*** BLUETOOTH HANDLER ***/	

void bt_handler(bool connected) {
	// whatever the connected state is, pass it to bluetooth_connected and update time to reflect on screen
	bluetooth_connected = connected;
	layer_mark_dirty(bluetooth_layer);
}

/*** COMMUNICATION HANDLER CALLBACKS ***/	

// inbox received callback, deal with successful receipt of message from phone
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {

	// before we do anything else, make necessary temporary variables (only required for colours)
	char colour_hex_buffer[8];
	colour_hex_buffer_pointer = colour_hex_buffer;
		
	// read first item from the dictionary
	Tuple *t = dict_read_first(iterator);
	
	// now there's more than one key, make sure we haven't passed the last one ...
	while(t != NULL) {
	
		// set bool_log_return to -1, so it defaults to an error behaviour for those keys which use it
		bool_log_return = -1;
		
		// ... then work out which key we're reading
		
		switch(t->key) {
			
			// if it's the bluetooth-showing item
			case KEY_SHOW_BLUETOOTH:
				// use persist_log_bool to log value and use the return value (1 true, 0 false, -1 error)
				bool_log_return = persist_log_bool(t->value->cstring, STORAGE_BLUETOOTH_VISIBLE, "Bluetooth Visible");

				// act on return value as appropriate
				if ( bool_log_return == 1) {
					
					// subscribe to the service, get current status
					bluetooth_connection_service_subscribe(bt_handler);
					APP_LOG(APP_LOG_LEVEL_INFO, "Subscribed to blueooth connection");
					bluetooth_connected = bluetooth_connection_service_peek();
					
				} else if (bool_log_return == 0){
					// unsubscribe
					bluetooth_connection_service_unsubscribe();
					
				} else {
					APP_LOG(APP_LOG_LEVEL_ERROR, "Bluetooth Visibility setting not determined, no action taken");
				}
			
				break;
			
			// colour-picking items, four of them ...
			case KEY_HOUR_COLOUR:
				// only do something if we're using a colour pebble
				#ifdef PBL_COLOR
					snprintf(colour_hex_buffer, sizeof(colour_hex_buffer), "%s", t->value->cstring );
					APP_LOG(APP_LOG_LEVEL_INFO, "Received hour colour as hex code %s, writing to storage", colour_hex_buffer);
					write_colour_to_persist(colour_hex_buffer_pointer, STORAGE_HOUR_COLOUR_RED, STORAGE_HOUR_COLOUR_GREEN, STORAGE_HOUR_COLOUR_BLUE);
					update_time();
				#endif
				break;
			
			case KEY_MINUTE_COLOUR:
				// only do something if we're using a colour pebble
				#ifdef PBL_COLOR
					snprintf(colour_hex_buffer, sizeof(colour_hex_buffer), "%s", t->value->cstring );
					APP_LOG(APP_LOG_LEVEL_INFO, "Received minute colour as hex code %s, writing to storage", colour_hex_buffer);
					write_colour_to_persist(colour_hex_buffer_pointer, STORAGE_MINUTE_COLOUR_RED, STORAGE_MINUTE_COLOUR_GREEN, STORAGE_MINUTE_COLOUR_BLUE);
					update_time();
				#endif
				break;
			
			case KEY_BACKGROUND_COLOUR:
				// only do something if we're using a colour pebble
				#ifdef PBL_COLOR
					snprintf(colour_hex_buffer, sizeof(colour_hex_buffer), "%s", t->value->cstring );
					APP_LOG(APP_LOG_LEVEL_INFO, "Received background colour as hex code %s, writing to storage", colour_hex_buffer);
					write_colour_to_persist(colour_hex_buffer_pointer, STORAGE_BG_COLOUR_RED, STORAGE_BG_COLOUR_GREEN, STORAGE_BG_COLOUR_BLUE);
					update_time();
				#endif
				break;
			
			case KEY_PATTERN_COLOUR:
				// only do something if we're using a colour pebble
				#ifdef PBL_COLOR
					snprintf(colour_hex_buffer, sizeof(colour_hex_buffer), "%s", t->value->cstring );
					APP_LOG(APP_LOG_LEVEL_INFO, "Received pattern colour as hex code %s, writing to storage", colour_hex_buffer);
					write_colour_to_persist(colour_hex_buffer_pointer, STORAGE_PATTERN_COLOUR_RED, STORAGE_PATTERN_COLOUR_GREEN, STORAGE_PATTERN_COLOUR_BLUE);
					update_time();
				#endif
				break;
			
			case KEY_INVERT_BW:
				// only store if our watch is Aplite, not doing anything with returned value of persist_log_bool
				#ifdef PBL_BW
					bool_log_return = persist_log_bool(t->value->cstring, STORAGE_INVERT_BW, "Invert BW Colours");
				#endif
				break;
			
			case KEY_SHOW_PATTERN:
				// not doing anything with returned value of persist_log_bool
				bool_log_return = persist_log_bool(t->value->cstring, STORAGE_PATTERN_VISIBLE, "Show Pattern");
				break;
			
			// if we didn't recognise the key, log it
			default:
				APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognised!", (int)t->key);
				break;
		} 
		
		// Look for next item
    t = dict_read_next(iterator);
	}
	
	// once we finish, update view; set all "drawDigits" false so we don't animate out new colours ...
	populate_locals_from_persists();
	for(int i = 0; i < 4; i++) {
		drawDigits[i] = false;
	}
	layer_mark_dirty(bluetooth_layer);
	update_time();
	for(int i = 0; i < 4; i++) {
			animateOutAndIn(i);
	}
}

// inbox dropped callback - logs a message that there's been an error
static void inbox_dropped_callback(AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped");
}

/*** INIT, DEINIT & MAIN ***/	

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
	
	// register appMessage handlers
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	
	// open AppMessage
	//app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());	// this is now frowned on
	app_message_open(MSG_INBOX, MSG_OUTBOX);
	
	// check whether bluetooth display is active and if so set it up
	if(persist_read_bool(STORAGE_BLUETOOTH_VISIBLE)) {
		bluetooth_connection_service_subscribe(bt_handler);
		bluetooth_connected = bluetooth_connection_service_peek();
	}
	
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
 