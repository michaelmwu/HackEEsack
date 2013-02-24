#define ACCEL_WINDOW 3
#define ACCEL_SAMPLES 4
unsigned long accelSamples[ACCEL_SAMPLES];
int accelOffset = 0;
const int accelSensorX = A0; // the accelerometer is connected to analog pin 3, 6, 7
const int accelSensorY = A1;
const int accelSensorZ = A2;
const int ACCEL_MIDDLE = 512;

const unsigned long kickThreshold = 1000000;
const unsigned long fallThreshold = 5000;
const unsigned long FALL_TIME = 100;
const unsigned long KICKSTATIONARY_TIME = 200;

const int STATE_STATIONARY = 0;
const int STATE_KICK = 1;
const int STATE_FALL = 2;
int hacky_state = STATE_STATIONARY;
unsigned long lastStateTime;

unsigned long hits = 0;

// Time stationary until we assume the hacksack is done
const unsigned long STATIONARY_TIME = 3000;

// Setup code
void setup() {
  reset();
  
  // Set up serial port
  Serial.begin(9600);       // use the serial port
}

void reset() {
  // Zero out accelerometer data array
  memset(accelSamples,0,sizeof(accelSamples));
  lastStateTime = millis();
  hits = 0;
}

// Main
void loop() {
  delay(50);
  recordAccelSample();
  
  unsigned long reading = accelReading();
//  Serial.print("weighted: ");
//  Serial.println(reading);
  stateMachine(reading);
}

int rebase(int sample) {
  sample -= ACCEL_MIDDLE;
  return sample * sample;
}

// Add it to the accelerometer buffers
void recordAccelSample() {
  int xsample = analogRead(accelSensorX);
  int ysample = analogRead(accelSensorY);
  int zsample = analogRead(accelSensorZ);
  
//  Serial.print("X: ");
//  Serial.print(xsample);
//  Serial.print(" Y: ");
//  Serial.print(ysample);
//  Serial.print(" Z: ");
//  Serial.print(zsample);
  
  unsigned long sample = rebase(xsample)+rebase(ysample)+rebase(zsample);
//  Serial.print("final: ");
//  Serial.println(sample);
  
  accelSamples[accelOffset] = sample;

  accelOffset++;
  if(accelOffset >= ACCEL_SAMPLES) {
    accelOffset = 0;
  }
}

// Get a weighted accelerometer reading
unsigned long accelReading() {
  unsigned long total = 0;
  
  for(int i = 0; i < ACCEL_WINDOW; i++) {
    int offset = (accelOffset - i - 1 + ACCEL_SAMPLES) % ACCEL_SAMPLES;
    
    total += accelSamples[offset];
  }
  
  return total / ACCEL_WINDOW;
}

void stateMachine(unsigned long reading) {
  unsigned long since_last = millis() - lastStateTime;
  int next_state = hacky_state;
  
//  Serial.print("STATE: ");
//  if(hacky_state == STATE_STATIONARY) {
//    Serial.println("STATIONARY");
//  }
//  else if(hacky_state == STATE_FALL) {
//    Serial.println("FALL");
//  } else if(hacky_state == STATE_KICK) {
//    Serial.println("KICK");
//  }
  
  if(hacky_state == STATE_STATIONARY) {
    if(reading > kickThreshold) {
      next_state = STATE_KICK;
      hits += 1;
      Serial.print("hits: ");
      Serial.println(hits);
    }
    else if(reading < fallThreshold) {
      next_state = STATE_FALL && since_last >= FALL_TIME;
    }
    
    if(since_last >= STATIONARY_TIME) {
      Serial.println("RESET");
      reset();
    }
  }
  else if(hacky_state == STATE_KICK) {
    if(reading < fallThreshold && since_last >= FALL_TIME) {
      next_state = STATE_FALL;
    }
    else if(reading < kickThreshold && since_last >= KICKSTATIONARY_TIME) {
      next_state = STATE_STATIONARY;
    }
  }
  else if(hacky_state == STATE_FALL) {
    if(reading > kickThreshold) {
      next_state = STATE_KICK;
      hits += 1;
      Serial.print("hits: ");
      Serial.println(hits);
    }
    else if(reading > fallThreshold && since_last >= KICKSTATIONARY_TIME) {
      next_state = STATE_STATIONARY;
    }
  }

  if(hacky_state != next_state) {
    Serial.print("STATE: ");
    if(hacky_state == STATE_STATIONARY) {
      Serial.println("STATIONARY");
    }
    else if(hacky_state == STATE_FALL) {
      Serial.println("FALL");
    } else if(hacky_state == STATE_KICK) {
      Serial.println("KICK");
    }
    
    Serial.print("NEXT STATE: ");
    if(next_state == STATE_STATIONARY) {
      Serial.println("STATIONARY");
    }
    else if(next_state == STATE_FALL) {
      Serial.println("FALL");
    } else if(next_state == STATE_KICK) {
      Serial.println("KICK");
    }
    
    lastStateTime = millis();
    hacky_state = next_state;
  }
}
