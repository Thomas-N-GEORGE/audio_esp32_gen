#include <Arduino.h>

// Constants & variables.
const int GEN_OUT_PIN = 23; // Pin 23 for output ?
const int TRIGGER_PIN = 4;	// Interruption pin.

// PWM channel 0 parameters.
const int GEN_PWM_CHANNEL = 0;
const int PWM_FREQ = 200000;  // 100 KHz.
const int PWM_RESOLUTION = 8; // bits resolution for output.
// 0.315V ≈ -10dBV and  1.23V ≈ +4dBu.
const int MAX_AMPLITUDE = pow(2, PWM_RESOLUTION - 1); // PWM -> 1.6V max is way enough... But means we loose 1bit of resolution.

const int SAMPLE_RATE = 20000;						  // 20KHz.
const int SAMPLE_PERIOD_MICRO = int(1 / SAMPLE_RATE); // Sample period in microseconds.

unsigned long lastMicros = 0;
volatile boolean noteOn = false;

// Forward declarations.
void IRAM_ATTR noteEvent(); // Function called by interruption.
void gen(float freq);

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT); // Sets the builtin led as output (pin #2 on ESP32 Vroom).
	pinMode(TRIGGER_PIN, INPUT_PULLUP);
	attachInterrupt(TRIGGER_PIN, noteEvent, CHANGE);

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
	if (noteOn)
	{
		gen(440);
	}
}

void gen(float freq)
{
	Serial.println("Now in gen()...");
	digitalWrite(LED_BUILTIN, HIGH); // sets the digital pin on.
	lastMicros = micros();

	Serial.println("Now looping until a note event...");
	while (noteOn)
	{
		// 		y = max_amp * sin (wt + phi)
		// => 	y = max_amp * sin (2*PI * freq * t + phi)
		// =>	Don't forget the fact that we have a "DC bias" of MAX_AMPLITUDE / 2.
		ledcWrite(GEN_PWM_CHANNEL, MAX_AMPLITUDE / 2 * (1 + sin(2 * PI * freq * (micros() - lastMicros) / 1000000))); // sets the PWM voltage on out pin GEN_OUT.
		delayMicroseconds(SAMPLE_PERIOD_MICRO);																		  // wait length of a sample period.
	}
	Serial.println("Now noteEvent is set to false...");

	digitalWrite(LED_BUILTIN, LOW); // Sets the digital pin off.
	ledcWrite(GEN_PWM_CHANNEL, 0);	// Sets the PWM voltage on out pin GEN_OUT back to 0 for now. May be we could try to fade out ?
	Serial.println("Now leaving gen()...");
}

void IRAM_ATTR noteEvent()
{
	ets_printf("note triggered\n"); // Cannot use Serial.println() nor delay() in an interruption function.
	noteOn = !noteOn;
}