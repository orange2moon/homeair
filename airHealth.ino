
#include <DHT.h>
// #include <DHT_U.h>
#define DHTPIN 2 
#define DHTTYPE DHT22 

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
DHT dht(DHTPIN, DHTTYPE)

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#include <SDS011.h>
#include "GREAT.h"
#include "MEDIUM.h"
#include "BAD.H"
#include "VERY_BAD.h"
#include "VERY_VERY_BAD.h"
#include "airHealth.h"

//#define DEBUG_TRUE

static SDS011 my_sds;
const uint8_t my_rx = 13;
const uint8_t my_tx = 12;
const int xpos = (display.width() - 32);
const int ypos = 0;
    
void reset_display(int dsize= 2){
    display.clearDisplay();
    display.setTextSize(dsize);
    display.setTextColor(WHITE);
    display.setCursor(0,2);
}


/* this is the chart holding the breakpoint
 *  values and corresponding index values
 */
static chart aqiv;

static int initChartPM10(){
    /*Set the values for the PM 10 breakpoints
      and the corresponding index level */
    aqiv.good.bpL = 0.0;
    aqiv.good.bpH = 54.0;
    aqiv.good.iL = 0.0;
    aqiv.good.iH = 50.0;
    
    aqiv.moderate.bpL = 55.0;
    aqiv.moderate.bpH = 154.0;
    aqiv.moderate.iL = 51.0;
    aqiv.moderate.iH = 100.0;
    
    aqiv.unhealthyGroups.bpL = 155.0;
    aqiv.unhealthyGroups.bpH = 254.0;
    aqiv.unhealthyGroups.iL = 101.0;
    aqiv.unhealthyGroups.iH = 150.0;
    
    aqiv.unhealthy.bpL = 255.0;
    aqiv.unhealthy.bpH = 354.0;
    aqiv.unhealthy.iL = 151.0;
    aqiv.unhealthy.iH = 200.0;
    
    aqiv.veryUnhealthy.bpL = 355.0;
    aqiv.veryUnhealthy.bpH = 424.0;
    aqiv.veryUnhealthy.iL = 201.0;
    aqiv.veryUnhealthy.iH = 300.0;
    
    aqiv.hazardous.bpL = 425.0;
    aqiv.hazardous.bpH = 504.0;
    aqiv.hazardous.iL = 301.0;
    aqiv.hazardous.iH = 400.0;
    
    aqiv.veryHazardous.bpL = 505.0;
    aqiv.veryHazardous.bpH = 604.0;
    aqiv.veryHazardous.iL = 401.0;
    aqiv.veryHazardous.iH = 500.0;
    
    aqiv.error.bpL = 1.0;
    aqiv.error.bpH = -2.0;
    aqiv.error.iL = 1.0;
    aqiv.error.iH = -2.0;
    
    return 0;
}

static int initChartPM25(){
    /*Set the values for the PM 25 breakpoints
    and the corresponding index level */
    
    aqiv.good.bpL = 0.0;
    aqiv.good.bpH = 12.0;
    aqiv.good.iL = 0.0;
    aqiv.good.iH = 50.0;

    aqiv.moderate.bpL = 12.1;
    aqiv.moderate.bpH = 35.4;
    aqiv.moderate.iL = 51.0;
    aqiv.moderate.iH = 100.0;

    aqiv.unhealthyGroups.bpL = 35.5;
    aqiv.unhealthyGroups.bpH = 55.4;
    aqiv.unhealthyGroups.iL = 101.0;
    aqiv.unhealthyGroups.iH = 150.0;

    aqiv.unhealthy.bpL = 55.5;
    aqiv.unhealthy.bpH = 150.4;
    aqiv.unhealthy.iL = 151.0;
    aqiv.unhealthy.iH = 200.0;

    aqiv.veryUnhealthy.bpL = 150.5;
    aqiv.veryUnhealthy.bpH = 250.4;
    aqiv.veryUnhealthy.iL = 201.0;
    aqiv.veryUnhealthy.iH = 300.0;

    aqiv.hazardous.bpL = 250.5;
    aqiv.hazardous.bpH = 350.4;
    aqiv.hazardous.iL = 301.0;
    aqiv.hazardous.iH = 400.0;

    aqiv.veryHazardous.bpL = 350.5;
    aqiv.veryHazardous.bpH = 500.4;
    aqiv.veryHazardous.iL = 401.0;
    aqiv.veryHazardous.iH = 500.0;

    aqiv.error.bpL = 1.0;
    aqiv.error.bpH = -2.0;
    aqiv.error.iL = 1.0;
    aqiv.error.iH = -2.0;
        
    return 0;
}

