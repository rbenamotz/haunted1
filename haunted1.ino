const int PIN_RADAR_IN = 12;
const int PIN_RELAY_1 = 3;
const int PIN_RELAY_2 = 4;
const int PIN_RELAY_3 = 5;
const int PIN_STATUS_LED = LED_BUILTIN;

//First number: time after movement detection, 2nd number: duration.
//Example {5,2} = Wait 5 seconds after movement, turn on realy for 2.
const int RELAY_1_SEQ[] = {1000, 2000};
const int RELAY_2_SEQ[] = {3000, 2000};
const int RELAY_3_SEQ[] = {5000, 2000};

const int PAUSE_AFTER_SEQ = 5000;
const int ASSUME_NO_MOVEMENT = 10000;

/*
 * Status LED
 * ------------------
 * Off: Init
 * Short blink every 2 seconds: waiting for movement
 * On: Executing scare sequence
 * Rapid blink: Sequence ended, pauising for PAUSE_AFTER_SEQ ms
 */


int totalExecutionTime = 0;

void setup() {
  totalExecutionTime = max(RELAY_1_SEQ[0] + RELAY_1_SEQ[1], RELAY_2_SEQ[0] + RELAY_2_SEQ[1]);
  totalExecutionTime = max(totalExecutionTime, RELAY_3_SEQ[0] + RELAY_3_SEQ[1]);
  totalExecutionTime = totalExecutionTime;
  Serial.begin(9600);
  pinMode(PIN_RADAR_IN, INPUT);
  pinMode(PIN_RELAY_1, OUTPUT);
  digitalWrite(PIN_RELAY_1, LOW);
  pinMode(PIN_RELAY_2, OUTPUT);
  digitalWrite(PIN_RELAY_2, LOW);
  pinMode(PIN_RELAY_3, OUTPUT);
  digitalWrite(PIN_RELAY_3, LOW);
  pinMode(PIN_STATUS_LED, OUTPUT);
  digitalWrite(PIN_STATUS_LED, LOW);
  Serial.println("Haunted house motion detector");
  Serial.print("\nStarting haunted. Total seq execution time: ");
  Serial.println(totalExecutionTime);
  Serial.print("Waiting for radar to init...");
  delay(2000); //Let the radar init
  Serial.println("Done.");
}

void loop() {
  Serial.println("Waiting for movement");
  bool b = digitalRead(PIN_RADAR_IN);
  bool statusLedState = true;
  unsigned long lastStatusLedChange = 0;
  //Wait until radata goes high
  while (!b) {
    unsigned long l = millis() - lastStatusLedChange;
    int p = statusLedState ? 2000 : 5;
    if (l >= p) {
      digitalWrite(PIN_STATUS_LED, statusLedState);
      statusLedState = !statusLedState;
      lastStatusLedChange = millis();
    }
    b = digitalRead(PIN_RADAR_IN);
    delay(10);

  }
  Serial.println("*******Detected motion**********");
  runSequence();
  b = digitalRead(PIN_RADAR_IN);

  //Wait until radar goes low
  unsigned long t1 = millis();
  while (b) {
    b = digitalRead(PIN_RADAR_IN);
    delay(100);
    unsigned long t2 = millis() - t1;
    if (t2 >= ASSUME_NO_MOVEMENT) {
      Serial.print("Clearing movement after ");
      Serial.print(ASSUME_NO_MOVEMENT);
      Serial.println("ms");
      break;
    }
  }
}

void runSequence() {
  String lastStr = "";
  Serial.println("Running scare sequence");
  digitalWrite(PIN_STATUS_LED, HIGH);
  unsigned long start = millis();
  unsigned long l = millis() - start;
  while (l <= totalExecutionTime) {
    bool b1 = (l >= RELAY_1_SEQ[0]  && l < RELAY_1_SEQ[0]  + RELAY_1_SEQ[1] );
    bool b2 = (l >= RELAY_2_SEQ[0]  && l < RELAY_2_SEQ[0]  + RELAY_2_SEQ[1] );
    bool b3 = (l >= RELAY_3_SEQ[0]  && l < RELAY_3_SEQ[0]  + RELAY_3_SEQ[1] );
    digitalWrite(PIN_RELAY_1, b1);
    digitalWrite(PIN_RELAY_2, b2);
    digitalWrite(PIN_RELAY_3, b3);
    String s = String(b1) + "," + String(b2) + "," + String(b3);
    if (s != lastStr) {
      Serial.print(l);
      Serial.print(": ");
      Serial.println(s);
      lastStr = s;
    }
    l = millis() - start;
  }
  digitalWrite(PIN_RELAY_1, false);
  digitalWrite(PIN_RELAY_2, false);
  digitalWrite(PIN_RELAY_3, false);
  Serial.print("Sequence ended. Pausing for ");
  Serial.print(PAUSE_AFTER_SEQ);
  Serial.println("ms");
  start = millis();
  l = 0;
  bool b = true;
  while (l < PAUSE_AFTER_SEQ) {
    digitalWrite(PIN_STATUS_LED, b);
    delay (100);
    b = !b;
    l = millis() - start;
  }
  Serial.println("done.");
  digitalWrite(PIN_STATUS_LED, LOW);
}
