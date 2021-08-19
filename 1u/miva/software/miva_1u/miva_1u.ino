#include <SPI.h>
#include <EEPROM.h>
#include <OneButton.h>

/*************************************************************
  MCVGO - USB MIDI to CV Interface

  Based on the 12 Gate 16 CV USB MIDI Interface from little-scale
  https://little-scale.blogspot.com/2019/01/12-gate-16-cv-usb-midi-interface-bom.html

    
  Modified by maasijam 
 *************************************************************/

/**************************************************************  
   MODES:
   mode0 (000): CV Pitch (1-8), Gates (1-8)
   mode1 (100): CV Pitch/Vel (1-4/5-8), Gates (1-4), Clock 24ppqn (5), Clock 1/4 (6), Clock 1/2 (7), Gate Run/Stop (8)
   mode2 (110): CV Pitch/Mod (1-4/5-8), Gates (1-4), Clock 24ppqn (5), Clock 1/4 (6), Clock 1/2 (7), Gate Run/Stop (8)  
   mode3 (111): CV Mod (1-8), Clock 24ppqn (1), Clock 1/4 (2), Clock 1/2 (3), Clock 1/8 (4), Clock 1/16 (5), Clock 1/4 T (6), Trigger Start(7), Gate Run/Stop (8)
   mode4 (001): CV Pitch/Vel (1-2/5-6), CV Mod (3-4/7-8), Gates (1-2), Clock 24ppqn (3), Clock 1/4 (4), Clock 1/2 (5), Clock 1/8 (6), Trigger Start(7), Gate Run/Stop (8)
   mode5 (011): CV Mod (1-8), Gates (1-4), Clock 24ppqn (5), Clock 1/4 (6), Clock 1/2 (7), Gate Run/Stop (8)

   Long press button to save the selected mode as default mode
**************************************************************/

int mode;

// Setup a new OneButton 
int sw = 8;
OneButton btn(sw, true);

const int pitchbend_value_positive = 1200; 
const int pitchbend_value_negative = -1200;
const float offset_pitch = 36;


const int LED = 13;

int cs_pin =  18;
int pitchbend_value[8];
int pitch_values[8];
int cs9 = 2;
byte shadow_gate;

byte clock_tick;
byte clock_value;
byte play_flag;
byte play_tick;

float voltage_range = 4.024 * 1200;

int yellow_led = 5;
int yellow_led2 = 16;
int yellow_led3 = 17;


void setup() {
  SPI.begin();
  pinMode(cs9, OUTPUT);
  pinMode(sw, INPUT_PULLUP);

  btn.attachClick(clickBtn);
  btn.attachLongPressStart(longPressStartBtn);
  btn.attachLongPressStop(longPressStopBtn);
  btn.attachDuringLongPress(longPressDurationBtn);
  
  for (int i = 0; i < 4; i ++) {
    pinMode(cs_pin + i, OUTPUT);
    digitalWriteFast(cs_pin + i, HIGH);
    delay(50);
  }

  for (int i = 0; i < 8; i ++) {
    writeGate(i, HIGH);
    delay(50);
  }

  delay(100);
  
  for (int i = 0; i < 8; i ++) {
    writeGate(i, LOW);
    delay(50);
  }

  delay(100);

  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandlePitchChange(OnPitchChange);
  usbMIDI.setHandleControlChange(OnControlChange);
  usbMIDI.setHandleRealTimeSystem(OnClock); 
  
  mode = EEPROM.read(0);

   if(mode > 5) {
    mode = 0; 
    EEPROM.write(0, mode);
  }

  pinMode(yellow_led, OUTPUT);
  pinMode(yellow_led2, OUTPUT);
  pinMode(yellow_led3, OUTPUT);

}

void loop() {
  btn.tick();
  displayMode();
  while (usbMIDI.read()) {
    // read & ignore incoming messages
  }
}

void OnNoteOn(byte channel, byte pitch, byte velocity) {
  
  if (mode == 0) {
    if (channel < 9) {
        pitch_values[channel - 1] = pitch;
        writeDAC(cs_pin + ((channel - 1) / 2), (channel) & 1, constrain(map((pitch - offset_pitch) * 100.0 + pitchbend_value[channel - 1], 0.0, voltage_range, 0.0, 4095.0), 0.0, 4095.0));
        if (velocity > 0) {
          writeGate(channel - 1, HIGH);
        }
        else {
          writeGate(channel - 1, LOW);
        }
    }
  }

  else if (mode == 1 || mode == 2) {
    if (channel < 5) {

        pitch_values[channel - 1] = pitch;
        writeDAC(cs_pin + ((channel - 1) / 2), (channel) & 1, constrain(map((pitch - offset_pitch) * 100.0 + pitchbend_value[channel - 1], 0.0, voltage_range, 0.0, 4095.0), 0.0, 4095.0));
        if (mode == 1) {
          writeDAC((cs_pin + 2) + ((channel - 1) / 2), (channel) & 1, map(velocity, 0, 127, 0, 4095));
        }
        if (velocity > 0) {
          writeGate(channel - 1, HIGH);
        }
        else {
          writeGate(channel - 1, LOW);
        }
    }
  }

  else if (mode == 4) {
    if (channel < 3) {
       pitch_values[channel - 1] = pitch;
        writeDAC(cs_pin + ((channel - 1) / 2), (channel) & 1, constrain(map((pitch - offset_pitch) * 100.0 + pitchbend_value[channel - 1], 0.0, voltage_range, 0.0, 4095.0), 0.0, 4095.0));
        writeDAC((cs_pin + 2) - ((channel - 1) / 2), (channel) & 1, map(velocity, 0, 127, 0, 4095));
        
        if (velocity > 0) {
          writeGate(channel - 1, HIGH);
        }
        else {
          writeGate(channel - 1, LOW);
        }
    }
  }
  else if (mode == 5) {
    if (channel < 5) {
              
        if (velocity > 0) {
          writeGate(channel - 1, HIGH);
        }
        else {
          writeGate(channel - 1, LOW);
        }
    }
  }
  
}

