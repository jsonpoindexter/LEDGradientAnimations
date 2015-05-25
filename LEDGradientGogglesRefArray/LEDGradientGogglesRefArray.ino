#include <Average.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);


#include <FastLED.h>

#define WAIT_FOR_KEYBOARD 1

#define LED_PIN     2
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

int xMax = 16;
int yMax = 16;
int spread = 1;
int startX = 0;
int startY = 0;
int stopX = (yMax * spread) - 1;
int stopY = (yMax * spread) - 1;
int A = (stopX - startX);
int B = (stopY - startY);
int C1 = 0;
int C2 = 450;
int refArray[32] = { 285, 285, 285, 270, 240, 210, 180, 165, 165, 165, 165, 180, 210, 240, 270, 285, 150, 165, 165, 165, 165, 150, 120, 90, 60, 45, 45, 45, 45, 60, 90, 120 };
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
	if (WAIT_FOR_KEYBOARD) {
		Serial.begin(9600);
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
	Serial.print("C1 :"); Serial.println(C1);
	Serial.print("C1 :"); Serial.println(C2);
}

int currentHeading;
void loop()
{
	if (Serial.available() > 0) {
		char inChar = Serial.read();
		if (inChar == 'a'){
			currentHeading = currentHeading - 3;
		}
		if (inChar == 'd'){
			currentHeading = currentHeading + 3;
		}
		if (currentHeading > 360){
			currentHeading = 0;
		}
		if (currentHeading < 0){
			currentHeading = 360;
		}
		//        int currentHeading = readMag();
		//        if(currentHeading == 0){
		//          return;  
		//        }
		if (currentHeading != previousHeading){
			Serial.print("currentHeading: "); Serial.println(currentHeading);
			Serial.print("currentHeading - previousHeading: "); Serial.println(currentHeading - previousHeading);
			if ((currentHeading - previousHeading) >= 2){
				if ((currentHeading - previousHeading) >= 180){
					shiftX--;
					if (shiftX < startX){
						shiftX = (stopX * 4 * spread) - 1;
					}
					if (shiftX >(stopX * 4 * spread) - 1){
						shiftX = startX;
					}
					Serial.print("ShiftX :"); Serial.println(shiftX);
					ShowGradient();
					previousHeading = currentHeading;
				}
				else{
					shiftX++;
					if (shiftX < startX){
						shiftX = (stopX * 4 * spread) - 1;
					}
					if (shiftX >(stopX * 4 * spread) - 1){
						shiftX = startX;
					}
					Serial.print("ShiftX :"); Serial.println(shiftX);
					ShowGradient();
					previousHeading = currentHeading;
				}
			}
			if ((currentHeading - previousHeading) <= -2){
				if ((currentHeading - previousHeading) <= -180){
					shiftX++;
					if (shiftX < startX){
						shiftX = (stopX * 4 * spread) - 1;
					}
					if (shiftX >(stopX * 4 * spread) - 1){
						shiftX = startX;
					}
					Serial.print("ShiftX :"); Serial.println(shiftX);
					ShowGradient();
					previousHeading = currentHeading;
				}
				else{
					shiftX--;
					if (shiftX < startX){
						shiftX = (stopX * 4 * spread) - 1;
					}
					if (shiftX >(stopX * 4 * spread) - 1){
						shiftX = startX;
					}
					Serial.print("ShiftX :"); Serial.println(shiftX);
					ShowGradient();
					previousHeading = currentHeading;
				}
			}
		}
	}
}

void ShowGradient() {
	for (int i = 0; i < 32; i++){
		C = refArray[i] + (15 * shiftX);
		if (C > C2){
			C = C2 - (C - C2);
		}
		if (C < C1){
			C = C1 - (C - C1);
		}
		Serial.print(C); Serial.print(", ");
		color = map(C, C1, C2, start_color, end_color);
		leds[i] = ColorFromPalette(currentPalette, color, BRIGHTNESS, currentBlending);
	}
	Serial.println("");
	FastLED.show();
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
}
