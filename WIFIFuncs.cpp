#include "WIFIFuncs.h"

//---------Variable declaration-----------
//Max address number
const int MaxMAC = 12;
//BLE addresses to scan for 
const char* MACAddress[] = {"F4:92:BF:5D:7C:63", "F4:92:BF:5D:7C:8F", "F4:92:BF:5D:7C:70", "F4:92:BF:5D:80:1C"};
//----------------------------------------

//---------Setup Function-----------------
void WiFiScanSetup(){
  Serial.println("WiFi scan initialized");
  WiFi.mode(WIFI_STA);
  //WiFi.disconnect();
}
//----------------------------------------

//---------Single Scan Function----------
std::vector<WiFiInfo> SingleWiFiScan() {

  std::vector<WiFiInfo> measurement;

  // Matrix to hold information about the networks
  WiFiInfo networks[MaxMAC];
  int numFound = 0;
  // WiFi.scanNetworks will return the number of networks found
  int numNetworks = WiFi.scanNetworks();
  // Iterate through each network found
  for (int i = 0; i < numNetworks; ++i) {
    String currentMAC = WiFi.BSSIDstr(i);

    // Check if the current network's MAC address matches any of the interested MACs
    for (int j = 0; j < sizeof(MACAddress) / sizeof(MACAddress[0]); j++) {
      if (currentMAC.equals(MACAddress[j])) {
        // Store information about the network in the matrix
        networks[numFound].RSSI = WiFi.RSSI(i);
        networks[numFound].MAC = currentMAC;
        measurement.push_back(networks[numFound]);
        numFound++;
        break;        // Break the inner loop once a match is found
      }
    }
  }
  return measurement;
  WiFi.scanDelete(); 
}
//----------------------------------------

//---------Multi Scan Function------------
std::vector<WiFiInfo> WiFiScan(int num) {
  Serial.println("Scanning for WiFi networks...");
  // vector of tuples to hold MAC and RSSI
  std::vector<std::tuple<String, int>> totalRSSI;
  std::map<String, int> macCount;
  int divide = 0;
  // Perform multiple scans
  for (int i = 0; i < num; ++i) {
    // Get results from a single scan
    const std::vector<WiFiInfo>& measurement = SingleWiFiScan();

    // Iterate through each WiFiInfo in the measurement
    for (const WiFiInfo& info : measurement) {
      // Find the existing entry for the MAC address, if any
      macCount[info.MAC]++;
      bool found = false;
      for (auto& [mac, total] : totalRSSI) {
        if (mac == info.MAC) {
          // Add the current RSSI to the existing total RSSI
            total += info.RSSI;
            found = true;
          break;
        }
      }
      // If the MAC address wasn't found, add a new entry
      if (!found) {
        totalRSSI.push_back(std::make_tuple(info.MAC, info.RSSI));
      }
    }
  }

  // Calculate the average RSSI for each MAC address
  std::vector<WiFiInfo> averagedResults;
  for (const auto& [mac, total] : totalRSSI) {
    // Calculate the average RSSI
    int Total = total;
    Total += (num - macCount[mac])*-110;
    int averageRSSI = Total / num;
    //WiFiInfo object with the average RSSI
    WiFiInfo averagedInfo;
    averagedInfo.MAC = mac;
    averagedInfo.RSSI = averageRSSI;

    //Averaged WiFiInfo added to resultvector
    averagedResults.push_back(averagedInfo);
  }

   // Print the count of occurrences for each MAC address
  Serial.println("MAC Address Counts:");
  for (const auto& [mac, count] : macCount) {
    Serial.print("MAC: ");
    Serial.print(mac);
    Serial.print(", Count: ");
    Serial.println(count);
  }

  return averagedResults;
  totalRSSI.clear();
}
//----------------------------------------