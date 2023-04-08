#include <RCSwitch.h>

RCSwitch rx = RCSwitch();
int rx_int = 0; // interrupt 0, pin #2

struct ref {
  int r_button_code;
  int w_button_code;
  int clr_button_code;
  int r_relay_pin;
  int w_relay_pin;
  int decision_state;
  int button_state;
  int button_consec_count;        
};

//NOTE we avoid using pins 0 and 1 because it's RX/TX for serial
// remember right is left from the back
int min_relay_pin = A0;
int max_relay_pin = A5;

int l_pos = 0;
int h_pos = 1;
int r_pos = 2;
int OFF = 0;
int RED = 1;
int WHITE = 2;
int CLEAR = 3;
int lights_fired = 0;

// we don't seem to have phantom reads with wireless, but we generally get 
// 2 reads on a button press
int BUTTON_READ_THRESHOLD = 2;

struct ref refs[3];

void update_ref_from_button_read(struct ref *refs, int button_code) {
  
  // find the ref and button state from the pin
  for (int ref_i = 0; ref_i < 3; ref_i++) {

    int new_button_state;    

    if (button_code == refs[ref_i].r_button_code) {
      new_button_state = RED;
    }
    else if (button_code == refs[ref_i].w_button_code) {
      new_button_state = WHITE;
    }
    // just in case, ignore "clear" from a ref that has the placeholder value.  In theory
    // a remote could default to -1 for the clear button; in wired systems they don't have 
    // a clear button
    else if ((button_code == refs[ref_i].clr_button_code) && (refs[ref_i].clr_button_code != -1)) {
      new_button_state = CLEAR;
    }
    else {
      // didn't match any pins for this ref, try next
      continue;
    }
    
    // update the ref button state if necc
    if (refs[ref_i].button_state != new_button_state) {
      refs[ref_i].button_state = new_button_state;
      refs[ref_i].button_consec_count = 0;
    }
    refs[ref_i].button_consec_count += 1;

    // if we got enough reads of the same button from the same ref, 
    // update decision state.  we use this threshold to ignore the
    // occasional phantom read    
    if (refs[ref_i].button_consec_count >= BUTTON_READ_THRESHOLD) {
      refs[ref_i].decision_state = refs[ref_i].button_state;
      Serial.print("DECISION STATE:REF:");
      Serial.print(ref_i);
      Serial.print(":");
      Serial.print(refs[ref_i].decision_state);
      Serial.print("\n");
    }
  }    
}

int is_decision_complete(struct ref *refs) {
  for (int ref_i = 0; ref_i < 3; ref_i++) {
    if ((refs[ref_i].decision_state != RED) && (refs[ref_i].decision_state != WHITE)) {
      return 0;
    }
  }
  Serial.print("DECISION:");
  for (int ref_i = 0; ref_i < 3; ref_i++) {
    Serial.print(refs[ref_i].decision_state);
  }
  Serial.print("\n");
  return 1;
}

void clear_refs(struct ref *refs) {
  for (int ref_i = 0; ref_i < 3; ref_i++) {
    refs[ref_i].button_state = OFF;
    refs[ref_i].button_consec_count = 0;
    refs[ref_i].decision_state = OFF;
    Serial.print("CLEAR REF:");
    Serial.println(ref_i);
  }
}

void set_lights(struct ref *refs) {
  for (int ref_i = 0; ref_i < 3; ref_i++) {
    if (refs[ref_i].decision_state == RED) {
      digitalWrite(refs[ref_i].r_relay_pin, HIGH);
      Serial.print("RELAY ON:REF:");
      Serial.print(ref_i);
      Serial.println("R");   
    }
    if (refs[ref_i].decision_state == WHITE) {
      digitalWrite(refs[ref_i].w_relay_pin, HIGH);
      Serial.print("RELAY ON:REF:");
      Serial.print(ref_i);
      Serial.println("W"); 
    }
    if (refs[ref_i].decision_state == OFF) {
      digitalWrite(refs[ref_i].r_relay_pin, LOW);
      digitalWrite(refs[ref_i].w_relay_pin, LOW);
      Serial.print("RELAYS OFF:REF:");
      Serial.println(ref_i);
    }    
  }
}

void setup(void) {

  Serial.begin(9600);

  rx.enableReceive(rx_int); // interrupt 0, pin #2
  Serial.print("Enabled receive for 433MhZ receiver on interrupt ");
  Serial.print(rx_int);
  Serial.print("\n");

  // setup relay output pins
  for (int relay_pin = min_relay_pin; relay_pin <= max_relay_pin; relay_pin++) {
    pinMode(relay_pin, OUTPUT);
    Serial.print("Set OUTPUT for relay ");
    Serial.print(relay_pin);
    Serial.print('\n');
  }

  refs[l_pos] = {
    8194,
    8196,
    -1,
    A1,
    A0,
    OFF,
    OFF,
    0
  };
  refs[h_pos] = {
    4098,
    4100,  
    4097,
    A3,
    A2,
    OFF,
    OFF,
    0
  };
  refs[r_pos] = {
    16386,
    16388,
    -1,
    A5,
    A4,
    OFF,
    OFF,
    0
  };
  Serial.println("REFS INITIALISED");
}

void loop(void) {

  int ref_button_code = 0;

  // get signal from radio
  if (rx.available()) {
    ref_button_code = rx.getReceivedValue();
    rx.resetAvailable();
  }

  // update ref state from what we read
  if (ref_button_code != 0) {
    update_ref_from_button_read(refs, ref_button_code);
  }  

  // if we have a complete decision and the lights haven't fired yet, do so
  if ((!lights_fired) && (is_decision_complete(refs))) {
    set_lights(refs);
    lights_fired = 1;
  }

  // if the head ref cleared, turn the lights off and 
  // clear ref button and decision states
  if (refs[h_pos].decision_state == CLEAR) {
    clear_refs(refs);
    set_lights(refs);
    lights_fired = 0;        
  }
}
