#include <Average.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);


#include <FastLED.h>

#define WAIT_FOR_KEYBOARD 0

#define LED_PIN     2
#define NUM_LEDS    60
#define BRIGHTNESS  255
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

const int xMax = 8;
const int yMax = 16;
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

//int ledMatrix[xMax][yMax] = {
//	{  0,  1,  2,  3,  4 },
//	{ 21, 20, 19, 18, 17 },
//	{ 22, 23, 24, 25, 26 },
//	{ 43, 42, 41, 40, 39 },
//	{ 44, 45, 46, 47, 48 }
//};

//full matrix I have
// int ledMatrix[xMax][yMax] = {
//	{	0, 	21, 22, 43, 44 },
//	{	1, 	20, 23, 42, 45 },
//	{	2, 	19, 24, 41, 46 },
//	{	3, 	18, 25, 40, 47 },
//	{	4, 	17, 26, 39, 48 },
//	{	5, 	16, 27, 38, 49 },
//	{	6, 	15, 28, 37, 50 },
//	{	7, 	14, 29, 36, 51 },
//	{	8, 	13, 30, 35, 52 },
//	{	9, 	12, 31, 34, 53 },
//	{  10,	11,	32,	33,	54 }
//};

//Possible LED Goggles
int ledMatrix[xMax][yMax] = {
  {  0,  0,  0,  26,  25,  0,  0,  0,  0,  0,  0,  8,  7,  0,  0,  0 },
  {  0,  0,  27,  0,  0,  24,  0,  0,  0,  0,  9,  0,  0,  6,  0,  0 },
  {  0,  28,  0,  0,  0,  0,  23,  0,  0, 10,  0,  0,  0,  0,  5,  0 },
  {  29,  0,  0,  0,  0,  0,  0,  22,  11, 0,  0,  0,  0,  0,  0,  4 },
  {  30,  0,  0,  0,  0,  0,  0,  21,  12, 0,  0,  0,  0,  0,  0,  3 },
  {  0,  31,  0,  0,  0,  0,  20,  0,  0,  13, 0,  0,  0,  0,  2,  0 },
  {  0,  0,  32,  0,  0,  19,  0,  0,  0,  0,  14,  0,  0, 1,  0,  0 },
  {  0,  0,   0,  17, 18,  0,  0,  0,  0,  0,  0,  15, 16,  0,  0,  0  }
};
//uint8_t brightness = 32;

int spread = 1;
int startX = 0;
int startY = (5 * spread) - 1;
int stopX = (5 * spread) -1 ;
int stopY = 0;
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


void setup()
{

	Serial.begin(9600);
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
        Serial.print("C1 :");Serial.println(C1);
        Serial.print("C1 :");Serial.println(C2);
}


void loop()
{
        int currentHeading = readMag();
//        if(currentHeading == previousHeading){
//           return;
//        }
//        if(currentHeading == 0){
//          return;
//        }
//        ave.push(currentHeading);
//        
//        int currentAveHeading = ave.mean();
//        
//        if((currentAveHeading - previousAveHeading) >= 2 ){
//             previousAveHeading = currentAveHeading;   
//             shiftX++;
//             if( shiftX > (stopX * 4 * spread) - 1 ){
//                shiftX = startX; 
//             } 
//             Serial.print("ShiftX :");Serial.println(shiftX);
//             ShowGradient();
//        }
//        if( (currentAveHeading - previousAveHeading) <= -2 ){
//            previousAveHeading = currentAveHeading; 
//            shiftX--;
//            if( shiftX < startX ){
//                shiftX = (stopX * 4 * spread) - 1 ; 
//             }
//             Serial.print("ShiftX :");Serial.println(shiftX);
//             ShowGradient();
//        }
        ShowGradient();
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
                                if(ledMatrix[x - shiftX][y - shiftY] == 0){
                                  
                                }
				else{
                                    C = A * x +  B * y;
    				  if (C > C2){
    					C = C2 - (C - C2);
    				  }
    				  if (C < C1){
    					C = C1 - (C - C1);
    				  }
    		                  if (C < 10){
    					Serial.print("  "); Serial.print(C); Serial.print(" ");
    				  }
    				  else if (C < 100 && C > 10){
    					Serial.print(" "); Serial.print(C); Serial.print(" ");
    				  }
    				  else{
    					Serial.print(C); Serial.print(" ");
    				  }
    //				color = (start_color * (C2 - C) + end_color * (C - C1)) / (C2 - C1);
                                    color = map(C,C1,C2,start_color,end_color);
    //				if (color < 10){
    //
    //					Serial.print("  "); Serial.print(color); Serial.print(" ");
    //				}
    //				else if (color < 100 && color > 10){
    //					Serial.print(" "); Serial.print(color); Serial.print(" ");
    //				}
    //				else{
    //					Serial.print(color); Serial.print(" ");
    //				}
    				  leds[ (ledMatrix[x - shiftX][y - shiftY] - 1) ] = ColorFromPalette(currentPalette, color, BRIGHTNESS, currentBlending);
                            }
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
//	Serial.print("Compass Heading: ");Serial.println(intHeading);
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
