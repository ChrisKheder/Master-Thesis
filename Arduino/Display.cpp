#include "Display.h"

#define ILI9341_BLACK       0x0000  ///<   0,   0,   0
#define ILI9341_NAVY        0x000F  ///<   0,   0, 123
#define ILI9341_DARKGREEN   0x03E0  ///<   0, 125,   0
#define ILI9341_DARKCYAN    0x03EF  ///<   0, 125, 123
#define ILI9341_MAROON      0x7800  ///< 123,   0,   0
#define ILI9341_PURPLE      0x780F  ///< 123,   0, 123
#define ILI9341_OLIVE       0x7BE0  ///< 123, 125,   0
#define ILI9341_LIGHTGREY   0xC618  ///< 198, 195, 198
#define ILI9341_DARKGREY    0x7BEF  ///< 123, 125, 123
#define ILI9341_BLUE        0x001F  ///<   0,   0, 255
#define ILI9341_GREEN       0x07E0  ///<   0, 255,   0
#define ILI9341_CYAN        0x07FF  ///<   0, 255, 255
#define ILI9341_RED         0xF800  ///< 255,   0,   0
#define ILI9341_MAGENTA     0xF81F  ///< 255,   0, 255
#define ILI9341_YELLOW      0xFFE0  ///< 255, 255,   0
#define ILI9341_WHITE       0xFFFF  ///< 255, 255, 255
#define ILI9341_ORANGE      0xFD20  ///< 255, 165,   0
#define ILI9341_GREENYELLOW 0xAFE5  ///< 173, 255,  41
#define ILI9341_PINK        0xFC18  ///< 255, 130, 198

//Screen SPI Setup
#define TFT_CS   12
#define TFT_DC   11
#define TFT_MOSI 14
#define TFT_CLK 13
#define TFT_RST 10

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

//RSSI Array
const int numRP = 250;
const int RSSI_COLS = 10;
int RSSI[numRP][10];
std::vector<int> measurement;
std::string measurementStr;
String message;

int State = 1; // 1=first screen, 2 = Main screen, 3 = Offline screen, 4 = Online screen, Scan Screen
bool phase = false; //false = offline, true = online 

int phasePicker = 0;

//Grid & RP orientation 
int rp = 0;
int Maxrp = 250;

int row = 0;
int oldRow = 0;
int col = 0;
int oldCol = 0;
int oldRP = 0;
std::pair<int, int> pos;

//---------Button pins--------------------
  const int CB = 7;
  const int UB = 16;
  const int DB = 6;
  const int LB = 5;
  const int RB = 15;
//----------------------------------------

//---------Button boolen------------------
  bool CBP = true;
  bool UBP = true;
  bool DBP = true;
  bool LBP = true;
  bool RBP = true;
//----------------------------------------

void DisplaySetup(){
  Serial.print("Display Ready");
  tft.begin(); 
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);  tft.setTextSize(1);
}

void CenterClear(){
  tft.fillRect(10, 21, tft.width()- 10, tft.height() - 21, ILI9341_BLACK);
}

void SmallClear(int X, int Y){
  tft.fillRect(X, Y, 45, 10, ILI9341_BLACK);
}

void RP(){
  tft.fillRect(18,4, 30, 10, ILI9341_BLACK);
  tft.setCursor(18,5);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);  tft.setTextSize(1);
  tft.print(rp);
}

void Phase(){
  tft.fillRect((tft.width()/2 + 5),4, 45, 10, ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);  tft.setTextSize(1);
  tft.setCursor((tft.width()/2 + 5),5);
  if(!phase){
    tft.print("Offline");
  }else{
    tft.print("Online");
  }
}

void Battery(int lvl){
  tft.fillRect((tft.width() - 34),5, 30, 10, ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);  tft.setTextSize(1);
  tft.setCursor((tft.width() - 33),5);
  tft.print(lvl);
  tft.print("%");
}

