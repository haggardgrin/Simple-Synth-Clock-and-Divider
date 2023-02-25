/* output pins : line number = pin number
16x whole note
4x whole note
whole note
half note
quarter note
eighth note
sixteenth note
thirty second note
half note triplets
quarter note triplets
eighth note triplets
sixteenth note triplets
*/
const int OutPins[12] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

//input pins
const int BPMPin = A0;
const int ResetPin = A1;                                            //low = don't reset, high = reset
const int InhPin = A2;                                              //Low = run, high = stop

int i = 0;
int BPM = 120;
unsigned long MSPerBeat = 0;
unsigned long CurrentTime = 0;
unsigned long PrevTime[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 
unsigned long DelayTime[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 
bool ClockState[12] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};
bool PrevClockState[12] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
int Inh = 0;
int PrevInh = 1023;

void setup() {
  for (i = 0; i < 12; i++) {
    pinMode(OutPins[i], OUTPUT); 
  }
}

void loop() {
  CurrentTime = millis();                                           //check the time
  InhCheck();                                                       //check inhibit pin
  if (Inh <= 200 && PrevInh <= 200) {                               //if inhibit pin is low and has been low
    ClockRateCheck();                                               //be a clock divider
    ResetCheck();
    ClockOut();
  }
}

void InhCheck() {
  Inh = analogRead(InhPin);                                         //read inhibit pin
  if (Inh <= 200 && PrevInh > 200) {                                //if inhibit pin just went low
    for (i = 0; i < 12; i++) {                                      //for each channel
      ClockState[i] = HIGH;                                         //internal state is now high
      PrevClockState[i] = HIGH;                                     //think internal state has been high so you don't mess up later
      digitalWrite(OutPins[i], HIGH);                               //set channel high
      PrevTime[i] = CurrentTime;                                    //restart delay timer
    }
  }
  PrevInh = Inh;                                                    //remember inhibit state for future
}

void ClockRateCheck() { 
  BPM = map(analogRead(BPMPin), 0, 1023, 40, 240);                  //read bpm pin and change millivolts to bpm
  MSPerBeat = 30000 / BPM;                                         //change bpm to milliseconds per quarter note
  DelayTime[0] = MSPerBeat * 64 ;                                   //calculate delay time for each note length
  DelayTime[1] = MSPerBeat * 16 ;
  DelayTime[2] = MSPerBeat * 4 ;
  DelayTime[3] = MSPerBeat * 2 ;
  DelayTime[4] = MSPerBeat ;
  DelayTime[5] = MSPerBeat / 2 ;
  DelayTime[6] = MSPerBeat / 4 ;
  DelayTime[7] = MSPerBeat / 8 ;
  DelayTime[8] = MSPerBeat * 4 / 3 ;
  DelayTime[9] = MSPerBeat * 2 / 3 ;
  DelayTime[10] = MSPerBeat / 3 ;
  DelayTime[11] = MSPerBeat / 6 ;
}

void ResetCheck() { 
  if (analogRead(ResetPin) >= 200) {                                //if reset pin is high
    for (i = 0; i < 12; i++) {                                      //for each channel
      PrevTime[i] = CurrentTime;                                    //restart delay timer
      ClockState[i] = HIGH;                                         //internal state is now high
      PrevClockState[i] = HIGH;                                     //think internal state has been high so you don't mess up later
      digitalWrite(OutPins[i], HIGH);                               //turn on that channel
    }
  }
}

void ClockOut() {
  for (i = 0; i < 12; i++) {                                        //for each channel
    if (CurrentTime - PrevTime[i] >= DelayTime[i]) {                //if enough time has passed
      PrevTime[i] = CurrentTime;                                    //restart delay timer
      PrevClockState[i] = ClockState[i];                            //remember internal state for future
      ClockState[i] = !ClockState[i];                               //flip current internal state
    }
    if (ClockState[i] == HIGH && PrevClockState[i] == LOW) {        //if internal state just went high
      digitalWrite(OutPins[i], HIGH);                               //turn on that channel
    } 
    if (ClockState[i] == LOW && PrevClockState[i] == HIGH) {        //if internal state just went low
      digitalWrite(OutPins[i], LOW);                                //turn off that channel
    }
  }
}


