#include <Arduino.h>

// Constants & variables.
const int GEN_OUT_PIN = 23; // pin 23 for output ?

// PWM channel 0 parameters.
const int GEN_PWM_CHANNEL = 0;
const int PWM_FREQ = 20000;	  // 20 KHz
const int PWM_RESOLUTION = 8; // Résolution de 8 bits.
// 0.315V ≈ -10dBV and  1.23V ≈ +4dBu.
const int MAX_AMPLITUDE = 127; // PWM -> 1.6V max is way enough... But goes down to 7bit resolution.

const int maxCounter = 2000;
const int touchThreshold = 30;
const unsigned long TIME_0 = millis();

int readValue = 66;
int counter = 0;

// Forward declarations.
void gen(int freq);

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT); // sets the builtin led as output (pin #2 on ESP32 Vroom).

	// Configure PWM channel 0.
	ledcSetup(GEN_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
	// Attach generator pin to channel 0.
	ledcAttachPin(GEN_OUT_PIN, GEN_PWM_CHANNEL);

	Serial.begin(9600);
	delay(1000); // Delai pour lancer le moniteur série.
	Serial.println("ESP32 Touch Demo");
}

void loop()
{
	readValue = touchRead(4);
	Serial.println(readValue);
	if (readValue < touchThreshold)
	{
		gen(1);
	}
}

void gen(int freq)
{
	// Serial.println("Touching the wire...");
	Serial.println("Now in gen()...");
	digitalWrite(LED_BUILTIN, HIGH);		   // sets the digital pin on.
	ledcWrite(GEN_PWM_CHANNEL, MAX_AMPLITUDE); // sets the PWM voltage on out pin GEN_OUT to max.
	Serial.println("Now waiting 5000...");
	delay(5000);
	Serial.println("Now passed waiting 5000...");
	digitalWrite(LED_BUILTIN, LOW); // sets the digital pin off.
	ledcWrite(GEN_PWM_CHANNEL, 0);	// sets the PWM voltage on out pin GEN_OUT to 0.
	Serial.println("Now leaving gen()...");
}