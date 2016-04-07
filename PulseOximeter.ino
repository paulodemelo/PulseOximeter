#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); 
int pulseSpike = 0;
int pulseThreshold = 2.0; //to be determined by oscilliscope
int ledPin = 0;
int irPin = 1;
int switchPin = 2;
int count = 0;
float recordTime[200] = {0};
float BPM = 0;
unsigned long previousMillis = 0;        
const long interval = 1000;    
boolean above = false, below = false;       

void setup() {
	lcd.begin(16, 2);
	Serial.begin(9600);
}

void loop() {
	float startSwitch = analogRead(switchPin) * (5.0/1023.0);
	float pulseSignal = analogRead(ledPin) * (5.0/1023.0);

	// Initialization
	if (startSwitch < 3) {
		count = 0; // reset count
		recordTime[200] = {0}; // reset recorded time
		lcd.setCursor(0, 0);
		lcd.print("Insert finger & ");
		lcd.setCursor(0, 1);
		lcd.print("flip switch     ");  
	} 

	// Start recording
	else if ((startSwitch >= 3) && (count < 61)) {
		// timer for recording, stops at 60 seconds  
		while (count < 61) {   
			float startSwitch = analogRead(0) * (5.0/1023.0);
			timer(); // Start timer  
			recordData(); // Start recording data
			// Checking for pulse spikes
			if (pulseSignal > pulseThreshold) {
				above = true;
				float pulseSignal = analogRead(ledPin) * (5.0/1023.0);
				if (pulseSignal < pulseThreshold) {
					below = true;
				}
			}
			// Display
			lcd.setCursor(0, 0);
			lcd.print("Recording..     ");  
			lcd.setCursor(14, 0);
			lcd.print(count);
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
	if (x > 0) {
		for (int i = 1; i < x; i++) {
			sum += arr[i] - arr[i-1];
		}
	}
	avg = (sum / x);
	return 60/avg; 
}

//Function to check if pulse spike accured
void checkPulse(boolean a, boolean b) {
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
	}
}

// Function to record and store data file
void recordData() {
	String dataString = "";
	// Record Time,Voltage; new line
	for (int analogPin = 0; analogPin < 2; analogPin++) { 
		float sensor = analogRead(0) * (5.0/1023.0); //change back to analogPin *******
		dataString += (millis() / 1000.00);
		dataString += ",";
		dataString += String(sensor);
		dataString += ";"; 
		if (analogPin == 0) {
			dataString += "\n";
		}
	}
	Serial.println(dataString);

	File dataFile = SD.open("datalog.txt", FILE_WRITE);
	
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

// Simple timer function
void timer() {
	unsigned long currentMillis = millis(); 
	if (currentMillis - previousMillis >= interval) {
		previousMillis = currentMillis;
		count+=1;
	}
}