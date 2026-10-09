// Vars for calculations 
const int sensorPin = A0;
const int threshold = 900; 

volatile bool objectDetected = false;
volatile unsigned long pulseCount = 0;
unsigned long lastTime = 0;
const unsigned long sampleInterval = 800; // 1 second

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT);
}

void loop() {
  
  // Read sensor and check for detection
  int rpmSensorValue = analogRead(sensorPin);
  
  
  if (rpmSensorValue < threshold) {
    if (!objectDetected) {
      pulseCount++;
      objectDetected = true;
      
    }
  } else {
    objectDetected = false;
  }
  
  // Calculate RPM every second
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= sampleInterval) {
    // Disable interrupts briefly while reading
    noInterrupts();
    unsigned long pulses = pulseCount;
    pulseCount = 0;
    interrupts();
    
    // Calculate RPM
  
    float rpm = pulses * 60.0 / 2;  
    
    // This just displays the code to the console. this 
    Serial.print("Pulses: ");
    Serial.print(pulses);
    Serial.print(" | RPM: ");
    Serial.print(rpm);
    Serial.print(" | Raw: "  );
    Serial.println(rpmSensorValue);  
    lastTime = currentTime;
  }
}












