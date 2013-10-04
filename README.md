arduino-musicinthrees
=====================

12-tone #1c / Music in Threes ( Arranged for the Arduino / ATMega328 )

Composition by Michael Winters / MW Ensemble - http://www.ionikrecords.com

Programming by Michael Bond - http://keepfrozen.com

PWM Synthesis based on Auduino by Peter Knight, Tinker.it - http://code.google.com/p/tinkerit/wiki/Auduino


=====================


An Arduino-based interpretation of "12-tone #1a (Sequences of length = 4, add 3 then 9, for piano)", the algorithmic composition from MW Ensemble's "6 Songs" EP (2013).

Developed with the composer's help, this piece iterates through a series of notes using a simple algorithm ("add 3 then 9"), and outputs audio on pin 3 using simple PWM (Pulse Width Modulation) synthesis. The synth code is a simplified (but polyphonic!) version of Peter King's Audiuno PWM, with a secondary "violin" holding the first note of each measure. The result is a looping series of cascading tritones.


http://vimeo.com/65076038

http://theionikrecordingscompanyllc.bandcamp.com/album/6-songs

