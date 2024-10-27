#include <Arduino.h>
#include "scales.h"
#include "melody.h"

const int GEN_OUT_PIN = 23; // Pin 23 for output ?
const int TRIGGER_PIN = 4;  // Interrupt pin.

// PWM channel 0 parameters.
const int GEN_PWM_CHANNEL = 0;
const int PWM_FREQ = 200000;  // 100 KHz.
const int PWM_RESOLUTION = 8; // bits resolution for PWM signal output.
// 0.315V ≈ -10dBV and  1.23V ≈ +4dBu.
const int MAX_AMPLITUDE = pow(2, PWM_RESOLUTION - 1); // PWM output -> 1.6V max is way enough... But means we loose 1bit of resolution ?

// Audio.
const int AUDIO_SAMPLE_RATE = 20000;                                    // 20KHz.
const int AUDIO_SAMPLE_PERIOD_MICRO = int(1000000 / AUDIO_SAMPLE_RATE); // Sample period in microseconds.

volatile boolean noteOn = false;
unsigned long lastMicros = 0;
unsigned long lastMillis = 0;
float freq = 440;

// Melody melody = Melody(C_MAJOR);
Melody melody = Melody(AU_CLAIR_DE_LA_LUNE);
// Melody melody = Melody(LA_MARSEILLAISE);

// Forward function declarations.
void IRAM_ATTR noteEvent(); // Function called by the interrupt.
void gen(float freq);

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT); // Sets the builtin led as output (pin #2 on ESP32 Vroom).
    pinMode(TRIGGER_PIN, INPUT_PULLUP);
    attachInterrupt(TRIGGER_PIN, noteEvent, CHANGE);
    lastMicros = micros();

    // Configure PWM channel 0.
    ledcSetup(GEN_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    // Attach generator pin to channel 0.
    ledcAttachPin(GEN_OUT_PIN, GEN_PWM_CHANNEL);

    freq = melody.rewind();

    Serial.begin(9600);
    delay(1000); // Delay to start serial monitoring.
    Serial.println("ESP32 Melody player");
}

void loop()
{
    if (noteOn)
    {
        gen(freq);
        freq = melody.getNextNote();
    }
}

void gen(float freq)
{
    digitalWrite(LED_BUILTIN, HIGH); // sets the digital led pin on.
    lastMicros = micros();

    while (noteOn)
    {
        /**
         * 		y = max_amp * sin (wt + phi)
         * 	=>	y = max_amp * sin (2*PI * freq * t + phi)
         *  =>	Don't forget the fact that we have a "DC bias" set to MAX_AMPLITUDE / 2.
         */
        ledcWrite(GEN_PWM_CHANNEL, MAX_AMPLITUDE / 2 * (1 + sin(2 * PI * freq * (micros() - lastMicros) / 1000000))); // sets the PWM voltage on out pin GEN_OUT.
        delayMicroseconds(AUDIO_SAMPLE_PERIOD_MICRO);                                                                 // wait length of a sample period.
    }

    digitalWrite(LED_BUILTIN, LOW); // Sets the digital led pin off.
    ledcWrite(GEN_PWM_CHANNEL, 0);  // Sets the PWM voltage on out pin GEN_OUT back to 0 for now. May be we could try to fade out ?
}

void IRAM_ATTR noteEvent()
{
    // ets_printf("note triggered\n"); // Cannot use Serial.println() nor delay() in an interrupt function.
    // We need to pad interrupts due to "dirty" on/off electric contact.
    // So we reduce the interrupts time intervall senitivity.
    if (millis() - lastMillis > 10)
    {
        noteOn = !noteOn;
        lastMillis = millis();
    }
}