//---------Case 2 Screens -----------------
  void PhasePointer(){
  if(phasePicker == 0){
    tft.fillRoundRect((tft.width()/2-53), 166, 100, 20, 10, ILI9341_WHITE);
    tft.setCursor((tft.width()/2-45), 170);
    tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
    tft.println("OFFLINE");
    tft.fillRoundRect((tft.width()/2-53), 146, 100, 20, 10, ILI9341_BLACK);
    tft.setCursor((tft.width()/2-38), 150);
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
    tft.println("ONLINE");
    tft.fillRoundRect((tft.width()/2-59), 126, 120, 20, 10, ILI9341_BLACK);
    tft.setCursor((tft.width()/2-50), 130);
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
    tft.println("RADIO MAP");
  } else if(phasePicker == 1){
    tft.fillRoundRect((tft.width()/2-53), 166, 100, 20, 10, ILI9341_BLACK);
    tft.setCursor((tft.width()/2-45), 170);
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
    tft.println("OFFLINE");
    tft.fillRoundRect((tft.width()/2-53), 146, 100, 20, 10, ILI9341_WHITE);
    tft.setCursor((tft.width()/2-38), 150);
    tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
    tft.println("ONLINE");
    tft.fillRoundRect((tft.width()/2-59), 126, 120, 20, 10, ILI9341_BLACK);
    tft.setCursor((tft.width()/2-50), 130);
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
    tft.println("RADIO MAP");
  } else if(phasePicker == 2){
    tft.fillRoundRect((tft.width()/2-53), 166, 100, 20, 10, ILI9341_BLACK);
    tft.setCursor((tft.width()/2-45), 170);
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
    tft.println("OFFLINE");
    tft.fillRoundRect((tft.width()/2-53), 146, 100, 20, 10, ILI9341_BLACK);
    tft.setCursor((tft.width()/2-38), 150);
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
    tft.println("ONLINE");
    tft.fillRoundRect((tft.width()/2-59), 126, 120, 20, 10, ILI9341_WHITE);
    tft.setCursor((tft.width()/2-50), 130);
    tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
    tft.println("RADIO MAP");
  }
  }

  void SelectedPhase(int num){
  if(num == 0){
    phase = false;
    CenterClear();
    Phase();
    tft.setCursor((tft.width()/2-115), 120);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);  tft.setTextSize(3);
    tft.println("Offline Phase");
    delay(1000);
    State = 3;
    
  }else if(num == 1){
    phase = true;
    CenterClear();
    Phase();
    tft.setCursor((tft.width()/2-115), 120);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);  tft.setTextSize(3);
    tft.println("Online Phase");
    delay(1000);
    State = 4;
    
  }else{
    CenterClear();
    tft.setCursor((tft.width()/2-80), 120);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);  tft.setTextSize(3);
    tft.println("Radio Map");
    delay(1000);
    State = 61;
  }
  }
//----------------------------------------

void RPPointer(){
  tft.fillRect((tft.width()/2-90), 108,210,30, ILI9341_BLACK); 
  tft.setCursor((tft.width()/2-80), 110);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
  tft.print(rp);
  tft.print(" (");
  tft.print(col);
  tft.print(",");
  tft.print(row);
  tft.print(")");
}

