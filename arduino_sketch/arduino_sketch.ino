struct ref {
  int r_button_pin;
  int w_button_pin;
  int clr_button_pin;
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

//NOTE - we avoid pin 13 because has an onboard LED and pulldown resistor hanging off of it
//meaning it always reads LOW even when the switch is closed
int min_button_pin = 6;
int max_button_pin = 12;

int l_pos = 0;
int h_pos = 1;
int r_pos = 2;
int OFF = 0;
int RED = 1;
int WHITE = 2;
int CLEAR = 3;
int lights_fired = 0;

// how many consecutive reads of the same state do we need
// to acknowledge?
int BUTTON_READ_THRESHOLD = 3;

struct ref refs[3];

void update_ref_from_button_read(struct ref *refs, int button_pin) {
  
  // find the ref and button state from the pin
  for (int ref_i = 0; ref_i < 3; ref_i++) {

    int new_button_state;    

    if (button_pin == refs[ref_i].r_button_pin) {
      new_button_state = RED;
    }
    else if (button_pin == refs[ref_i].w_button_pin) {
      new_button_state = WHITE;
    }
    else if (button_pin == refs[ref_i].clr_button_pin) {
      new_button_state = CLEAR;
    }
    else {
      continue;
    }
    
    // update the ref button state if necc
    if (refs[ref_i].button_state != new_button_state) {
      refs[ref_i].button_state = new_button_state;
      refs[ref_i].button_consec_count = 0;
    }
    refs[ref_i].button_consec_count += 1;

    // if we got enough button reads, update decision state    
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
      Serial.println("R"); 
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

  // setup relay output pins
  for (int relay_pin = min_relay_pin; relay_pin <= max_relay_pin; relay_pin++) {
    pinMode(relay_pin, OUTPUT);
    Serial.print("Set OUTPUT for relay ");
    Serial.print(relay_pin);
    Serial.print('\n');
  }

  // setup button input pins - use internal pullup resistors
  // this means when circuit is open, pin is pulled high, and
  // when circuit is closed, it goes low
  for (int button_pin = min_button_pin; button_pin <= max_button_pin; button_pin++) {
    pinMode(button_pin, INPUT_PULLUP);
    Serial.print("Set INPUT_PULLUP for button ");
    Serial.print(button_pin);
    Serial.print('\n');
  }

  refs[0] = {
    11,
    12,
    -1,
    A1,
    A0,
    OFF,
    OFF,
    0
  };
  refs[1] = {
    8,
    9,  
    10,
    A3,
    A2,
    OFF,
    OFF,
    0
  };
  refs[2] = {
    6,
    7,
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

  // poll ref input buttons
  for (int button_pin = min_button_pin; button_pin <= max_button_pin; button_pin++) {
    int button_read = digitalRead(button_pin);
    
    //INPUT_PULLUP means button close makes the read LOW
    if (button_read == LOW) {
      Serial.print("BUTTON READ PIN:");
      Serial.println(button_pin);
      update_ref_from_button_read(refs, button_pin);      
    }
  }

  // if we have a complete decision and the lights haven't fired yet, do so
  if ((!lights_fired) && (is_decision_complete(refs))) {
    set_lights(refs);
    lights_fired = 1;
  }

  // if the head ref cleared, turn the lights off and 
  // clear ref button and decision states
  if (refs[h_pos].button_state == CLEAR) {
    clear_refs(refs);
    set_lights(refs);
    lights_fired = 0;        
  }
}
