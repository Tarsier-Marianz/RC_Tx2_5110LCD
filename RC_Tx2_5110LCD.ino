#include <PCD8544.h>

//static PCD8544 lcd = PCD8544(sclk, sdin, dc, reset, sce);
static PCD8544 lcd = PCD8544(7, 6, 5, 3, 4);

/* Frequencies */
int PREAM_HIGH_FREQ     = 1500;
int PREAM_LOW_FREQ      = 500;
int FUNCTION_FREQ       = 500;
/* number of function code (N) W1 */
int CMD_ENDCODE         =  4;
int CMD_FORWARD         = 10;
int CMD_FORWARD_TURBO   = 16;
int CMD_TURBO           = 22;
int CMD_FORWARD_LEFT    = 28;
int CMD_FORWARD_RIGHT   = 34;
int CMD_BACKWARD        = 40;
int CMD_BACKWARD_RIGHT  = 46;
int CMD_BACKWARD_LEFT   = 52;
int CMD_LEFT            = 58;
int CMD_RIGHT           = 64;
int CMD_WIPE_LEFT       = 70;
int CMD_WIPE_RIGHT      = 76;
int CMD_PUSH            = 82;
/* output pin */
int SI_PIN              = 9;
int PUSH_PIN            = 2;
int PUSH_LED_STATE      = 11;
int JOYX_PIN            = A0;
int JOYY_PIN            = A1;
int JOYZ_PIN            = A2;
int X_val               = 0;
int Y_val               = 0;
int Z_val               = 0;
int pushState           = 0;

char cmd[4]; // array that holds command for two or more commands

int middlePoint = 1023 / 2;       // 1023 is the default max value of analog
int wiperMiddlePoint = 500 / 2;
int threshold = 80; // it helps reading sensitivity
int wiperThreshold = 100; // it helps reading sensitivity

void setup() {
  Serial.begin(9600);
  lcd.begin(84, 48);
  pinMode(JOYX_PIN, INPUT);
  pinMode(JOYY_PIN, INPUT);
  pinMode(JOYZ_PIN, INPUT);
  pinMode(PUSH_PIN, OUTPUT);
  pinMode(PUSH_LED_STATE, OUTPUT);
  pinMode(SI_PIN, OUTPUT);
}
void loop() {
  lcd.clear();
  X_val = analogRead(JOYX_PIN);
  Y_val = analogRead(JOYY_PIN);
  Z_val = analogRead(JOYZ_PIN);
  pushState = digitalRead(PUSH_PIN);

  if (Y_val > (middlePoint + threshold) || Y_val < (middlePoint - threshold)) {
    cmd[0] = (Y_val > (middlePoint + threshold))  ? 'F' : 'B';
  } else {
    cmd[0] = 0;
  }
  if (X_val > (middlePoint + threshold) || X_val < (middlePoint - threshold)) {
    cmd[1] = (X_val > (middlePoint + threshold)) ? 'L' : 'R';
  } else {
    cmd[1] = 0;
  }
  lcd.setCursor(0, 0);
  lcd.print("X-val: "); lcd.print(X_val);
  lcd.setCursor(0, 1);
  lcd.print("Y-val: "); lcd.print(Y_val);
  lcd.setCursor(0, 2);
  lcd.print("Z-val: "); lcd.print(Z_val);
  lcd.setCursor(0, 3);
  lcd.print("STATE: "); lcd.print(pushState);
  digitalWrite( PUSH_LED_STATE, pushState);
  command();
  //delay(10);
}
void command() {
  String cmds;
  for (int i = 0; i < 2; i++) {
    char c = cmd[i];
    if (c != NULL && c != '\0') {
      cmds += c;
    }
  }

  lcd.setCursor(0, 4);
  lcd.print("CMD  : "); lcd.print(cmds);
  lcd.setCursor(0, 5);
  // check if commands is not empty
  if (cmds != NULL && cmds != '\0') {
    Serial.println(cmds);
    if (cmds == "F") {
      encode_cmd(CMD_FORWARD);
    } else if (cmds == "B") {
      encode_cmd(CMD_BACKWARD);
    } else if (cmds == "L") {
      encode_cmd(CMD_LEFT);
    } else if (cmds == "R") {
      encode_cmd(CMD_RIGHT);
    } else if (cmds == "FR") {
      encode_cmd(CMD_FORWARD_RIGHT);
    } else if (cmds == "FL") {
      encode_cmd(CMD_FORWARD_LEFT);
    } else if (cmds == "BL") {
      encode_cmd(CMD_BACKWARD_LEFT);
    } else if (cmds == "BR") {
      encode_cmd(CMD_BACKWARD_RIGHT);
    } else {
      send_nosignal();
    }
  } else {
    if (Z_val > (wiperMiddlePoint + wiperThreshold) || Z_val < (wiperMiddlePoint - wiperThreshold)) {
      /*
        if (Z_val > (wiperMiddlePoint + wiperThreshold)) {
        lcd.print("--WIPE_RIGHT--");
        encode_cmd(CMD_WIPE_RIGHT);
        } else {
        lcd.print("--WIPE_LEFT--");
        encode_cmd(CMD_WIPE_LEFT);
        }*/
      /**** start for testing only ****/
      if (Z_val < 20 ) {
        lcd.print("--WIPE_LEFT--");
        encode_cmd(CMD_WIPE_LEFT);
      } else {
        if (Z_val > (500 - 20) ) {
          lcd.print("--WIPE_RIGHT--");
          encode_cmd(CMD_WIPE_RIGHT);
        } else {
          if (pushState == HIGH) {
            encode_cmd(CMD_PUSH);
          } else {
            send_nosignal();
            lcd.print("--NO SIGNAL--");
          }
        }
      }
      /**** end for testing only ****/
    } else {
      if (pushState == HIGH) {
        encode_cmd(CMD_PUSH);
      } else {
        send_nosignal();
        lcd.print("--NO SIGNAL--");
      }
    }
  }
}
void encode_cmd( int code) {
  int w1 = 0, w2 = 0;
  /* start code  4 x W2, 500Hz, Duty Cycle 75% */
  for ( w2 = 0; w2 < CMD_ENDCODE ; w2++) {
    digitalWrite( SI_PIN, HIGH);
    delayMicroseconds( PREAM_HIGH_FREQ);
    digitalWrite( SI_PIN, LOW);
    delayMicroseconds( PREAM_LOW_FREQ);
  }
  /* function code , n x W1, 1KHz, Duty Cycle 50% */
  for ( w1 = 0; w1 < code; w1++) {
    digitalWrite( SI_PIN, HIGH);
    delayMicroseconds(FUNCTION_FREQ);
    digitalWrite( SI_PIN, LOW);
    delayMicroseconds(FUNCTION_FREQ);
  }
}

void send_nosignal() {
  //digitalWrite( SI_PIN, LOW);
  //delayMicroseconds(FUNCTION_FREQ);
}
