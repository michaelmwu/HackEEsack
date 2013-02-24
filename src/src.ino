#define ACCEL_WINDOW 3
#define ACCEL_SAMPLES 4
int accelSamples[ACCEL_SAMPLES];
int accelOffset = 0;
const int accelSensor = A0; // the accelerometer is connected to analog pin 0
const int kickThreshold = 12;
const int fallThreshold = 7;

const int STATE_STATIONARY = 0;
const int STATE_KICK = 1;
const int STATE_FALL = 2;
int hacky_state = STATE_STATIONARY;
unsigned long lastStateTime;

// Time stationary until we assume the hacksack is done
const int STATIONARY_TIME = 3000;

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
}

// Main
void loop() {
  delay(50);
  recordAccelSample();
  
  int reading = accelReading();
  Serial.println(reading);
}

// Add it to the accelerometer buffers
void recordAccelSample() {
  int sample = analogRead(accelSensor);
  accelSamples[accelOffset] = sample;

  accelOffset++;
  if(accelOffset >= ACCEL_SAMPLES) {
    accelOffset = 0;
  }
}

// Get a weighted accelerometer reading
int accelReading() {
  int total = 0;
  
  for(int i = 0; i < ACCEL_WINDOW; i++) {
    int offset = accelOffset - i - 1;
    
    total += accelSamples[offset];
  }
  
  return total / ACCEL_WINDOW;
}

void stateMachine(int reading) {
  unsigned long since_last = millis() - lastStateTime;
  
  if(hacky_state == STATE_STATIONARY) {
  }
  else if(hacky_state == STATE_KICK) {
  }
  else if(hacky_state == STATE_FALL) {
  }
}
