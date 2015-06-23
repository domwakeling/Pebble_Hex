#pragma once

/*	PersistBoolLog is an API for easy logging of bools to persistant storage as part of watchface configuration.
 *	
 *	The method takes arguments as follows:
 *		*char input_string - this is expected to be "on" or "off"; expect to provide this from a key using cstring
 *		uint32_t key - this is the storage key value against which the bool is to be logged to persistent storage
 *		*char value_name - this is the name of the configurable element which is being logged, and will be used in APP_LOG calls
 *
 *	The method returns an integer value; the meaning of return values is as follows:
 *		1 : "on" received, key logged as true
 *		0 : "off" received, key logged as false
 *	 -1 : error, unable to properly log the key
 */
	
int persist_log_bool(char *input_string, uint32_t key, char *value_name);