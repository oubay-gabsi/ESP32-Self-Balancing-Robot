#include <Wire.h>

// ================= PIN SETTINGS =================
// Change these if you moved pins for the PCB.
#define PIN_SDA   21
#define PIN_SCL   22

#define PIN_STBY  27
#define PIN_AIN1  26
#define PIN_AIN2  25
#define PIN_PWMA  33
#define PIN_BIN1  14
#define PIN_BIN2  12
#define PIN_PWMB  13

// ================= MPU6050 AXIS SETTINGS =================
// Fill these in from the axis test (Step 1).
#define ACC_TILT   ay     // accel axis that changes most when tilting
#define ACC_UP     az     // accel axis that reads about +16384 when upright
#define GYRO_TILT  gx     // gyro axis for the tilt (the remaining axis)
#define TILT_SIGN  1      // set -1 if leaning forward shows a negative angle
#define GYRO_SIGN  1      // set -1 if the angle moves away from the accel reading

// ================= MOTOR SETTINGS =================
#define MOTOR_A_DIR  -1
#define MOTOR_B_DIR  1   // was 1; the right motor is mirrored
#define OUTPUT_SIGN  1    // set -1 if the robot drives away from the lean
#define MIN_PWM      40   // lowest PWM where the motors really turn (test it, see notes)
#define DEAD_ZONE    2    // output units; tiny so small corrections still reach the motors
#define MAX_TILT     35   // degrees; motors stop if the robot tips further

// ================= TUNING HELPERS =================
// If the robot always runs in one direction, change this by 0.5 at a time.
// Try +0.5 once. If it stays up longer, keep going. If it is worse, go the other way.
#define SETPOINT_OFFSET  0.0   // degrees, added to the measured balance point

// If the robot drives in a curve, lower the stronger motor (for example 0.95).
#define MOTOR_A_TRIM  1.0
#define MOTOR_B_TRIM  1.0

// ================= PID GAINS =================
float Kp = 9.0;       //9
float Ki = 150.0;     //150
float Kd = 0.2;    // 0.11

// ================= VARIABLES =================
#define MPU_ADDR 0x68
int16_t ax, ay, az, gx, gy, gz;
float setpoint = 0;        // measured automatically at startup
float gyroBias = 0;        // gyro reading when still (deg/s), measured at startup
float angle = 0;           // filtered tilt angle (degrees)
float rateFilt = 0;        // smoothed gyro rate for the D term
float integral = 0;
unsigned long lastMicros = 0;
unsigned long lastPrint = 0;

// ================= SENSOR =================
int16_t readWord() {
  int16_t hi = Wire.read();
  int16_t lo = Wire.read();
  return (hi << 8) | lo;
}

void readMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)14, (uint8_t)true);
  ax = readWord();
  ay = readWord();
  az = readWord();
  readWord();                 // temperature, not used
  gx = readWord();
  gy = readWord();
  gz = readWord();
}

// Tilt angle from gravity (accurate but noisy)
float accelAngle() {
  return TILT_SIGN * atan2((float)ACC_TILT, (float)ACC_UP) * 180.0 / PI;
}

// Tilt speed from the gyro in degrees per second, with the still offset removed
float gyroRate() {
  return GYRO_SIGN * ((float)GYRO_TILT / 131.0 - gyroBias);
}

// Hold the robot upright and still. Measures the balance angle and the gyro offset.
void calibrateSetpoint() {
  float sum = 0, gsum = 0;
  for (int i = 0; i < 300; i++) {
    readMPU();
    sum += accelAngle();
    gsum += (float)GYRO_TILT / 131.0;
    delay(5);
  }
  setpoint = sum / 300.0 + SETPOINT_OFFSET;
  gyroBias = gsum / 300.0;
}

// ================= MOTORS =================
void setOneMotor(int in1, int in2, int pwmPin, bool forward, int pwm) {
  digitalWrite(in1, forward ? HIGH : LOW);
  digitalWrite(in2, forward ? LOW : HIGH);
  analogWrite(pwmPin, pwm);
}

void stopMotors() {
  digitalWrite(PIN_AIN1, LOW);
  digitalWrite(PIN_AIN2, LOW);
  digitalWrite(PIN_BIN1, LOW);
  digitalWrite(PIN_BIN2, LOW);
  analogWrite(PIN_PWMA, 0);
  analogWrite(PIN_PWMB, 0);
}

