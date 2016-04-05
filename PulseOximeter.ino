#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // initialize the library with the numbers of the interface pins
int x = 0;
int y = 0;

void setup(){
	lcd.begin(16, 2);
}

void loop(){
	float V1 = analogRead(A0) * (5.0/1023.0);
	// Stop recording
	if (V1 > 3){
		x = 0;
		lcd.setCursor(0, 0);
		lcd.print("Insert finger & ");
		lcd.setCursor(0, 1);
		lcd.print("flip switch     ");  
	} 

	// Start recording
	else if ((V1 <= 3) && (x < 61)){
		// timer for recording, stops at 60 seconds  
		for (x=0; x < 61; x+=1){      
			delay(1000); 
			float V1 = analogRead(A0) * (5.0/1023.0);
			lcd.setCursor(0, 0);
			lcd.print("Recording..     ");  
			lcd.setCursor(14, 0);
			lcd.print(x);
			lcd.setCursor(0, 1);
			lcd.print("Pulse: ");
			// if switch is depressed, break out of recording
			if (V1 > 3){ 
				break;
			}    
		} 
	}

	// if done recording, stop
	else{   
		lcd.setCursor(0, 0);
		lcd.print("Done recording  "); 
		lcd.setCursor(0, 1);
		lcd.print("Pulse:          "); 
	}
}

