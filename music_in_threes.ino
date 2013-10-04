// Music in Threes
// ( Arranged for the ATMega328 )
// Composition by Michael Winters, Programming by Michael Bond
// Based on Auduino code by Peter Knight, Tinker.it http://tinker.it
// http://code.google.com/p/tinkerit/wiki/Auduino

#include <avr/io.h>
#include <avr/interrupt.h>

uint16_t syncPhaseAcc, syncPhaseInc, grainPhaseAcc, grainAmp, grain2PhaseAcc, grain2Amp;
uint16_t syncPhaseAcc2, syncPhaseInc2, grainPhaseAcc2, grainAmp2, grain2PhaseAcc2, grain2Amp2;
uint16_t algo, next_threestable[12];
uint16_t note = -1;

// overall timbre settings, frequency and decay controls
uint16_t grainPhaseInc = 50;
uint8_t grainDecay = 10;
uint16_t grain2PhaseInc = 16;
uint8_t grain2Decay = 10;

// create seed note table
uint16_t threestable[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// PWM Output is on Pin 3
#define PWM_PIN       3
#define PWM_VALUE     OCR2B
#define LED_PIN       13
#define LED_PORT      PORTB
#define LED_BIT       5
#define PWM_INTERRUPT TIMER2_OVF_vect

// Stepped chromatic mapping to MIDI pitches: C, Db, D, Eb, E, F...
uint16_t midiTable[] = {
  259, // to offset, so that C = 1
  274,291,308,326,346,366,388,411,435,461,489,518,
  549,581,616,652,691,732,776,822,871,923,978,1036,
  1097,1163,1232,1305,1383,1465,1552,1644,1742,1845,1955,2071
};

// function to input MIDI position, output frequency
uint16_t mapMidi(uint16_t input) {
  return (midiTable[(1023-input) >> 3]);
}

void audioOn() {
  // Set up PWM to 31.25kHz, phase accurate
  TCCR2A = _BV(COM2B1) | _BV(WGM20);
  TCCR2B = _BV(CS20);
  TIMSK2 = _BV(TOIE2);
}

void setup() {
  pinMode(PWM_PIN,OUTPUT);
  // set initial sequence { 1, 4, 7, 10, 2, 5, 8, 11, 3, 6, 9, 12 }
  for (int i=0; i<12; i++) threestable[i] = (i * 3) % 12 + floor(i / 4) + 1;
  audioOn();
  pinMode(LED_PIN,OUTPUT);
}

void loop() {
  
  // set tempo
  delay(800);
  
  // blink LED
  LED_PORT ^= 1 << LED_BIT;
  
  // step to next note
  note++;
  
  // when the end of three measures is reached, replace old sequence with new sequence
  if (note == 12) for (int i=0; i<12; i++) threestable[i] = next_threestable[i];
  note = note % 12;
    
  // set piano pitch
  syncPhaseInc = midiTable[threestable[note]];

  // compute the next iteration of this note
  algo = ( ( note/4 + note%4 ) * 3 + ( note/4 ) ) % 12;
  
  // set the next sequence's pitch at this note
  next_threestable[note] = threestable[algo];
  
  // set violin pitch, once every measure
  if (note%4 == 0) syncPhaseInc2 = midiTable[threestable[note]+24];
  
  // Serial.println(threestable[note]);  
  
}

// Output PWM audio, frame by frame
SIGNAL(PWM_INTERRUPT) {
  
  uint8_t value;
  uint16_t output;

  syncPhaseAcc += syncPhaseInc;
  if (syncPhaseAcc < syncPhaseInc) {
    // Time to start the next grain
    grainPhaseAcc = 0;
    grainAmp = 0x7fff;
    grain2PhaseAcc = 0;
    grain2Amp = 0x7fff;
  }
  
  // Increment the phase of the grain oscillators
  grainPhaseAcc += grainPhaseInc;
  grain2PhaseAcc += grain2PhaseInc;

  // Convert phase into a triangle wave
  value = (grainPhaseAcc >> 7) & 0xff;
  if (grainPhaseAcc & 0x8000) value = ~value;
  // Multiply by current grain amplitude to get sample
  output = value * (grainAmp >> 8);

  // Repeat for second grain
  value = (grain2PhaseAcc >> 7) & 0xff;
  if (grain2PhaseAcc & 0x8000) value = ~value;
  output += value * (grain2Amp >> 8);

  // Make the grain amplitudes decay by a factor every sample (exponential decay)
  grainAmp -= (grainAmp >> 8) * grainDecay;
  grain2Amp -= (grain2Amp >> 8) * grain2Decay;

  // Scale output to the available range, clipping if necessary
  output >>= 9;
  
  // amplify
  output = output * 120;
  
  // Now, for the second tone..
  
  uint8_t value2;
  uint16_t output2;

  syncPhaseAcc2 += syncPhaseInc2;
  if (syncPhaseAcc2 < syncPhaseInc2) {
    // Time to start the next grain
    grainPhaseAcc2 = 0;
    grainAmp2 = 0x7fff;
    grain2PhaseAcc2 = 0;
    grain2Amp2 = 0x7fff;
  }
  
  // Increment the phase of the grain oscillators
  grainPhaseAcc2 += grainPhaseInc;
  grain2PhaseAcc2 += grain2PhaseInc;

  // Convert phase into a triangle wave
  value2 = (grainPhaseAcc2 >> 7) & 0xff;
  if (grainPhaseAcc2 & 0x8000) value2 = ~value2;
  // Multiply by current grain amplitude to get sample
  output2 = value2 * (grainAmp2 >> 8);

  // Repeat for second grain
  value = (grain2PhaseAcc2 >> 7) & 0xff;
  if (grain2PhaseAcc2 & 0x8000) value2 = ~value2;
  output2 += value2 * (grain2Amp2 >> 8);

  // Make the grain amplitudes decay by a factor every sample (exponential decay)
  grainAmp2 -= (grainAmp2 >> 8) * grainDecay;
  grain2Amp2 -= (grain2Amp2 >> 8) * grain2Decay;

  // Scale output to the available range, clipping if necessary
  output2 >>= 9;
  
  // amplify
  output2 = output2 * 30;

  // mix the two tones
  output = output + output2;
  
  // amplify
  //output = output*8;

  // clip if too loud
  if (output > 255) output = 255;
  

  // Output to PWM (this is faster than using analogWrite)  
  PWM_VALUE = output;
  
}