//---------Main Screens -----------------
  void StartScreen(){

  //Top bar with General information
  tft.fillScreen(ILI9341_BLACK);
  tft.drawFastHLine(0, 20-1, tft.width(), ILI9341_BLUE);
  tft.setCursor(0,5);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("RP:");
  tft.setCursor((tft.width()/2 - 30), 5);
  tft.print("PHASE:");
  tft.setCursor(tft.width() - 55, 5);
  tft.print("BAT:");

  //Initial RP,Phase and Battery status
  tft.setCursor(18,5);
  tft.print(" --");
  tft.setCursor((tft.width()/2 + 5),5);
  tft.print(" --");
  tft.setCursor((tft.width() - 33),5);
  tft.print(" --");

  //Start Screen 
  tft.setCursor(10, 110);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);  tft.setTextSize(2);
  tft.println("Indoor Localization Unit");
  tft.setCursor(95, 140);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);  tft.setTextSize(1);
  tft.println("By Christian Khederchah");

  }

  void MainScreen(){
  CenterClear();
  tft.setCursor((tft.width()/2-115), 80);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);  tft.setTextSize(3);
  tft.println("CHOOSE PHASE");
  PhasePointer();
  }

  void OfflineScreen(){
    RP();
    CenterClear();
    tft.setCursor((tft.width()/2-100), 80);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);  tft.setTextSize(3);
    tft.println("Selected RP: ");
    tft.setCursor((tft.width()/2-70), 110);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);  tft.setTextSize(3);
    tft.print(rp);
    tft.print(" (");
    tft.print(col);
    tft.print(",");
    tft.print(row);
    tft.print(")");
    tft.setCursor((tft.width()/2-80), 200);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);  tft.setTextSize(1);
    tft.print("Press center button to scan");
  }
 
 void ScanScreen(){
  CenterClear();
  tft.setCursor((tft.width()/2-105), 100);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);  tft.setTextSize(3);
  tft.println("Scanning...");
 }

 void ArrayScreen(int rp){
  RP();
  CenterClear();
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);

  // Define initial position to start printing
  int xPos = 20;
  int yPos = 40;

  // Iterate through the array elements starting from index rp
  for (int i = rp; i < min(rp + 10, numRP); ++i) {
    for (int j = 0; j < 10; ++j) {
      // Print the value at current position
      tft.setCursor(xPos, yPos);
      tft.print(RSSI[i][j]);
      xPos += 30; 
    }
    // Move to the next row
    xPos = 20;
    yPos += 20;
  }
 }
//----------------------------------------

void DeviceInit(){
  int oldvalue;
  std::pair<int, int> newPos;
  switch(State){

  case 1:
    StartScreen();
    delay(2000);
    State = 2;
  break;

  case 2:
    MainScreen();
    State = 21;
  break;

  case 21:
    oldvalue = phasePicker;
    phasePicker = IncreButton(phasePicker, 2);
    phasePicker = DecreButton(phasePicker, 2);
    if(oldvalue != phasePicker){
      State = 22;
    }
    if(PhaseSelector()){
      Battery(BatStatus());
      SelectedPhase(phasePicker);
    }
  break;

  case 22:
    PhasePointer();
    State = 21;
  break;

  case 3:
    OfflineScreen();
    State = 31;
  break;

  case 31:
    oldRP = GetRP(row,col);
    pos = NavigateGrid(row, col);
    row = pos.first;
    col = pos.second;
    rp = GetRP(row,col);
    if(oldRP != rp){  
      State = 32;
    }
    if(PhaseSelector()){
      State = 5;
    }
  break;

  case 32:
    RP();
    RPPointer();
    State = 31;
  break;

  case 33:
    OfflineScreen();
    tft.setCursor((tft.width()/2-90), 150);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);  tft.setTextSize(2);
    tft.print("Scan results:");
    tft.setCursor((tft.width()/2-125), 180);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);  tft.setTextSize(1);
    tft.print("RP:");
    tft.print(rp);
    tft.print(" ");
    tft.print(measurementStr.c_str());
    State = 31;
  break;

  case 4:
  break;

  case 5:
    ScanScreen();
    if(!phase){
      State = 51;
    }else{
      State = 4;
    }
    
  break;

  case 51:
    measurement = ScanButton(35);
    //InsertMeasurement(RSSI, rp, measurement);
    delay(10);
    MQTTSetup();
    measurementStr = measurementToString(measurement);
    message = MQTTString(rp, measurement);
    Pub(message);
    State = 33;
  break;

  case 6:
    oldvalue = rp;
    rp = IncreButton((rp+9), numRP);
    rp = DecreButton((rp-9), numRP);
    if(oldvalue != rp){
      State = 61;
    }
  break;

  case 61:
    ArrayScreen(rp);
    printMatrix(RSSI, numRP);
    //printMatrix(Test, numRP);
    State = 6;
  break;


  }
  
}