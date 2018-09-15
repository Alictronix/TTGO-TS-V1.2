
#include <Wire.h>
#include <MPU9250_asukiaaa.h>
#include <SPI.h>




#include <Adafruit_GFX.h>                                  // Core graphics library
#include <Adafruit_ST7735.h>                               // Hardware-specific library
Adafruit_ST7735 tft = Adafruit_ST7735(16, 17, 23, 5, 9); // CS,A0,SDA,SCK,RESET
float r, x1, ya, z1, x2, y2, z2, x3, y3, z3;               //
int f[8][2], x, y;                                         // Draw box, x, y center
int c[8][3] = {                                            // Cube
 {-20,-20, 20},{20,-20, 20},{20,20, 20},{-20,20, 20},
 {-20,-20,-20},{20,-20,-20},{20,20,-20},{-20,20,-20} };
 
#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 19
#define SCL_PIN 18
#endif

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
//char buffer[50]; float f; //#############################################################
long t1, t2, t3;

float diff;
byte erstesmal_1,erstesmal_2,erstesmal_3;  // damit beim Start sofort die MeÃŸ-Werte angezeigt werden
                                           // sonst erst bei Ã„nderungen


int a;


void setup(void) {
  Serial.begin(115200);
  tft.initR(INITR_18BLACKTAB);                             // 1.44 v2.1
  tft.fillScreen(ST7735_BLACK);                            // CLEAR
 // tft.setTextColor(0x5FCC);                                // GREEN
  tft.setRotation(1);                                      // 
  x = tft.width() /2; x = x + 14;                          // x Center Calculate
  y = tft.height()/2;                                      // y Center Calculate

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

  Serial.println("Calibration Werte");
  Serial.print("Pitch:" + String(gyro_x_cal));                                               //Print text to screen

  //Set the Serial cursor to position to position 0,1

  Serial.print("   Roll :" + String(gyro_y_cal));                                               //Print text to screen
  Serial.print("  yaw :" + String(gyro_z_cal));


}
 
void loop(){

  
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



 

  angle_pitch_output = angle_pitch;
  angle_roll_output = angle_roll;
  t2 = micros();


   Serial.print(" p " + String(angle_pitch_output));

    Serial.print(" r " + String(angle_roll_output));

   Serial.println(" y " + String(angle_yaw));





 
 //for ( a = 0; a <= 1; a = a + 2 )
 {                   // 0 to 360 degree
 
     tft.drawLine(f[0][0],f[0][1],f[1][0],f[1][1],ST7735_BLACK);
   tft.drawLine(f[1][0],f[1][1],f[2][0],f[2][1],ST7735_BLACK);
   tft.drawLine(f[2][0],f[2][1],f[3][0],f[3][1],ST7735_BLACK);
   tft.drawLine(f[3][0],f[3][1],f[0][0],f[0][1],ST7735_BLACK);
   tft.drawLine(f[4][0],f[4][1],f[5][0],f[5][1],ST7735_BLACK);
   tft.drawLine(f[5][0],f[5][1],f[6][0],f[6][1],ST7735_BLACK);
   tft.drawLine(f[6][0],f[6][1],f[7][0],f[7][1],ST7735_BLACK);
   tft.drawLine(f[7][0],f[7][1],f[4][0],f[4][1],ST7735_BLACK);
   tft.drawLine(f[0][0],f[0][1],f[4][0],f[4][1],ST7735_BLACK);
   tft.drawLine(f[1][0],f[1][1],f[5][0],f[5][1],ST7735_BLACK);
   tft.drawLine(f[2][0],f[2][1],f[6][0],f[6][1],ST7735_BLACK);
   tft.drawLine(f[3][0],f[3][1],f[7][0],f[7][1],ST7735_BLACK);
 //  tft.drawLine(f[1][0],f[1][1],f[3][0],f[3][1],ST7735_BLACK);// cross
 //  tft.drawLine(f[0][0],f[0][1],f[2][0],f[2][1],ST7735_BLACK);// cross
   
   
   x=angle_roll_output*10+80;
   y=angle_pitch_output*10+50;
   a=angle_yaw*10;
   
   
   
   
   for (int i = 0; i < 8; i++) {                           //
    r  = a * 0.0174532;                                    // 1 degree
    x1 = c[i][2] * sin(r) + c[i][0] * cos(r);              // rotate Y
    ya = c[i][1];                                          //
    z1 = c[i][2] * cos(r) - c[i][0] * sin(r);              //
    x2 = x1;                                               //
    y2 = ya * cos(r) - z1 * sin(r);                        // rotate X
    z2 = ya * sin(r) + z1 * cos(r);                        //
    x3 = x2 * cos(r) - y2 * sin(r);                        // rotate Z
    y3 = x2 * sin(r) + y2 * cos(r);                        //
    z3 = z2; x3 = x3 + x ; y3 = y3 + y ;                   //
    f[i][0] = x3; f[i][1] = y3; f[i][2] = z3;              // store new values
   }
   
  // tft.fillScreen(ST7735_GREEN);
   tft.drawLine(f[0][0],f[0][1],f[1][0],f[1][1],ST7735_WHITE);
   tft.drawLine(f[1][0],f[1][1],f[2][0],f[2][1],ST7735_WHITE);
   tft.drawLine(f[2][0],f[2][1],f[3][0],f[3][1],ST7735_WHITE);
   tft.drawLine(f[3][0],f[3][1],f[0][0],f[0][1],ST7735_WHITE);
   tft.drawLine(f[4][0],f[4][1],f[5][0],f[5][1],ST7735_WHITE);
   tft.drawLine(f[5][0],f[5][1],f[6][0],f[6][1],ST7735_WHITE);
   tft.drawLine(f[6][0],f[6][1],f[7][0],f[7][1],ST7735_WHITE);
   tft.drawLine(f[7][0],f[7][1],f[4][0],f[4][1],ST7735_WHITE);
   tft.drawLine(f[0][0],f[0][1],f[4][0],f[4][1],ST7735_WHITE);
   tft.drawLine(f[1][0],f[1][1],f[5][0],f[5][1],ST7735_WHITE);
   tft.drawLine(f[2][0],f[2][1],f[6][0],f[6][1],ST7735_WHITE);
   tft.drawLine(f[3][0],f[3][1],f[7][0],f[7][1],ST7735_WHITE);
 //  tft.drawLine(f[1][0],f[1][1],f[3][0],f[3][1],ST7735_WHITE);// cross
 //  tft.drawLine(f[0][0],f[0][1],f[2][0],f[2][1],ST7735_WHITE);// cross
 }
}






