

#include <Wire.h>



#include <MPU9250_asukiaaa.h>

#include <Adafruit_GFX.h>                                  // Core graphics library

#include <Adafruit_ST7735.h>                               // Hardware-specific library

#include <SPI.h>

#define TFT_CS     10
#define TFT_RST    8  // you can also connect this to the Arduino reset
// in which case, set this #define pin to 0!
#define TFT_DC     9




//#include "M4Stack.h"

//#include "Button.h"





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

#define zmax 2        //  2 fÃ¼r sehr schnelle Anzeige
// diesen Wert hochsetzen auf z.B.   10 ,  wenn die Anzeige viel ruhiger sein soll

#define komma 1        //   1 fÃ¼r eine Nachkommastelle
//    0    wenn Gradzahlen ganzzahlig sein sollen

#define diffmax 1.0    // 1.0 fÃ¼r eine ruhige Anzeige, 0.2 Abstufung.
// 0.5 Anzeige ist lebhafter, Abstufung 0.1  fÃ¼r hohe Genauigkeitsanzeige
// 2.0 fÃ¼r noch grÃ¶bere Abstufung, noch ruhiger.
MPU9250 mySensor;
uint8_t sensorId;
float gyro_x, gyro_y, gyro_z;
double gyro_x_cal, gyro_y_cal, gyro_z_cal;
long loop_timer;
int Serial_loop_counter;  // wird hier nicht benutzt
float angle_pitch, angle_roll, angle_yaw, ap, ar, ay;
boolean set_gyro_angles;
float angle_roll_acc, angle_pitch_acc;
float angle_pitch_output, angle_roll_output;

int z = 0;
char buffer[50]; float f;
long t1, t2, t3;

float diff;
byte erstesmal_1,erstesmal_2,erstesmal_3;  // damit beim Start sofort die MeÃŸ-Werte angezeigt werden
                                           // sonst erst bei Ã„nderungen

void setup() {
  Serial.begin(115200);
  Serial.println("started");

  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.setTextWrap(true);
  tft.setRotation(3);

  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(0xFFFF);
  tft.setCursor(0, 0);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(0);



#ifdef _ESP32_HAL_I2C_H_
  // for esp32
  Wire.begin(19, 18); //sda, scl
#else
  Wire.begin();
#endif
  mySensor.setWire(&Wire);
  mySensor.beginAccel();
  mySensor.beginMag();
  mySensor.beginGyro();
  sensorId = mySensor.readId();
  Serial.println("MPU 9250 Sensor-ID " + String (sensorId));
  tft.println("MPU 9250 Sensor-ID " + String (sensorId));
  //Print text to screen



  delay(1500);                                                         //Delay 1.5 second to display the text
  tft.fillScreen(ST7735_BLACK);
  //Clear the Serial

  tft.setTextSize(0);



  Serial.print("Calibrating gyro");
  tft.print("Calibrate");


  for (int cal_int = 0; cal_int < 2000 ; cal_int ++) {                 //Run this code 2000 times

    if (cal_int % 125 == 0) {
      Serial.print(".");  //Print a dot on the Serial every 125 readings
      tft.print(".");
    }

    //Read the raw acc and gyro data from the MPU-6050

    mySensor.gyroUpdate();
    gyro_x = mySensor.gyroX();
    gyro_y = mySensor.gyroY();
    gyro_z = mySensor.gyroZ();
    gyro_x_cal += gyro_x;                                              //Add the gyro x-axis offset to the gyro_x_cal variable

    gyro_y_cal += gyro_y;                                              //Add the gyro y-axis offset to the gyro_y_cal variable

    gyro_z_cal += gyro_z;                                              //Add the gyro z-axis offset to the gyro_z_cal variable

    delay(3);                                                          //Delay 3us to simulate the 250Hz program loop
    //die Schleife  ist nun aber langsamer
    // ist wohl egal
  }

  gyro_x_cal /= 2000;                                                  //Divide the gyro_x_cal variable by 2000 to get the avarage offset
  gyro_y_cal /= 2000;                                                  //Divide the gyro_y_cal variable by 2000 to get the avarage offset
  gyro_z_cal /= 2000;                                                  //Divide the gyro_z_cal variable by 2000 to get the avarage offset

  tft.setTextSize(0);                                                      //Clear the Serial
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(5, 10);
  tft.print("pitch ");
  tft.setCursor(5, 30);
  tft.print("roll  ");
  tft.setCursor(5, 50);
  tft.println("yaw   ");
  //Set the Serial cursor to position to position 0,0
  Serial.println(" Calibration Werte");
  Serial.print("Pitch:" + String(gyro_x_cal));                                               //Print text to screen

  //Set the Serial cursor to position to position 0,1

  Serial.print("   Roll :" + String(gyro_y_cal));                                               //Print text to screen
  Serial.print("  yaw :" + String(gyro_z_cal));
  // tft.fillScreen(ST7735_BLACK);
  // digitalWrite(13, LOW);
  loop_timer = micros();                                               //Reset the loop timer

  Serial.println("...........");

  erstesmal_1 = 1;
   erstesmal_2 = 1;
    erstesmal_3 = 1;

}      // setup



