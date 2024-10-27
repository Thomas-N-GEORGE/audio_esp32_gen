# ESP32 Audio melody player

## A simple project to be compiled and run on an ESP32 device.

- Create note lists and play them by using a push button wired to pin #4.
- The (analog) audio outputs on pin #23.

### How it works

- Series of notes in `src/scales.h` are listed as melodies by the class Melody `src/melody.h`
- They are then played in a loop by an audio generator found in `src/main.cpp`, every next note playing when push button shorts pin #4 to ground.
- The internal blue led lights up when audio is running.

### Experiment...

- Define your own new melodies and notes in `src/scales.h` and instanciate them in `src/main.cpp` with `Melody melody = Melody(MY_MELODY);`.
- Try changing constants in `src/main.cpp`: 
  - `AUDIO_SAMPLE_RATE` if lowered can cause spectrum aliasing...
  - lower `PWM_RESOLUTION` to "bit crush" the sound...