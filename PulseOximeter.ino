#include <LiquidCrystal.h>
#include <SPI.h>
#include <SD.h>

const int chipSelect = 10;

LiquidCrystal lcd(A3, 9, 5, 4, 3, 2); 
int pulseSpike = 0;
int pulseThreshold = 1.0; //to be determined by oscilliscope
int ledPin = 0;
int irPin = 1;
int switchPin = 2;
int count = 0;
float recordTime[100] = {0}; 
int BPM = 0;
unsigned long previousMillis = 0;
const long interval = 1000;    
boolean above = false, below = false;       

void setup() {
    pinMode(A3, OUTPUT);
    digitalWrite(A3, HIGH);
	Serial.begin(9600);
    lcd.begin(16, 2);
}

void loop() {
	float startSwitch = analogRead(switchPin) * (5.0/1023.0);
	float pulseSignal = analogRead(ledPin) * (5.0/1023.0);

	// Initialization
	if (startSwitch < 3) {
		count = 0; // reset count
		recordTime[200] = {0}; // reset recorded time
		lcd.setCursor(0, 0);
		lcd.print("Place finger &  ");
		lcd.setCursor(0, 1);
		lcd.print("flip switch     ");  
	} 

	// Start recording
	else if ((startSwitch >= 3) && (count < 61)) {
		// timer for recording, stops at 60 seconds  
	    while (count < 61) {   
			float startSwitch = analogRead(0) * (5.0/1023.0);
			recordData(); // Start recording data
			// Checking for pulse spikes
            if (pulseSignal < pulseThreshold) {
                float pulseSignal = analogRead(A0) * (5.0/1023.0);
                above = true;
                if (pulseSignal > pulseThreshold) {
                    below = true;
                }
            }
            checkPulse(above, below);
			// Stops recording
			if (startSwitch < 3) { 
				break;
			}    
		} 
	}

	// If done recording, stop
	else {   
		lcd.setCursor(0, 0);
		lcd.print("Done recording. "); 
		lcd.setCursor(0, 1);
		lcd.print("                "); 
	}
}

// Function to calculate pulse rate
float calcPulse(int x, float arr[]) {
    float avg = 1;
    float sum = 0;
    float last = 0;
    if (x > 4) {
        for (int i = x - 5; i < x; i++) { // Summing last 5 values
            sum += arr[i] - arr[i-1];
        }
    }
    last = arr[x] - arr[x-1]; // just displays last pulse
    avg=sum/5;
    return 60/avg; 
}

// Function to check if pulse spike accured
int checkPulse(boolean a, boolean b) {
    if (a && b){
        above = false;
        below = false;  
        recordTime[pulseSpike] = float(millis() / 1000.00);
        BPM = calcPulse(pulseSpike, recordTime);       
        pulseSpike++;
        lcd.setCursor(0, 1);
        lcd.print("Pulse:          ");
        lcd.setCursor(7, 1);
        lcd.print(BPM);
        if (pulseSpike == 99) { // Just in case array gets filled
            float recordTime[100] = {0};
            pulseSpike = 0;
        }
    }
}

// Function to record and store data file
void recordData() {
    // Calibration test.. wait for at least 5 spikes to start recording
    if (pulseSpike > 4) { 
    	String dataString = "";
    	// Record [LEDTime,LEDVoltage,IRTime,IRVoltage] 
    	for (int analogPin = 0; analogPin < 2; analogPin++) { 
    		float sensor = analogRead(analogPin) * (5.0/1023.0); 
    		dataString += (millis() / 1000.00);
    		dataString += ",";
    		dataString += String(sensor);
    		if (analogPin == 0) {
    			dataString += ",";
    		}
    	}
    	Serial.println(dataString);
        timer();
        lcd.setCursor(0, 0);
        lcd.print("Recording...    ");  
        lcd.setCursor(14, 0);
        lcd.print(count);
        
        File dataFile = SD.open("datalog.csv", FILE_WRITE);
        if (dataFile) {
            dataFile.println(dataString);
            dataFile.close();
        }

        else {
            lcd.setCursor(0, 0);
            lcd.print("ERROR Could not "); 
            lcd.setCursor(0, 1);
            lcd.print("open file       "); 
        }
        
    }
    else
    {
        float sensor = analogRead(0) * (5.0/1023.0);
        Serial.println(sensor);
        lcd.setCursor(0, 0);
        lcd.print("Calibrating...  ");   
    }
}

// Simple timer function
void timer() {
	unsigned long currentMillis = millis(); 
	if (currentMillis - previousMillis >= interval) {
		previousMillis = currentMillis;
		count+=1;
	}
}
