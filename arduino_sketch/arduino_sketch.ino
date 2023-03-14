//NOTE we avoid using pins 0 and 1 because it's RX/TX for serial
// remember right is left from the back
int min_relay_pin = A0;
int max_relay_pin = A5;

int r_r_relay_pin = A5;
int r_w_relay_pin = A4;
int h_r_relay_pin = A3;
int h_w_relay_pin = A2;
int l_r_relay_pin = A1;
int l_w_relay_pin = A0;

//NOTE - we avoid pin 13 because has an onboard LED and pulldown resistor hanging off of it
//meaning it always reads LOW even when the switch is closed
int min_button_pin = 6;
int max_button_pin = 12;

int r_r_button_pin = 6;
int r_w_button_pin = 7;
int h_r_button_pin = 8;
int h_w_button_pin = 9;
int h_clr_button_pin = 10;
int l_r_button_pin = 11;
int l_w_button_pin = 12;


int light_states[3];
int l_light_state_pos = 0;
int h_light_state_pos = 1;
int r_light_state_pos = 2;
int OFF = 0;
int RED = 1;
int WHITE = 2;
int fired_lights = 0;

// how many consecutive reads of the same state do we need
// to acknowledge?
int BUTTON_READ_THRESHOLD = 3;
int button_consec_reads[3];
int button_states[3];

int is_decision_complete(int *light_states);
void clear_light_and_button_states(int *light_states);
int set_light_state_from_button(int *light_states, int button_pin);
void print_light_states(int *light_states);

int is_decision_complete(int *light_states) {
  for (int light_pos = 0; light_pos <= 2; light_pos++) {
    if (light_states[light_pos] == OFF) {
      return 0;
    }
  }
  return 1;
}

void clear_light_and_button_states(int *light_states, int *button_states, int *button_consec_reads) {
  for (int light_pos = 0; light_pos <= 2; light_pos++) {
    light_states[light_pos] = OFF;
    button_states[light_pos] = OFF;
    button_consec_reads[light_pos] = 0;
  }
}

int set_button_check_threshold(int *button_states, int *button_consec_reads, int pos, int check_state) {
  if (button_states[pos] == check_state) {
    button_consec_reads[pos] += 1;
    if (button_consec_reads[pos] >= BUTTON_READ_THRESHOLD) {
      return 1;
    }
  } else {
    button_states[pos] = check_state;
    button_consec_reads[pos] = 1;
  }
  return 0;
}

//TODO - rewrite using button threshold logic
int set_light_state_from_button(int *light_states, int *button_states, int *button_consec_reads, int button_pin) {

  //write light state from button state only if that light state is OFF
  if (light_states[l_light_state_pos] == OFF) {
    if ((button_pin == l_r_button_pin) && set_button_check_threshold(button_states, button_consec_reads, l_light_state_pos, RED)) {
      light_states[l_light_state_pos] = RED;
      return 1;
    }
    if ((button_pin == l_w_button_pin) && set_button_check_threshold(button_states, button_consec_reads, l_light_state_pos, WHITE)) {
      light_states[l_light_state_pos] = WHITE;
      return 1;
    }
  }
  if (light_states[h_light_state_pos] == OFF) {
    if ((button_pin == h_r_button_pin) && set_button_check_threshold(button_states, button_consec_reads, h_light_state_pos, RED)) {
      light_states[h_light_state_pos] = RED;
      return 1;
    }
    if ((button_pin == h_w_button_pin) && set_button_check_threshold(button_states, button_consec_reads, h_light_state_pos, WHITE)) {
      light_states[h_light_state_pos] = WHITE;
      return 1;
    }
  }
  if (light_states[r_light_state_pos] == OFF) {
    if ((button_pin == r_r_button_pin) && set_button_check_threshold(button_states, button_consec_reads, r_light_state_pos, RED)) {
      light_states[r_light_state_pos] = RED;
      return 1;
    }
    if ((button_pin == r_w_button_pin) && set_button_check_threshold(button_states, button_consec_reads, r_light_state_pos, WHITE)) {
      light_states[r_light_state_pos] = WHITE;
      return 1;
    }
  }
  return 0;
}

void set_lights_from_states(int *light_states) {
  Serial.print("RELAY SET:");  
  for (int light_pos = 0; light_pos <= 2; light_pos++) {
    if ((light_states[light_pos] == RED) || (light_states[light_pos] == OFF)) {
      if (light_pos == l_light_state_pos) {
        digitalWrite(l_w_relay_pin, LOW);
        Serial.print("LW:OFF:");            
      }
      if (light_pos == h_light_state_pos) {
        digitalWrite(h_w_relay_pin, LOW);
        Serial.print("HW:OFF:");        
      }
      if (light_pos == r_light_state_pos) {
        digitalWrite(r_w_relay_pin, LOW);
        Serial.print("RW:OFF:");        
      }
    }
    if ((light_states[light_pos] == WHITE) || (light_states[light_pos] == OFF)) {
      if (light_pos == l_light_state_pos) {
        digitalWrite(l_r_relay_pin, LOW);
        Serial.print("LR:OFF:");    
      }
      if (light_pos == h_light_state_pos) {
        digitalWrite(h_r_relay_pin, LOW);
        Serial.print("HR:OFF:");
      }
      if (light_pos == r_light_state_pos) {
        digitalWrite(r_r_relay_pin, LOW);
        Serial.print("RR:OFF:");
      }
    }
    if (light_states[light_pos] == RED) {
      if (light_pos == l_light_state_pos) {
        digitalWrite(l_r_relay_pin, HIGH);
        Serial.print("LR:ON:");    
      }
      if (light_pos == h_light_state_pos) {
        digitalWrite(h_r_relay_pin, HIGH);
        Serial.print("HR:ON:");
      }
      if (light_pos == r_light_state_pos) {
        digitalWrite(r_r_relay_pin, HIGH);
        Serial.print("RR:ON:");
      }
    }
    if (light_states[light_pos] == WHITE) {
      if (light_pos == l_light_state_pos) {
        digitalWrite(l_w_relay_pin, HIGH);
        Serial.print("LW:ON:");    
      }
      if (light_pos == h_light_state_pos) {
        digitalWrite(h_w_relay_pin, HIGH);
        Serial.print("HW:ON:");
      }
      if (light_pos == r_light_state_pos) {
        digitalWrite(r_w_relay_pin, HIGH);
        Serial.print("RW:ON:");
      }
    }
  }
  Serial.print("\n");
}

