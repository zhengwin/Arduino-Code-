// column and row pins
const int col1 = 12;
const int col2 = 13;
const int rowPin[] = {4, 5, 6, 7, 8, 9, 10, 11};

// Song
bool playSong = true;
const int songArray[12][7][2] = 
{
  { {1, 0}, {0, 1}, {0, 1}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
  { {1, 0}, {1, 0}, {0, 1}, {0, 1}, {0, 0}, {0, 0}, {0, 0} },
  { {0, 1} ,{1, 0}, {1, 0}, {0, 1}, {0, 1}, {0, 0}, {0, 0} },
  { {1, 0} ,{0, 1} ,{1, 0}, {1, 0}, {0, 1}, {0, 1}, {0, 0} },
  { {0, 1} ,{1, 0} ,{0, 1} ,{1, 0}, {1, 0}, {0, 1}, {0, 1} },
  { {0, 0} ,{0, 1} ,{1, 0} ,{0, 1} ,{1, 0}, {1, 0}, {0, 1} },
  { {0, 0} ,{0, 0} ,{0, 1} ,{1, 0} ,{0, 1} ,{1, 0}, {1, 0} },
  { {0, 0} ,{0, 0} ,{0, 0} ,{0, 1} ,{1, 0} ,{0, 1} ,{1, 0} },
  { {0, 0} ,{0, 0} ,{0, 0} ,{0, 0} ,{0, 1} ,{1, 0} ,{0, 1} },
  { {0, 0} ,{0, 0} ,{0, 0} ,{0, 0} ,{0, 0} ,{0, 1} ,{1, 0} },
  { {0, 0} ,{0, 0} ,{0, 0} ,{0, 0} ,{0, 0} ,{0, 0} ,{0, 1} },
  { {0, 0} ,{0, 0} ,{0, 0} ,{0, 0} ,{0, 0} ,{0, 0} ,{0, 0} },
};

//millis
volatile double ovf_count = 0;
double ovf_count_sec = 0;

// Interrupts
const int interruptPin = 2;
const int pausePin = 3;
volatile boolean pause = false;
volatile boolean redButtonOn = false;
volatile boolean greenButtonOn = false;
volatile boolean changeNote = false;

//Timers
volatile double redStartTime = 0;
volatile double redLength = 0;
volatile boolean readyToTimeRed = false;
volatile boolean startGreen = false;
volatile boolean startRed = false;
volatile double greenStartTime = 0;
volatile double greenLength = 0;
volatile boolean readyToTimeGreen = false;
volatile boolean lastPressedGreen = false;
volatile boolean lastPressedRed = false;
volatile boolean readyToTimePause = false;
volatile boolean startPause = false;
volatile double pauseStartTime = 0;
volatile double pauseLength = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  cli(); // disable interrupts

  // Configure pins
  pinMode(interruptPin, INPUT);
  pinMode(pausePin, INPUT);
  pinMode(col1, OUTPUT);
  pinMode(col2, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

  //Interrupts
  attachInterrupt(digitalPinToInterrupt(interruptPin), buttonPress, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pausePin), pauseButton, RISING);

  // Timer register configurations
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B = 4;
  TIMSK1 = 6;
  OCR1A = 55000; // value for timer interrupt to overflow, controls beat of the song

  // initialize all leds to be off
  digitalWrite(col1, LOW);
  digitalWrite(col2, LOW);
  for (int i = 0; i < 8; i++) {
    pinMode(rowPin[i], OUTPUT);
    digitalWrite(rowPin[i], HIGH); 
  }
  
  sei(); // enable interrupts
  
//  TURN ALL LIGHTS ON
//  digitalWrite(col1, HIGH);
//  digitalWrite(col2, HIGH);
//  for (int i = 0; i < 8; i++) {
//    pinMode(rowPin[i], OUTPUT);
//    digitalWrite(rowPin[i], LOW);  
//  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (playSong) {
    for(int i = 0; i < 12 ; i++){ // number of lines of song array
      while(!changeNote || pause) {
        redLedFeedback();
        timeRed();
        recordRedStartTime();
        greenLedFeedback();
        timeGreen();
        recordGreenStartTime();
        timePause();
        recordPauseStartTime();
        for(int r = 0; r < 7; r++) {
          int rowNumber = r + 4;
          if(songArray[i][r][0] == 1){
            digitalWrite(rowNumber, LOW);
            digitalWrite(12, HIGH);
            digitalWrite(rowNumber, HIGH);
            digitalWrite(12, LOW);
          }
          if(songArray[i][r][1] == 1){
            digitalWrite(rowNumber, LOW);
            digitalWrite(13, HIGH);
            digitalWrite(rowNumber, HIGH);
            digitalWrite(13, LOW);
          }
        }
      }
      changeNote = false;
    }
    playSong = false;
  }
  redLedFeedback();
  timeRed();
  greenLedFeedback();
  timeGreen();
  recordGreenStartTime();
  recordRedStartTime();
  recordPauseStartTime();
}

//*** Interrupt handlers ***
void buttonPress() {
  if (analogRead(A0) >= 1000) {
    greenButtonOn = true;
    startGreen = true;
    lastPressedGreen = true;
  } else if (lastPressedGreen) {
    greenButtonOn = false;
    readyToTimeGreen = true;
    lastPressedGreen = false;
  }

  if (analogRead(A1) >= 1000) {
    redButtonOn = true;
    startRed = true;
    lastPressedRed = true;
  } else if (lastPressedRed) {
    redButtonOn = false;
    lastPressedRed = false;
    readyToTimeRed = true;
  }
}

void pauseButton() {
  pause = !pause;
  if (pause) {
    startPause = true;
  } else {
    readyToTimePause = true;
  }
  Serial.println("paused");
}

ISR(TIMER1_COMPA_vect) {
  //overflow counter
  changeNote = true;
  ovf_count ++;
  TCNT1 = 0;
}

//*** Helper Functions ***

double milliss() {
  double current_time = 0;
  current_time = ovf_count * 0.885 + TCNT1 * 0.0000016;
  return current_time;
}

void redLedFeedback() {
  if(redButtonOn) {
    digitalWrite(11, LOW);
    digitalWrite(12, HIGH);
  
    digitalWrite(11, HIGH);
    digitalWrite(12, LOW);
  }
}

void greenLedFeedback() {
  if(greenButtonOn) {
    digitalWrite(11, LOW);
    digitalWrite(13, HIGH);
  
    digitalWrite(11, HIGH);
    digitalWrite(13, LOW);
  }
}

void timeRed() {
  if (readyToTimeRed) {
    redLength = milliss() - redStartTime;
    readyToTimeRed = false;
    Serial.println("Red Length");
    Serial.println(redLength);
  }
}

void timeGreen() {
  if (readyToTimeGreen) {
    greenLength = milliss() - greenStartTime; 
    readyToTimeGreen = false;
    Serial.println("Green Length");
    Serial.println(greenLength);
  }
}

void timePause() {
  if (readyToTimePause) {
    pauseLength = milliss() - pauseStartTime;
    readyToTimePause = false;
    Serial.println("Pause Length");
    Serial.println(pauseLength);
  }
}

void recordGreenStartTime() {
  if (startGreen) {
    greenStartTime = milliss();
    startGreen = false;
  }
}

void recordRedStartTime() {
  if (startRed) {
    redStartTime = milliss();
    startRed = false;
  }
}

void recordPauseStartTime() {
  if (startPause) {
    pauseStartTime = milliss();
    startPause = false;
  }
}
