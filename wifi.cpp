#include "wifi.hh"
#include "arduino_secrets.h"

int status = WL_IDLE_STATUS;

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)

void wifiBegin(void) {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial1.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial1.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial1.print("Attempting to connect to SSID: ");
    Serial1.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial1.println("Connected to WiFi");
  printWifiStatus();
}

void printWifiStatus(void) {
  // print the SSID of the network you're attached to:
  Serial1.print("SSID: ");
  Serial1.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial1.print("IP Address: ");
  Serial1.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial1.print("signal strength (RSSI):");
  Serial1.print(rssi);
  Serial1.println(" dBm");
}
