#ifndef INCLUDE_WIFI_WIFIMANAGER_H_
#define INCLUDE_WIFI_WIFIMANAGER_H_

#include <ESP8266WiFi.h>

namespace wifi {

class WifiManager {
public:
	bool enableSoftAp();
	bool disableSoftAp();
};

extern WifiManager manager;

}


#endif /* INCLUDE_WIFI_WIFIMANAGER_H_ */