void loop() {

  t1 = micros();

  mySensor.gyroUpdate();
  gyro_x = mySensor.gyroX();
  gyro_y  = mySensor.gyroY();
  gyro_z = mySensor.gyroZ();                                            //Read the raw acc and gyro data from the MPU-6050

  gyro_x -= gyro_x_cal;                                                //Subtract the offset calibration value from the raw gyro_x value
  gyro_y -= gyro_y_cal;                                                //Subtract the offset calibration value from the raw gyro_y value
  gyro_z -= gyro_z_cal;                                                //Subtract the offset calibration value from the raw gyro_z value

  angle_pitch += gyro_x * 0.0080;                                   //Calculate the traveled pitch angle and add this to the angle_pitch variable
  angle_roll += gyro_y * 0.0080;                                    //Calculate the traveled roll angle and add this to the angle_roll variable
  angle_yaw += gyro_z * 0.0080;



  write_Serial();

  angle_pitch_output = angle_pitch;
  angle_roll_output = angle_roll;
  t2 = micros();

  
//  Serial.println(" t2-t1     " + String(t2 - t1)); //##########################################
  
  z = z + 1;

  //if (z==1)
  {

    if (abs(angle_pitch_output) > 360) angle_pitch_output = 0;
    if (abs(angle_roll_output) > 360) angle_roll_output = 0;
    if (abs(angle_yaw) > 360) angle_yaw = 0;

 //   Serial.print(" p " + String(angle_pitch_output));

  //  Serial.print(" r " + String(angle_roll_output));

  //  Serial.println(" y " + String(angle_yaw));

    if (z == 1) {
      diff = (round(ap * 10) - (round(angle_pitch_output * 10)));
      if (abs(diff )   > diffmax || erstesmal_1 == 1                ) {

        erstesmal_1 = 0;
        tft.setTextColor(0x0000, 0x0000);
        dtostrf(ap, 5, komma, buffer);
        tft.setCursor(65, 10);        tft.print(buffer);
        tft.setTextColor(0xFFFF, 0xFFFF);
        dtostrf(angle_pitch_output, 5, komma, buffer);
        tft.setCursor(65, 10);       tft.print(buffer); ap = angle_pitch_output;
      }
    }

    if (z == 2) {
      diff = round(ar * 10) - round(angle_roll_output * 10);

      if (abs(diff) > diffmax || erstesmal_2 == 1  ) {
        erstesmal_2 = 0;
        tft.setTextColor(0x0000, 0x0000);
        dtostrf(ar, 5, komma, buffer);
        tft.setCursor(65, 30);      tft.print(buffer);
        tft.setTextColor(0xFFFF, 0xFFFF);
        dtostrf(angle_roll_output, 5, komma, buffer);
        tft.setCursor(65, 30);    tft.print(buffer);      ar = angle_roll_output;
      }
    }

    if (z == 3) {
      diff = round(ay * 10) - round(angle_yaw * 10);
      if (abs(diff) > diffmax || erstesmal_3 == 1  ) {
        erstesmal_3 = 0;
        tft.setTextColor(0x0000, 0x0000);
        dtostrf(ay, 5, komma, buffer);
        tft.setCursor(65, 50);        tft.println(buffer);


        tft.setTextColor(0xFFFF, 0xFFFF);
        dtostrf(angle_yaw, 5, komma, buffer);
        tft.setCursor(65, 50);       tft.println(buffer); ay = angle_yaw;

      }
    }

 }
  if (z > zmax) {
    z = 0;
  }

  t3 = micros();
  if (t3-t1>2000){
  Serial.println(" t3-t1     " + String(t3 - t1));}

  while (micros() - loop_timer < 8000);                                //Wait until the loop_timer reaches 4000us (250Hz) before starting the next loop
  // 8000 wegen langsamer Anzeige
  loop_timer = micros();
}

void write_Serial() {                                                     //Subroutine for writing the Serial
}

void setup_mpu_6050_registers() {
  mySensor.setWire(&Wire);
  mySensor.beginAccel();
  mySensor.beginMag();
  mySensor.beginGyro();
  sensorId = mySensor.readId();                                     //End the transmission

}






