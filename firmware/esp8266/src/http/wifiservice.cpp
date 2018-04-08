#include "http/wifiservice.h"
#include "http/webhandlers.h"
#include "http/basicresponses.h"
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

extern "C" {
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "user_interface.h"

}

namespace http {

WifiService::WifiService( AsyncWebServer* server, const String& path, io::Display* display ) {

	AsyncMethodWebHandler<WifiService>::Factory handlers(this, display);

	String softApPath = path +  "/softap";
	server->addHandler( handlers.create( softApPath, HTTP_GET, &WifiService::handleGetSoftApConfig ) );
	server->addHandler( handlers.create( softApPath + "/state", HTTP_PUT, &WifiService::handleSetSoftApEnabled ) );
	server->addHandler( handlers.create( softApPath + "/bss", HTTP_PUT, &WifiService::handleSetSoftApBss ) );

	String stationPath = path +  "/station";
	server->addHandler( handlers.create( stationPath, HTTP_GET, &WifiService::handleGetStationConfig ) );
	server->addHandler( handlers.create( stationPath + "/state", HTTP_PUT, &WifiService::handleSetStationEnabled ) );
	server->addHandler( handlers.create( stationPath + "/bss", HTTP_PUT, &WifiService::handleSetStationBss ) );
	server->addHandler( handlers.create( stationPath + "/dhcp", HTTP_PUT, &WifiService::handleSetStationDhcp ) );
	server->addHandler( handlers.create( stationPath + "/network", HTTP_PUT, &WifiService::handleSetStationNetwork ) );
}

void WifiService::handleGetSoftApConfig( AsyncWebServerRequest *request ) {
	struct softap_config conf_current;
	if ( !wifi_softap_get_config(&conf_current) ) {
		TRACE_ERROR( F("Reading SoftAP config") );
		sendError( request );
		return;
	}

	struct ip_info ip;
	if ( !wifi_get_ip_info(SOFTAP_IF, &ip) ) {
		TRACE_ERROR( F("Reading IP info") );
		sendError( request );
		return;
	}


	bool enabled = (WiFi.getMode() & WIFI_AP);

	const int BUFFER_SIZE = JSON_OBJECT_SIZE(10) + 21 + 35 + 19*3;
	StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
	JsonObject &ret = jsonBuffer.createObject();
	ret["enabled"] = enabled;
	ret["channel"] = conf_current.channel;
	ret["mac"] = WiFi.softAPmacAddress();

	JsonObject &bss = ret.createNestedObject("bss");
	bss["ssid"] = String(reinterpret_cast<const char*>(conf_current.ssid));

	JsonObject &network = ret.createNestedObject("network");
	network["dhcp"] = true;
	network["ip"] = IPAddress(ip.ip.addr).toString();
	network["netmask"] = IPAddress(ip.netmask.addr).toString();
	network["gateway"] = IPAddress(ip.gw.addr).toString();

	sendBasicJson( request, ret );
}


void WifiService::handleSetSoftApEnabled( AsyncWebServerRequest *request, char* data, size_t len ) {
	DynamicJsonBuffer jsonBuffer;
	const JsonObject& root = jsonBuffer.parseObject( data, 1 );

	bool enable = root["enabled"];
	if ( !WiFi.enableAP(enable) ) {
		TRACE_ERROR( F("%s SoftAP"), enable ? "Enabling" : "Disabling" );
		sendError( request );
		return;
	}

	handleGetSoftApConfig( request );
}


void WifiService::handleSetSoftApBss( AsyncWebServerRequest *request, char* data, size_t len ) {
	DynamicJsonBuffer jsonBuffer;
	const JsonObject& root = jsonBuffer.parseObject( data, 1 );

	String ssid = root["ssid"];
	String password = root["password"];

	if ( !WiFi.softAP(ssid.c_str(), password.c_str()) ) {
		TRACE_ERROR( F("Setting SoftAP SSID") );
		sendError( request );
		return;
	}

	handleGetSoftApConfig( request );
}


static String statusToString( station_status_t status ) {
    switch(status) {
        case STATION_GOT_IP:
            return "Got IP";
        case STATION_NO_AP_FOUND:
            return "No AP found";
        case STATION_CONNECT_FAIL:
        	return "Connect fail";
        case STATION_WRONG_PASSWORD:
        	return "Wrong password";
        case STATION_IDLE:
        	return "idle";
        default:
        	return "disconnected";
    }
}


void WifiService::handleGetStationConfig( AsyncWebServerRequest *request ) {
	struct station_config config;
	if ( !wifi_station_get_config(&config) ) {
		TRACE_ERROR( F("Reading Station config") );
		sendError( request );
		return;
	}

	struct ip_info ip;
	if ( !wifi_get_ip_info(STATION_IF, &ip) ) {
		TRACE_ERROR( F("Reading IP info") );
		sendError( request );
		return;
	}


	bool enabled = (WiFi.getMode() & WIFI_STA);

	const int BUFFER_SIZE = JSON_OBJECT_SIZE(8) + 15 + 35 + 19*3;
	StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
	JsonObject &ret = jsonBuffer.createObject();
	ret["status"] = statusToString( wifi_station_get_connect_status() );
	ret["dhcp"] = (wifi_station_dhcpc_status() == DHCP_STARTED);

	JsonObject &bss = ret.createNestedObject("bss");
	bss["ssid"] = String(reinterpret_cast<const char*>(config.ssid));

	JsonObject &network = ret.createNestedObject("network");
	network["ip"] = IPAddress(ip.ip.addr).toString();
	network["netmask"] = IPAddress(ip.netmask.addr).toString();
	network["gateway"] = IPAddress(ip.gw.addr).toString();
	sendBasicJson( request, ret );
}



void WifiService::handleSetStationEnabled( AsyncWebServerRequest *request, char* data, size_t len ) {
	DynamicJsonBuffer jsonBuffer;
	const JsonObject& root = jsonBuffer.parseObject( data, 1 );

	bool enable = root["enabled"];
	if ( enable ) {
		WiFi.begin();
		WiFi.waitForConnectResult();
	}
	else {
		//Don't call WiFi.disconnect to avoid changing configuration
		ETS_UART_INTR_DISABLE();
		bool ret = wifi_station_disconnect();
		ETS_UART_INTR_ENABLE();

		WiFi.enableSTA(false);
		if ( !ret ) {
			TRACE_ERROR( F("Disabling Station mode") );
			sendError( request );
			return;
		}
	}

	handleGetStationConfig( request );
}


void WifiService::handleSetStationBss( AsyncWebServerRequest *request, char* data, size_t len ) {
	DynamicJsonBuffer jsonBuffer;
	const JsonObject& root = jsonBuffer.parseObject( data, 1 );

	WiFi.begin( root["ssid"].as<const char*>(), root["password"].as<const char*>(), 0, NULL, false );

	handleGetStationConfig( request );
}


void WifiService::handleSetStationDhcp( AsyncWebServerRequest *request, char* data, size_t len ) {
	DynamicJsonBuffer jsonBuffer;
	const JsonObject& root = jsonBuffer.parseObject( data, 1 );

	bool enable = root["enabled"];
	bool isEnabled = wifi_station_dhcpc_status() == DHCP_STARTED;
	bool ret = true;
	if ( enable && !isEnabled ) {
		ret = wifi_station_dhcpc_start();
	}
	else if ( !enable && isEnabled ) {
		ret = wifi_station_dhcpc_stop();
	}

	if ( !ret ) {
		TRACE_ERROR( F("Error %s DHCP"), enable ? "enabling" : "disabling" );
		sendError( request );
		return;
	}

	handleGetStationConfig( request );
}


static bool toIp( AsyncWebServerRequest *request, const char* address, u32_t* res ) {
	IPAddress ip;
	if ( !ip.fromString( address) ) {
		TRACE_ERROR( F("Error parsing IP: %s"), address );
		sendError( request );
		return false ;
	}
	*res = ip;
	return true;
}

static bool toIpInfo( AsyncWebServerRequest *request, const JsonObject& json, struct ip_info* res ) {
	return toIp( request, json["ip"].as<const char*>(), &res->ip.addr ) &&
		toIp( request, json["gateway"].as<const char*>(), &res->gw.addr ) &&
		toIp( request, json["netmask"].as<const char*>(), &res->netmask.addr );

}


void WifiService::handleSetStationNetwork( AsyncWebServerRequest *request, char* data, size_t len ) {
	DynamicJsonBuffer jsonBuffer;
	const JsonObject& root = jsonBuffer.parseObject( data, 1 );

	struct ip_info info;
	toIpInfo( request, root, &info );

	wifi_station_dhcpc_stop();

	if( !wifi_set_ip_info(STATION_IF, &info) ) {
		TRACE_ERROR( F("Error setting IP station") );
		sendError( request );
		return ;
	}

	handleGetStationConfig( request );
}

}