void OnNoteOff(byte channel, byte pitch, byte velocity) {

  if (mode == 0) {
    if (channel < 9) {
      writeGate(channel - 1, LOW);
    }
  }

  else if (mode == 1 || mode == 2 || mode == 5) {
    if (channel < 5) {
      writeGate(channel - 1, LOW);
    }
  }
  else if (mode == 4) {
    if (channel < 3) {
      writeGate(channel - 1, LOW);
    }
  }

  
}

void OnPitchChange (byte channel, int pitch_change) {

  if (mode == 0) {
    if (channel < 9) {
      pitchbend_value[channel - 1] = map(pitch_change, 0, 16383, pitchbend_value_negative, pitchbend_value_positive);
      writeDAC(cs_pin + ((channel - 1) / 2), (channel) & 1, constrain(map((pitch_values[channel - 1] - offset_pitch) * 100.0 + pitchbend_value[channel - 1], 0.0, voltage_range, 0.0, 4095.0), 0.0, 4095.0));
    }
  }

  else if (mode == 1 || mode == 2) {
    if (channel < 5) {
      pitchbend_value[channel - 1] = map(pitch_change, 0, 16383, pitchbend_value_negative, pitchbend_value_positive);
      writeDAC(cs_pin + ((channel - 1) / 2), (channel) & 1, constrain(map((pitch_values[channel - 1] - offset_pitch) * 100.0 + pitchbend_value[channel - 1], 0.0, voltage_range, 0.0, 4095.0), 0.0, 4095.0));
    }
  }

  else if (mode == 4) {
    if (channel < 3) {
      pitchbend_value[channel - 1] = map(pitch_change, 0, 16383, pitchbend_value_negative, pitchbend_value_positive);
      writeDAC(cs_pin + ((channel - 1) / 2), (channel) & 1, constrain(map((pitch_values[channel - 1] - offset_pitch) * 100.0 + pitchbend_value[channel - 1], 0.0, voltage_range, 0.0, 4095.0), 0.0, 4095.0));
    }
  }
}

void OnControlChange (byte channel, byte control, byte value) {
  if (mode == 2) {
    if (channel < 5) {
      if (control == 1) {
        writeDAC((cs_pin + 2) - ((channel - 1) / 2), (channel) & 1, map(value, 0, 127, 0, 4095));
      }
    }
  }
  else if(mode == 3 || mode == 5) {
    if (channel < 9) {
      if (control == 1) {
        writeDAC(cs_pin + ((channel - 1) / 2), (channel) & 1, map(value, 0, 127, 0, 4095));
      }
    }
  }
  else if(mode == 4) {
    if (channel < 3 || channel > 6) {
      if (control == 1) {
        if(channel < 3) {
          writeDAC((cs_pin + 1) - ((channel - 1) / 2), (channel) & 1, map(value, 0, 127, 0, 4095));
        } else {
          writeDAC(cs_pin + ((channel - 1) / 2), (channel) & 1, map(value, 0, 127, 0, 4095));
        }
      }
    }
  }
}

