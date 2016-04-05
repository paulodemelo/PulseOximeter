#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); 
int pulseSpike = 0;
int pulseThreshold = 3.0; //to be determined by oscilliscope
float recordTime[150] = {0};
float BPM = 0;
int counter = 0;
boolean above = false, below = false;

void setup(){
	lcd.begin(16, 2);
}

void loop(){
	float pushButton = analogRead(A0) * (5.0/1023.0);
	float pulseSignal = analogRead(A0) * (5.0/1023.0);

	// Stop recording
	if (pushButton < 3){
		counter = 0;
		lcd.setCursor(0, 0);
		lcd.print("Insert finger & ");
		lcd.setCursor(0, 1);
		lcd.print("flip switch     ");  
	} 

	// Start recording
	else if ((pushButton >= 3) && (counter < 61)){
		// timer for recording, stops at 60 seconds  
		for (int counter = 0; counter < 61; counter++){      
			delay(1000); 
			float pushButton = analogRead(A0) * (5.0/1023.0);
			lcd.setCursor(0, 0);
			lcd.print("Recording..     ");  
			lcd.setCursor(14, 0);
			lcd.print(counter);
			lcd.setCursor(0, 1);
			lcd.print("Pulse: ");
			// if switch is depressed, break out of recording
			if (pushButton < 3){ 
				break;
			}    
		} 
	}

	// if done recording, stop
	else{   
		lcd.setCursor(0, 0);
		lcd.print("Done recording. "); 
		lcd.setCursor(0, 1);
		lcd.print("                "); 
	}

	// Checking for pulse spikes
	if (pulseSignal > pulseThreshold){
		float pulseSignal = analogRead(A0) * (5.0/1023.0);
		above = true;
		if (pulseSignal < pulseThreshold){
			below = true;
		}
	}

	// Calculate pulse rate
	if (above && below){
		above = false;
		below = false;
		recordTime[pulseSpike] = float(millis() / 1000.00);
		BPM = calcPulse(pulseSpike, recordTime);       
		pulseSpike++;
	}
}

// Function to calculate pulse rate
float calcPulse(int x, float arr[]){
	float avg = 0;
	float sum = 0;
	for (int i = 1; i < x; i++) {
		sum += arr[i] - arr[i-1];
	}
	avg = (sum / x);
	return avg*60; 
}

