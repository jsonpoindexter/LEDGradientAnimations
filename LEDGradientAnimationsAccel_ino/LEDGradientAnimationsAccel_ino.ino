#include <Average.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);


#include <FastLED.h>

#define WAIT_FOR_KEYBOARD 0

#define LED_PIN     3
#define NUM_LEDS    60
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

uint8_t brightness = 255;

void setup()
{

//	Serial.begin(9600);
	if (WAIT_FOR_KEYBOARD) {

		// Wait for serial to initalize.
		while (!Serial) {}

		Serial.println("Strike any key to start...");
		// Wait for the next keystroke.
		while (!Serial.available()) {}

		// Clear the serial buffer.
		Serial.read();
	}

        Serial.println("Magnetometer Test"); Serial.println("");

	/* Initialise the sensor */
	if (!mag.begin())
	{
		/* there was a problem detecting the lsm303 ... check your connections */
		Serial.println("ooops, no lsm303 detected ... check your wiring!");
		while (1);
	}
	/* Display some basic information on this sensor */
	displaySensorDetails();

	FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
	FastLED.setBrightness(BRIGHTNESS);

	currentPalette = RainbowColors_p;
	currentBlending = BLEND;

	colorOff();

}

const int xMax = 5;
const int yMax = 5;
const int xMin = 0;
const int yMin = 0;

//test matrix for LED rings
//int ledMatrix[xMax][yMax] = {
//	{  0,  0,  2,  0,  0 },
//	{  0, 20,  0, 18,  0 },
//	{ 22,  0,  0,  0, 26 },
//	{  0, 42,  0, 40,  0 },
//	{  0,  0, 46,  0,  0 }
//};

int ledMatrix[xMax][yMax] = {
	{  0,  1,  2,  3,  4 },
	{ 21, 20, 19, 18, 17 },
	{ 22, 23, 24, 25, 26 },
	{ 43, 42, 41, 40, 39 },
	{ 44, 45, 46, 47, 48 }
};

//full matrix I have
//int ledMatrix[xMax][yMax] = {
//	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
//	{ 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11 },
//	{ 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 },
//	{ 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33 },
//	{ 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54 }
//};

int startX = xMin;
int startY = yMin;
int stopX = xMax - 1;
int stopY = yMax - 1;
int A = (stopX - startX);
int B = (stopY - startY);
int C1 = A * startX + B * startY;
int C2 = A * stopX + B * stopY;

int C = 0;

int start_color = 0;
int end_color = 255;
int color = 0;

long totalTime = 0;
int previousAveHeading = 0;
int previousHeading = 0;
int shiftX = 0;
int shiftY = 0;

Average<float> ave(10);

int spread = 3;

void loop()
{
        int currentHeading = readMag();
//        if(currentHeading == previousHeading){
//           return;
//        }
        if(currentHeading == 0){
          return;
        }
        ave.push(currentHeading);
        
        int currentAveHeading = ave.mean();
        
        if((currentAveHeading - previousAveHeading) >= 2 ){
          Serial.println(currentAveHeading - previousAveHeading);
             previousAveHeading = currentAveHeading;   
             shiftX++;
             ShowGradient();
             if( shiftX > (xMax - 1) * (xMax - 1)){
                shiftX = 0; 
             }
        }
        if( (currentAveHeading - previousAveHeading) <= -2 ){
          Serial.println(currentAveHeading - previousAveHeading);
            previousAveHeading = currentAveHeading; 
            shiftX--;
            ShowGradient();
            if( shiftX > xMin){
                shiftX = 0; 
             }
        }
        //moves LED rainbow (angled gradient) Left 1 cycle
//	GradientLeft();
	//moves LED rainbow (angled gradient) Right 1 cycle
//	GradientRight();
//	FastLED.delay(1000 / UPDATES_PER_SECOND);
}


//
//void GradientRight() {
////	for (int shiftX = (xMax - 1) * (xMax - 1); shiftX > xMin; shiftX--){
////		Serial.print(shiftX); Serial.print("\n");
//		for (int x = xMin + shiftX; x < xMax + shiftX; x++){
//			for (int y = yMin + shiftY; y < yMax + shiftY; y++){
//				C = A * x + B * y;
//				if (C > C2){
//					C = C2 - (C - C2);
//				}
//				if (C < C1){
//					C = C1 - (C - C1);
//				}
//				color = (start_color * (C2 - C) + end_color * (C - C1)) / (C2 - C1);
//				if (color < 10){
//					Serial.print("  "); Serial.print(color); Serial.print(" ");
//				}
//				else if (color < 100 && color > 10){
//					Serial.print(" "); Serial.print(color); Serial.print(" ");
//				}
//				else{
//					Serial.print(color); Serial.print(" ");
//				}
//				int led = ledMatrix[x - shiftX][y - shiftY];
//				leds[led] = ColorFromPalette(currentPalette, color, brightness, currentBlending);
//				
//			}
//			Serial.print("\n");
//		}
//		FastLED.show();
////	}
//	Serial.print("\n");
//}


void ShowGradient() {
//	for (int shiftX = 0; shiftX < (xMax - 1) * (xMax - 1); shiftX++){
//		Serial.print(shiftX); Serial.print("\n");
		for (int x = xMin + shiftX; x < xMax + shiftX; x++){
			for (int y = yMin + shiftY; y < yMax + shiftY; y++){
				C = A * x +  B * y;
				if (C > C2){
					C = C2 - (C - C2);
				}
				if (C < C1){
					C = C1 - (C - C1);
				}
				color = (start_color * (C2 - C) + end_color * (C - C1)) / (C2 - C1);
				if (color < 10){

					Serial.print("  "); Serial.print(color); Serial.print(" ");
				}
				else if (color < 100 && color > 10){
					Serial.print(" "); Serial.print(color); Serial.print(" ");
				}
				else{
					Serial.print(color); Serial.print(" ");
				}
				leds[ledMatrix[x - shiftX][y - shiftY]] = ColorFromPalette(currentPalette, color, brightness, currentBlending);
			}
			Serial.print("\n");
		}
		FastLED.show();
//	}
	Serial.print("\n");
}

void colorOff() {
	for (int i = 0; i < NUM_LEDS; i++) {
		leds[i] = CRGB::Black;
	}
	FastLED.show();
}

float readMag(){
	int intHeading = 0;
	/* Get a new sensor event */
	sensors_event_t event;
	mag.getEvent(&event);

	float Pi = 3.14159;

	//   Calculate the angle of the vector y,x
	float heading = (atan2(event.magnetic.y, event.magnetic.x) * 180) / Pi;

	//	Normalize to 0 - 360
	if (heading < 0){
		heading = 360 + heading;
	}
	intHeading = (int)heading; // round and convert to int
	Serial.print("Compass Heading: ");
	Serial.println(intHeading);
	return intHeading;
}

void displaySensorDetails(void)
{
	sensor_t sensor;
	mag.getSensor(&sensor);
	Serial.println("------------------------------------");
	Serial.print("Sensor:       "); Serial.println(sensor.name);
	Serial.print("Driver Ver:   "); Serial.println(sensor.version);
	Serial.print("Unique ID:    "); Serial.println(sensor.sensor_id);
	Serial.print("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" uT");
	Serial.print("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" uT");
	Serial.print("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" uT");
	Serial.println("------------------------------------");
	Serial.println("");
	delay(500);
}
