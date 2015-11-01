var initialized = false;

var options = {};

Pebble.addEventListener("ready", function() {
	console.log("HEX: ready called!");
	initialized = true;
});

Pebble.addEventListener("showConfiguration", function() {
	// on call, make sure options represents latest persistant data
	options = JSON.parse(localStorage.getItem("settings"));
	console.log("Options being sent= " + JSON.stringify(options));
	
	// get the watch type, make a console entry and open the website
	var watch_type = getWatchVersion();
	
	if(watch_type >= 3) {
		console.log("showing configuration for basalt");
		Pebble.openURL('http://www.domwakeling.com/pebble/hex/hex_1_5_col.html?'+encodeURIComponent(JSON.stringify(options)));
	} else {
		console.log("showing configuration for aplite");
		Pebble.openURL('http://www.domwakeling.com/pebble/hex/hex_1_5_bw.html?'+encodeURIComponent(JSON.stringify(options)));
	}
});

Pebble.addEventListener("webviewclosed", function(e) {
	// make a console entry
	console.log("configuration closed");

	//Using primitive JSON validity and non-empty check
	if (e.response.charAt(0) == "{" && e.response.slice(-1) == "}" && e.response.length > 5) {
		
		var watch_type = getWatchVersion();
		
		// set options based on the return data, and send to persistant storage
		options = JSON.parse(decodeURIComponent(e.response));
		localStorage.setItem("settings", JSON.stringify(options));
		
		// log the returned info
		console.log("Options = " + JSON.stringify(options));
		
		// we want to make a dictionary from the available information, so get the values as variables ...
		
		var hourcolour;
		var minutecolour;
		var bgcolour;
		var patterncolour;
		
		if( watch_type >= 3) {
			hourcolour = options.hourcolour.toString().slice(2);
			minutecolour = options.minutecolour.toString().slice(2);
			bgcolour = options.bgcolour.toString().slice(2);
			patterncolour = options.patterncolour.toString().slice(2);
			console.log("hour colour selected: " + hourcolour);
			console.log("minute colour selected: " + minutecolour);
			console.log("background colour selected: " + bgcolour);
			console.log("pattern colour selected: " + patterncolour);
		}
		
		var bluetoothshowing;
		if( options.showbluetooth == "checked") {
			bluetoothshowing = "on";
		} else {
			bluetoothshowing = "off";
		}
		console.log("bluetoothshowing: " + bluetoothshowing);
		
		var patternvisible;
		if( options.patternbackground == "checked") {
			patternvisible = "on";
		} else {
			patternvisible = "off";
		}
		var invertbw;
		if( options.invertcolours == "checked") {
			invertbw = "on";
		} else {
			invertbw = "off";
		}
		
		if( watch_type < 3) {
			console.log("patternvisible: " + patternvisible);
			console.log("invert BW colours: " + invertbw);
		}
		
		var dictionary;
		
		if(watch_type >= 3) {
			dictionary = {
				'KEY_SHOW_BLUETOOTH' : bluetoothshowing,
				'KEY_HOUR_COLOUR' : hourcolour,
				'KEY_MINUTE_COLOUR' : minutecolour,
				'KEY_BACKGROUND_COLOUR' : bgcolour,
				'KEY_PATTERN_COLOUR' : patterncolour
			};
		} else {
			dictionary = {
				'KEY_SHOW_BLUETOOTH' : bluetoothshowing,
				'KEY_SHOW_PATTERN' : patternvisible,
				'KEY_INVERT_BW' : invertbw
			};
		}
		
		// ... and send it to the watch
		Pebble.sendAppMessage(dictionary,
			function(e) {
				console.log("Configuation successfuly sent to Pebble");
			},
			function(e) {
				console.log("Error sending configuration to Pebble");
			}
		);
		
	} else {
		// if we failed the test, log that configuation was cancelled
		console.log("Cancelled");
	}
});

function getWatchVersion() {
	// 1 = Pebble OG
	// 2 = Pebble Steel
	// 3 = Pebble Time
	// 3 = Pebble Basalt Emulator (currently Pebble Time)
	// 4 = Pebble Time Steel
	// 5 = Pebble Time Round
	// 5 = Pebble Time Emulator
	
	var watch_version = 1;
 
	if(Pebble.getActiveWatchInfo) {
		// Available for use!
		var watch_name = Pebble.getActiveWatchInfo().model;
 		if (watch_name.indexOf("pebble_time_round") >= 0) {
			watch_version = 5;
		} else if (watch_name.indexOf("qemu_platform_chalk") >= 0) {
			watch_version = 5;
		} else if (watch_name.indexOf("pebble_time_steel") >= 0) {
			watch_version = 4;
		} else if (watch_name.indexOf("pebble_time") >= 0) {
			watch_version = 3;
		} else if (watch_name.indexOf("qemu_platform_basalt") >= 0) {
			watch_version = 3;
		} else if (watch_name.indexOf("pebble_steel") >= 0) {
			watch_version = 2;
		}
		console.log("watch name " + watch_name + ", watch version " + watch_version);
	}
	return watch_version;
}

