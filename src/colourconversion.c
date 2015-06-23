#include <pebble.h>
#include "colourconversion.h"

/* change a char (passed by pointer) to a 3-0 reference */
int convert_hex_to_persist(char *hexString) {
	if(strcmp(hexString, "00") == 0) {
		return 3;
	} else if(strcmp(hexString, "55") == 0) {
		return 2;
	} else if(strcmp(hexString, "AA") == 0) {
		return 1;
	} else {
		return 0;
	}
}
	
/* generates a 0-255 RGB code from a 3-0 reference (0 is 255 because persist generates a 0 by default */
int persist_read_to_255(int readval) {
	switch(readval) {
		case 3:
			return 0;
			break;
		case 2:
			return 85;
			break;
		case 1:
			return 170;
			break;
		default:
			return 255;
			break;
	}
}

/* takes a hex string and writes 3-0 references to persist */
void write_colour_to_persist(char *hexColourString, uint32_t key1, uint32_t key2, uint32_t key3) {
	
	int tempInt = (int)strlen(hexColourString);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Received %s with length %d", hexColourString, tempInt);
	
	char redhex[3] = "";
	char greenhex[3] = "";
	char bluehex[3] = "";
	
	char *redhexpointer = redhex;
	char *greenhexpointer = greenhex;
	char *bluehexpointer = bluehex;
	
	// only act if hexColourString is the correct length
	if( strlen(hexColourString) == 6) {
		
		redhex[0] = hexColourString[0];
		redhex[1] = hexColourString[1];
		greenhex[0] = hexColourString[2];
		greenhex[1] = hexColourString[3];
		bluehex[0] = hexColourString[4];
		bluehex[1] = hexColourString[5];
		
		// write the persists
		int key1int = (int)key1;
		int key2int = (int)key2;
		int key3int = (int)key3;
		persist_write_int(key1int, convert_hex_to_persist(redhexpointer));
		persist_write_int(key2int, convert_hex_to_persist(greenhexpointer));
		persist_write_int(key3int, convert_hex_to_persist(bluehexpointer));
	}
	
}