void print_light_states(int *light_states) {
  Serial.print("LIGHT STATE:");
  for (int light_pos = 0; light_pos <= 2; light_pos++) {
    if (light_states[light_pos] == RED) {
      Serial.print('R');
    }
    if (light_states[light_pos] == WHITE) {
      Serial.print('W');  
    }
    if (light_states[light_pos] == OFF) {
      Serial.print('X');
    }
  }
  Serial.print('\n');
}

void lights_test_pattern(int *light_states) {

  //first test - RWR, leave it for 5s
  light_states[l_light_state_pos] = RED;
  light_states[h_light_state_pos] = WHITE;
  light_states[r_light_state_pos] = RED;
  set_lights_from_states(light_states);
  print_light_states(light_states);
  delay(5000);

  //clear, leave for 1s
  clear_light_and_button_states(light_states, button_states, button_consec_reads);
  set_lights_from_states(light_states);
  print_light_states(light_states);
  delay(1000);

  //second test - WRW, 5s
  light_states[l_light_state_pos] = WHITE;
  light_states[h_light_state_pos] = RED;
  light_states[r_light_state_pos] = WHITE;
  set_lights_from_states(light_states);
  print_light_states(light_states);
  delay(5000);

  //clear, leave for 1s
  clear_light_and_button_states(light_states, button_states, button_consec_reads);
  set_lights_from_states(light_states);
  print_light_states(light_states);
  delay(1000);

  //third test - RRR, 5s
  light_states[l_light_state_pos] = RED;
  light_states[h_light_state_pos] = RED;
  light_states[r_light_state_pos] = RED;
  set_lights_from_states(light_states);
  print_light_states(light_states);
  delay(5000);

  //clear, leave for 1s
  clear_light_and_button_states(light_states, button_states, button_consec_reads);
  set_lights_from_states(light_states);
  print_light_states(light_states);
  delay(1000);

  //fourth test, WWW, 5s
  light_states[l_light_state_pos] = WHITE;
  light_states[h_light_state_pos] = WHITE;
  light_states[r_light_state_pos] = WHITE;
  set_lights_from_states(light_states);
  print_light_states(light_states);
  delay(5000);

  //clear, leave for 1s
  clear_light_and_button_states(light_states, button_states, button_consec_reads);
  set_lights_from_states(light_states);
  print_light_states(light_states);
  delay(1000);

  //fifth test, RRW, 5s
  light_states[l_light_state_pos] = RED;
  light_states[h_light_state_pos] = RED;
  light_states[r_light_state_pos] = WHITE;
  set_lights_from_states(light_states);
  print_light_states(light_states);
  delay(5000);

  //clear, leave for 1s
  clear_light_and_button_states(light_states, button_states, button_consec_reads);
  set_lights_from_states(light_states);
  print_light_states(light_states);
  delay(1000);

  //sixth test, WWR, 5s
  light_states[l_light_state_pos] = WHITE;
  light_states[h_light_state_pos] = WHITE;
  light_states[r_light_state_pos] = RED;
  set_lights_from_states(light_states);
  print_light_states(light_states);
  delay(5000);

  //clear, leave for 1s
  clear_light_and_button_states(light_states, button_states, button_consec_reads);
  set_lights_from_states(light_states);
  print_light_states(light_states);
  delay(1000);
  
}

void setup() {
  // put your setup code here, to run once:

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

  // setup initial light states
  clear_light_and_button_states(light_states, button_states, button_consec_reads);
  print_light_states(light_states);


  // set initial relay state from light state
  set_lights_from_states(light_states);

  //go through a test pattern
  lights_test_pattern(light_states);

  //back to initial
  clear_light_and_button_states(light_states, button_states, button_consec_reads);
  print_light_states(light_states);
  set_lights_from_states(light_states);

}

void loop() {
  // put your main code here, to run repeatedly:

  // if our decision isn't complete, poll the buttons except clear
  if (!is_decision_complete(light_states)) {
    for (int button_pin = min_button_pin; button_pin <= max_button_pin; button_pin++) {
      // button closed: state LOW due to pullup
      int cur_button_pin_read = digitalRead(button_pin);
      if (cur_button_pin_read == LOW) {
        Serial.print("BUTTON PRESS PIN:");
        Serial.println(button_pin);
        if (button_pin != h_clr_button_pin) {
          // only print light state if we changed state, we can get a few
          // reads per press
          if (set_light_state_from_button(light_states, button_states, button_consec_reads, button_pin)) {
            print_light_states(light_states);
          }
        }
      }
    }
  // decision complete - turn relays on and poll clear button   
  } else {

    // make sure we only fire relays once per decision
    if (!fired_lights) {
      print_light_states(light_states);
      set_lights_from_states(light_states);
      fired_lights = 1;
    }
 
    //if the clear button is closed, clear light states and turn relays off
    if (digitalRead(h_clr_button_pin) == LOW && fired_lights) {
      clear_light_and_button_states(light_states, button_states, button_consec_reads);
      set_lights_from_states(light_states);
      fired_lights = 0;
   }
 }
}