void OnClock(byte clockbyte) {

  // clock (decimal 248, hex 0xF8)
  // start (decimal 250, hex 0xFA)
  // continue (decimal 251, hex 0xFB)
  // stop (decimal 252, hex 0xFC)
  
  
    if(clockbyte == 0xf8 && play_flag == 1) {
      if (mode == 1 || mode == 2) {
        clockThru(4);
        fireGates(5,clock_tick,24); // quarter note
        fireGates(6,clock_tick,48); // half note
        if(play_tick == 1) {
          writeGate(7, HIGH); // half note
        }
      }

      else if (mode == 3) {
        clockThru(0);
        fireGates(1,clock_tick,6); // 16th note
        fireGates(2,clock_tick,12); // 8th note
        fireGates(3,clock_tick,16); // quarter triplet note
        fireGates(4,clock_tick,24); // quarter note
        fireGates(5,clock_tick,48); // half note

        if(play_tick == 1 && clock_tick < 3) {
          writeGate(6, HIGH); // Trigger start
        }
        if(play_tick == 1) {
          writeGate(7, HIGH); // Run/Stop Gate
        }
      }
      else if (mode == 4) {
        clockThru(2);
        fireGates(3,clock_tick,12); // 8th note
        fireGates(4,clock_tick,24); // quarter note
        fireGates(5,clock_tick,48); // half note

        if(play_tick == 1 && clock_tick < 3) {
          writeGate(6, HIGH); // Trigger start
        }
        if(play_tick == 1) {
          writeGate(7, HIGH); // Run/Stop Gate
        }
      }
      else if (mode == 5) {
        clockThru(4);
        fireGates(5,clock_tick,24); // quarter note
        fireGates(6,clock_tick,48); // half note

        if(play_tick == 1) {
          writeGate(7, HIGH); // Run/Stop Gate
        }
      }
      clock_tick ++; 
  
      if(clock_tick == 96) {
        clock_tick = 0; 
      }
      if(clock_tick == 3 && play_tick == 1) {
        if (mode == 3 || mode == 4) {
           writeGate(6, LOW);  
        }
      }
      if(clock_tick == 6 && play_tick == 1) {
        play_tick = 0;
      }
    }
    
    if(clockbyte == 0xfa || clockbyte == 0xfb) { // start or continue bytes
      play_flag = 1; 
      play_tick = 1;
      clock_value = 0; 
      clock_tick = 0; 
    }
  
    if(clockbyte == 0xfc) { // stop byte
      if (mode == 1 || mode == 2 || mode == 5) {
        writeGate(4, LOW);
        writeGate(5, LOW);
        writeGate(6, LOW);
        writeGate(7, LOW); 
      }
      else if (mode == 3) {
        writeGate(0, LOW);
        writeGate(1, LOW);
        writeGate(2, LOW);
        writeGate(3, LOW);
        writeGate(4, LOW);
        writeGate(5, LOW);
        writeGate(6, LOW);
        writeGate(7, LOW);   
      }
      else if (mode == 4) {
         writeGate(2, LOW);
        writeGate(3, LOW);
        writeGate(4, LOW);
        writeGate(5, LOW);
        writeGate(6, LOW);
        writeGate(7, LOW); 
      }
      play_flag = 0; 
      play_tick = 0;
    }
}

void writeDAC (int cs, int dac, int val) {
  digitalWriteFast(cs, LOW);
  dac = dac & 1;
  val = val & 4095;
  SPI.transfer(dac << 7 | 0 << 5 | 1 << 4 | val >> 8);
  SPI.transfer(val & 255);
  digitalWriteFast(cs, HIGH);
}

void writeGate(byte bit_number, byte bit_value) {
  bitWrite(shadow_gate, bit_number, bit_value);
  digitalWriteFast(cs9, LOW);
  SPI.transfer(shadow_gate);
  digitalWriteFast(cs9, HIGH);
}


void clickBtn() {
  Serial.println("clickBtn");
  for (int i = 0; i < 8; i ++) {
    writeGate(i, LOW);
  }
  mode++;
  if(mode == 6){mode=0;}
}

void longPressStartBtn() {
  Serial.println("longPressStartBtn");
}

void longPressStopBtn() {
  Serial.println("longPressStopBtn");
  EEPROM.write(0, mode);
}

void longPressDurationBtn() {
  Serial.println("longPressDurationBtn");

  digitalWrite(yellow_led, HIGH);
  digitalWrite(yellow_led2, HIGH);
  digitalWrite(yellow_led3, HIGH);
  delay(100);
  digitalWrite(yellow_led, LOW);
  digitalWrite(yellow_led2, LOW);
  digitalWrite(yellow_led3, LOW);
  delay(100);
}

void displayMode() {
  if (mode == 0) {
    digitalWrite(yellow_led, LOW);
    digitalWrite(yellow_led2, LOW);
    digitalWrite(yellow_led3, LOW);
  } else if(mode == 1) {
    digitalWrite(yellow_led, HIGH);
    digitalWrite(yellow_led2, LOW);
    digitalWrite(yellow_led3, LOW);
  } else if(mode == 2) {
    digitalWrite(yellow_led, HIGH);
    digitalWrite(yellow_led2, HIGH);
    digitalWrite(yellow_led3, LOW);
  } else if(mode == 3) {
    digitalWrite(yellow_led, HIGH);
    digitalWrite(yellow_led2, HIGH);
    digitalWrite(yellow_led3, HIGH);
  } else if(mode == 4) {
    digitalWrite(yellow_led, LOW);
    digitalWrite(yellow_led2, LOW);
    digitalWrite(yellow_led3, HIGH);
  } else if(mode == 5) {
    digitalWrite(yellow_led, LOW);
    digitalWrite(yellow_led2, HIGH);
    digitalWrite(yellow_led3, HIGH);
  }  
}

void clockThru(byte gate_number) {
  writeGate(gate_number, HIGH);
  delay(5);
  writeGate(gate_number, LOW);
}
