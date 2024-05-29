#include "Peripherals.h"

bool isScanning = false;
//LED pins
const int SCN = 47;
const int STAT = 48;

//Battery level pins
const int EN = 8;
const int READ = 18;

//grid constants
int maxRow = 16;
int RPColValue[16] = {0, 26, 52, 78, 104, 130, 156, 173, 190, 207, 208, 214, 220, 226, 232, 238};
int maxCol[16] = {25, 25, 25, 25, 25, 25, 16, 16, 16, 0, 5, 5, 5, 5, 5, 5};

//Refrencepoint Format
const char* fingerprint[] = {"F4:92:BF:5D:7C:63", "F4:92:BF:5D:7C:8F", "F4:92:BF:5D:7C:70", "F4:92:BF:5D:80:1C", "48:27:E2:84:3C:ED", "48:27:E2:84:3D:2D", "48:27:E2:84:3D:41", "48:27:E2:84:3D:01", "48:27:E2:84:3C:F9","00:00:00:00:00:00"}; 


//---------Button pins--------------------
  extern const int CB = 7;
  extern const int UB = 16;
  extern const int DB = 6;
  extern const int LB = 5;
  extern const int RB = 15;
//----------------------------------------

//---------Button boolean------------------
  extern bool CBP;
  extern bool UBP;
  extern bool DBP;
  extern bool LBP;
  extern bool RBP;
//----------------------------------------

//---------Peripheral setup---------------
void PeripheralSetup(){
  //Button setup
  pinMode(CB,INPUT_PULLUP);
  pinMode(UB,INPUT_PULLUP);
  pinMode(DB,INPUT_PULLUP);
  pinMode(LB,INPUT_PULLUP);
  pinMode(RB,INPUT_PULLUP);
  //LED setup
  pinMode(SCN,OUTPUT);
  pinMode(STAT,OUTPUT);
  //Battery level setup
  pinMode(EN, OUTPUT);
  pinMode(READ, INPUT);
}
//----------------------------------------

//---------Scan functions---------------
  std::vector<int> ScanButton(int num){
    std::vector<int> measurement(10,-110);

    digitalWrite(SCN, HIGH);
    std::vector<WiFiInfo> scan = WiFiScan(num);
    for(const auto& network : scan){
      ExtractMeasurement(network.MAC.c_str(), network.RSSI, measurement);
    }
    
    std::vector<BLEInfo> scanResults = BLEScan(num*2);
    for(const auto& ble : scanResults){
      ExtractMeasurement(ble.Address.c_str(), ble.RSSI, measurement);
    }
    
    MagData data = MagScan(num*5);
    ExtractMeasurement("00:00:00:00:00:00", data.avgMag, measurement);

    digitalWrite(SCN,LOW);
    return measurement;
  } 
//----------------------------------------

int IncreButton(int val, int max){
  if(digitalRead(UB) == LOW && !UBP){
    UBP = true;
    if(val >= max){
      val = 0;
    }else{
      ++val;
    }
    } else if(digitalRead(UB) == HIGH){
    UBP = false;
  }
  return val;
}

int DecreButton(int val, int max){
  if(digitalRead(DB) == LOW && !DBP){
    DBP = true;
    if(val == 0){
      val = max;
    } else{
      --val;
    }
    }else if(digitalRead(DB) == HIGH){
    DBP = false;
  }
  return val;
}

bool PhaseSelector(){
  bool selected = false;
  if(digitalRead(CB) == LOW && !CBP){
    CBP = true;
    selected = true;
  } else if(digitalRead(CB) == HIGH){
    CBP = false;
  }
  return selected;
} 

int BatStatus(){
  //Turn on transistor switch
  digitalWrite(EN, HIGH);
  delay(100);

  //Read battery voltage
  int batlvl = analogRead(READ);
  float volt = batlvl * (25 / 1024);
  int percentage = int(volt*100);

  digitalWrite(EN, LOW);

  return percentage;
}

// Function to navigate the grid based on button presses and return the updated row and column
std::pair<int, int> NavigateGrid(int row, int col) {
  int newRow = row;
  int newCol = col;
  
  if (digitalRead(UB) == LOW && newRow < maxRow) {
    newRow++;
    delay(200);  // Debounce delay
  }
  
  if (digitalRead(DB) == LOW && newRow >= 1) {
    newRow--;
    delay(200);  // Debounce delay
  }
  
  if (digitalRead(RB) == LOW && newCol < maxCol[row]) {
    newCol++;
    delay(200);  // Debounce delay
  }
  
  if (digitalRead(LB) == LOW && newCol >= 1) {
    newCol--;
    delay(200);  // Debounce delay
  }

  if (newCol > maxCol[row]){
    newCol = maxCol[row];
  }

  return std::make_pair(newRow, newCol);
}
//function to fetch correct RP for grid coordinate
int GetRP(int row, int col){
  int rp;
  rp = RPColValue[row] + col;
  return rp;
}

void InsertMeasurement(int RSSI[][10], int row, const std::vector<int>& measurement) {
    // Copy elements from the vector to the corresponding row of the array
    for (int col = 0; col < 10; ++col) {
        RSSI[row][col] = measurement[col];
    }
}

void ExtractMeasurement(const char* address, int val, std::vector<int>& measurement){
  for(int i = 0; i < 10; ++i){
    if (strcmp(fingerprint[i], address) == 0){
      if(val == 0){
      measurement[i] = -110;
      }else{
        measurement[i] = val;
      }
    }
  }
}

std::string measurementToString(const std::vector<int>& measurement) {
    std::stringstream ss;
    ss << "[";

    // Iterate over the elements of the measurement vector
    for (size_t i = 0; i < measurement.size(); ++i) {
        // Convert each element to a string and append it to the stringstream
        ss << measurement[i];
        
        // Add ':' delimiter unless it's the last element
        if (i < measurement.size() - 1) {
            ss << ":";
        }
    }

    ss << "]";
    
    // Return the stringstream content as a string
    return ss.str();
}

String MQTTString(int rp, const std::vector<int>& measurement) {
    std::stringstream ss;
    ss << "[";
    ss << rp;
    ss << ":";

    // Iterate over the elements of the measurement vector
    for (size_t i = 0; i < measurement.size(); ++i) {
        // Convert each element to a string and append it to the stringstream
        ss << measurement[i];
        
        // Add ':' delimiter unless it's the last element
        if (i < measurement.size() - 1) {
            ss << ":";
        }

        ss << "]";
    }
    
    // Return the stringstream content as a string
    return String(ss.str().c_str());
}

void printMatrix(int RSSI[][10], int numRows) {
  Serial.println("Matrix:");
  // Print column headers
  Serial.print("Row\t");
  for (int col = 0; col < 10; ++col) {
    Serial.print("Col");
    Serial.print(fingerprint[col]);
    Serial.print("\t");
  }
  Serial.println();
  
  // Print matrix data
  for (int i = 0; i < numRows; ++i) {
    Serial.print(i);
    Serial.print("\t"); // Print row number
    
    for (int j = 0; j < 10; ++j) {
      Serial.print(RSSI[i][j]);
      Serial.print("\t");
    }
    Serial.println();
  }
}