void driveMotors(float out) {
  float a = fabs(out);
  if (a < DEAD_ZONE) {           // tiny dead zone
    stopMotors();
    return;
  }
  // Map DEAD_ZONE..255 smoothly onto MIN_PWM..255
  int pwm = MIN_PWM + (int)((a - DEAD_ZONE) / (255.0 - DEAD_ZONE) * (255 - MIN_PWM));
  pwm = constrain(pwm, 0, 255);
  int pwmA = constrain((int)(pwm * MOTOR_A_TRIM), 0, 255);
  int pwmB = constrain((int)(pwm * MOTOR_B_TRIM), 0, 255);
  bool goForward = out > 0;
  setOneMotor(PIN_AIN1, PIN_AIN2, PIN_PWMA, goForward == (MOTOR_A_DIR > 0), pwmA);
  setOneMotor(PIN_BIN1, PIN_BIN2, PIN_PWMB, goForward == (MOTOR_B_DIR > 0), pwmB);
}

// ================= DASHBOARD DATA =================
// One CSV line: time, angle, error, P, I, D, output
void sendData(float error, float p, float i, float d, float out) {
  Serial.print(millis() / 1000.0, 3);  Serial.print(",");
  Serial.print(angle, 2);              Serial.print(",");
  Serial.print(error, 2);              Serial.print(",");
  Serial.print(p, 2);                  Serial.print(",");
  Serial.print(i, 2);                  Serial.print(",");
  Serial.print(d, 2);                  Serial.print(",");
  Serial.println(out, 1);
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  Wire.begin(PIN_SDA, PIN_SCL);
  Wire.setClock(400000);

  // Wake up the MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  // Low-pass filter inside the MPU6050 (about 44 Hz) to cut motor vibration noise
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1A);
  Wire.write(3);
  Wire.endTransmission(true);

  pinMode(PIN_STBY, OUTPUT);
  pinMode(PIN_AIN1, OUTPUT);
  pinMode(PIN_AIN2, OUTPUT);
  pinMode(PIN_PWMA, OUTPUT);
  pinMode(PIN_BIN1, OUTPUT);
  pinMode(PIN_BIN2, OUTPUT);
  pinMode(PIN_PWMB, OUTPUT);

  digitalWrite(PIN_STBY, HIGH);   // enable the TB6612
  stopMotors();

  Serial.println("Hold the robot upright and still...");
  delay(2000);
  calibrateSetpoint();
  angle = setpoint;
  rateFilt = 0;
  lastMicros = micros();

  Serial.print("Balance point (setpoint): ");
  Serial.println(setpoint, 2);
  Serial.print("Gyro offset (deg/s): ");
  Serial.println(gyroBias, 2);
}

// ================= MAIN LOOP (about 200 Hz) =================
void loop() {
  unsigned long now = micros();
  if (now - lastMicros < 5000) return;    // wait until 5 ms have passed
  float dt = (now - lastMicros) / 1000000.0;
  lastMicros = now;

  readMPU();
  float rate = gyroRate();

  // Smooth the rate for the D term (less noise, so a higher Kd is possible)
  rateFilt = 0.7 * rateFilt + 0.3 * rate;

  // Complementary filter: 99% gyro, 1% accelerometer
  // (less accelerometer = less error when the robot itself is accelerating)
  angle = 0.99 * (angle + rate * dt) + 0.01 * accelAngle();

  // error > 0 means leaning forward
  float error = angle - setpoint;

  // Fallen over: stop the motors and reset the integral
  if (fabs(error) > MAX_TILT) {
    stopMotors();
    integral = 0;
    // keep sending data so the dashboard still updates
    if (now - lastPrint >= 50000) {
      lastPrint = now;
      sendData(error, 0, 0, 0, 0);
    }
    return;
  }

  // PID
  integral += error * dt;
  //integral = constrain(integral, -20, 20);   // anti-windup (I term stays within +-60 at Ki = 3)

  float pTerm = Kp * error;
  float iTerm = Ki * integral;
  float dTerm = Kd * rateFilt;

  float output = pTerm + iTerm + dTerm;
  output = constrain(output * OUTPUT_SIGN, -255, 255);

  driveMotors(output);

  // Send one CSV line 20 times per second for the dashboard
  if (now - lastPrint >= 50000) {
    lastPrint = now;
    sendData(error, pTerm, iTerm, dTerm, output);
  }
}