int getIndex(float pm25){
  /* Calculate the Air Quality Index Value for 
   *  a given concentration (milligrams per cubic microgram)
   *  Be sure to to set the variable aqiv to the correct
   *  break points and corresponding index values before
   *  running this function.
   */
  aqi chart;
  float taint, result = 0.0;

  taint = int( pm25 * 10.0) / 10.0;

  if( taint >= aqiv.good.bpL && taint <= aqiv.good.bpH ){
    chart = aqiv.good;

  } else if ( taint >= aqiv.moderate.bpL && taint <= aqiv.moderate.bpH ){
    chart = aqiv.moderate;

  } else if ( taint >= aqiv.unhealthyGroups.bpL && taint <= aqiv.unhealthyGroups.bpH ){
    chart = aqiv.unhealthyGroups;

  } else if ( taint >= aqiv.unhealthy.bpL && taint <= aqiv.unhealthy.bpH ){
    chart = aqiv.unhealthy;

  } else if ( taint >= aqiv.veryUnhealthy.bpL && taint <= aqiv.veryUnhealthy.bpH ){
    chart = aqiv.veryUnhealthy;

  } else if ( taint >= aqiv.hazardous.bpL && taint <= aqiv.hazardous.bpH ){
    chart = aqiv.hazardous;

  } else if ( taint >= aqiv.veryHazardous.bpL && taint <= aqiv.veryHazardous.bpH ){
    chart = aqiv.veryHazardous;

  } else {
    chart = aqiv.error;
    result = -1.0;
  }

  if(result == -1.0 && taint > aqiv.veryHazardous.bpH){
    result = 999.0;
  } else if( result == -1.0 && taint < aqiv.good.bpL){
    result = 0.0;
  } else {
    result = (chart.iH - chart.iL)/(chart.bpH - chart.bpL)*(taint-chart.bpL) + chart.iL;
  }
  
  return int(result + 0.5);
  
}

/* Collect the pm25 and pm 10 concentrations
 * then calculate the aqi
 * then display the aqui with a note saying
 * good-medium-bad-bad!!-bad!!!!
 * finally print commma seperated data to serial
 * output
 */
int display_data(){
    float p10, p25;
    int pm10AQI, pm25AQI, myAQI;
    int error = 0;
    int square = 32;

    error = my_sds.read(&p25, &p10); 
    reset_display(4);
    
    if (error == 0){
      
        initChartPM25();
        pm25AQI = getIndex(p25);
        
        initChartPM10();
        pm10AQI = getIndex(p10);

        if( pm10AQI > pm25AQI ){
          myAQI = pm10AQI;
          display.println(pm10AQI);
        }else{
          myAQI = pm25AQI;
          display.println(pm25AQI);
        }
     
        if( myAQI < 50 ){
          display.drawBitmap(xpos, ypos, GREAT, square, square, WHITE);
        }else if (myAQI < 100){
          display.drawBitmap(xpos, ypos, MEDIUM, square, square, WHITE);  
        }else if (myAQI < 150){
          display.drawBitmap(xpos, ypos, BAD, square, square, WHITE);        
        }else if (myAQI < 200){
          display.drawBitmap(xpos, ypos, VERY_BAD, square, square, WHITE);         
        }else{
          display.drawBitmap(xpos, ypos, VERY_VERY_BAD, square, square, WHITE);
        }
   }else{
        display.print(F("No Data"));
   }

    display.display();

    Serial.print(p25);
    Serial.print(F(","));
    Serial.print(pm25AQI);
    Serial.print(F(","));
    Serial.print(p10);
    Serial.print(F(","));
    Serial.print(pm10AQI);
    Serial.print(F(","));
    Serial.println(myAQI);

    return error;
}

void setup() {
  
  my_sds.begin(my_rx, my_tx);
  my_sds.wakeup();
  Serial.begin(9600);
  // comma seperated value data
  Serial.print(F("PM25,PM25AQI,PM10,PM10AQI,MYAQI"));
  // initialize with the I2C addr 0x3C (for the 128x32)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  
  for( int i = 30; i > 0; i--){
    reset_display();
    display.println(F("Data in"));
    display.print(i);
    display.println(F(" seconds"));
    display.display();
    delay(1000); 
  }

}

void loop() {
    display_data();
    my_sds.sleep();
    // Serial.println(F("sleeping 4 minutes"));
    delay(240000);
    // Serial.println(F("waking up"));
    my_sds.wakeup();
    delay(60000);
}


