#include <pebble.h>
#include "persistboollog.h"

int persist_log_bool(char *input_string, uint32_t key, char *value_name) {
	// make a buffer
	char temp_buffer[4];
	
	// get the value ("on" or "off") and log it				
	snprintf(temp_buffer, sizeof(temp_buffer), "%s", input_string );

	// change patternshow as appropriate
	if (strcmp(temp_buffer,"on") == 0) {

		// store true
		APP_LOG(APP_LOG_LEVEL_INFO, "Received %s 'on', writing to storage", value_name);
		persist_write_bool(key, true);
		return 1;

	} else if (strcmp(temp_buffer, "off") == 0){

		// store false
		APP_LOG(APP_LOG_LEVEL_INFO, "Received %s 'off', writing to storage", value_name);
		persist_write_bool(key, false);
		return 0;

	} else {
		
		APP_LOG(APP_LOG_LEVEL_ERROR, "Error interpreting config - received value %s for %s", temp_buffer, value_name);
		return -1;
	}
}