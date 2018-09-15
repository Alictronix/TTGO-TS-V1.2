#include <WiFi.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>                               // Hardware-specific library
#include <MPU9250_asukiaaa.h>

#include <NTPClient.h>
#include <WiFiUdp.h>
const char* ssid     = "TALKTALK0E6422";
const char* password = "UJECR83R";

#define NTP_OFFSET  3600 // In seconds 
#define NTP_INTERVAL 60 * 1000    // In miliseconds
#define NTP_ADDRESS  "europe.pool.ntp.org"  
#define TFT_CS     10
#define TFT_RST    8  // you can also connect this to the Arduino reset
// in which case, set this #define pin to 0!
#define TFT_DC     9
Adafruit_ST7735 tft = Adafruit_ST7735(16, 17, 23, 5, 9); // CS,A0,SDA,SCK,RESET  fÃ¼r ttgo
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);    // fÃ¼r UNO und 1.8 Display
float r, x1, ya, z1, x2, y2, z2, x3, y3, x, y, z3;               //

#ifdef _ESP32_HAL_I2C_H_

#define SDA_PIN 19

#define SCL_PIN 18

#endif

#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

#define zmax 2        
#define komma 1        
#define diffmax 1.0 

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);


void setup()
{
//display.begin();
Serial.begin(115200);

Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED)
{
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected.");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());

timeClient.begin();
tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.setTextWrap(true);
  tft.setRotation(3);

  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(0xFFFF);
  tft.setCursor(10, 20);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(2);

#ifdef _ESP32_HAL_I2C_H_
  // for esp32
  Wire.begin(19, 18); //sda, scl
#else
  Wire.begin();
#endif
}

void loop()
{
timeClient.update();
String formattedTime = timeClient.getFormattedTime();
tft.fillScreen(ST7735_BLACK);
tft.setCursor(20, 20);
tft.println(formattedTime);
delay(1000);

}
