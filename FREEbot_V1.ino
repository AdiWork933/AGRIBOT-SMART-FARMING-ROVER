#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_BMP085.h>
#include <Wire.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// Motor pins
const int in1 = 27, in2 = 26, ena = 14;
const int in3 = 25, in4 = 33, enb = 32;

// Ultrasonic pins
#define TRIG_PIN 5
#define ECHO_PIN 18

// Relay pins (NEW)
const int GEAR_MOTOR_RELAY_PIN = 19;
const int PUMP_MOTOR_RELAY_PIN = 23;
const int LED_PIN = 2; // onboard LED

// Sensor data
float distanceCm = 0;
float temperature = 0;
float pressure = 0;
String objectStatus = "clear";

// Default speed
int motorSpeed = 150;
const int maxSpeed = 180;

// BMP180 + Web server
Adafruit_BMP085 bmp;
WebServer server(80);

// Gear motor toggle
bool gearMotorActive = false;
bool gearRelayState = false;
unsigned long lastToggleTime = 0;

// ===== Motor Control Functions =====
void forward(int speed) {
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
  analogWrite(ena, speed); analogWrite(enb, speed);
}

void backward(int speed) {
  digitalWrite(in1, LOW); digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH);
  analogWrite(ena, speed); analogWrite(enb, speed);
}

void left(int speed) {
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH);
  analogWrite(ena, speed); analogWrite(enb, speed);
}

void right(int speed) {
  digitalWrite(in1, LOW); digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
  analogWrite(ena, speed); analogWrite(enb, speed);
}

void Stop() {
  digitalWrite(in1, LOW); digitalWrite(in2, LOW);
  digitalWrite(in3, LOW); digitalWrite(in4, LOW);
  analogWrite(ena, 0); analogWrite(enb, 0);
}

// ===== Read Sensors =====
void readSensors() {
  float total = 0;
  int validReadings = 0;

  for (int i = 0; i < 5; i++) {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(5);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
    float dist = duration * 0.0343 / 2;

    if (dist > 2 && dist < 300) {
      total += dist;
      validReadings++;
    }
    delay(10);
  }

  if (validReadings > 0) {
    distanceCm = total / validReadings;
  } else {
    distanceCm = 0;
  }

  temperature = bmp.readTemperature();
  pressure = bmp.readPressure() / 100.0F;
  objectStatus = (distanceCm > 0 && distanceCm < 10) ? "object is coming" : "clear";
}

// ===== Web Page HTML =====
String htmlPage() {
  readSensors();
  String html = R"====(
  <!DOCTYPE html><html><head>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <style>
    body { font-family: Arial; text-align: center; padding: 20px; }
    button { padding: 14px 28px; font-size: 16px; margin: 5px; }
    input[type=range] { width: 80%; }
    .sensor { font-size: 18px; margin-top: 10px; }
  </style></head><body>
  <h2>ESP32 Rover Control Panel</h2>

  <div class='sensor'>
    <p><b>Distance:</b> )====" + String(distanceCm, 1) + R"====( cm</p>
    <p><b>Status:</b> )====" + objectStatus + R"====(</p>
    <p><b>Temperature:</b> )====" + String(temperature, 1) + R"====( °C</p>
    <p><b>Pressure:</b> )====" + String(pressure, 1) + R"====( hPa</p>
  </div>

  <p><b>Speed: </b><span id='speedVal'>)====" + String(motorSpeed) + R"====(</span></p>
  <input type='range' min='0' max='180' value=')====" + String(motorSpeed) + R"====(' id='speedSlider' oninput='updateSpeed(this.value)'>

  <p>
    <button onclick="move('f')">Forward</button><br>
    <button onclick="move('l')">Left</button>
    <button onclick="move('s')">Stop</button>
    <button onclick="move('r')">Right</button><br>
    <button onclick="move('b')">Backward</button>
  </p>

  <h3>Accessory Controls</h3>
  <p>
    <button onclick="controlRelay('gear', 'on')">Gear Motor ON</button>
    <button onclick="controlRelay('gear', 'off')">Gear Motor OFF</button>
  </p>
  <p>
    <button onclick="controlRelay('pump', 'on')">Water Pump ON</button>
    <button onclick="controlRelay('pump', 'off')">Water Pump OFF</button>
  </p>

  <script>
    function updateSpeed(val) {
      document.getElementById('speedVal').innerText = val;
    }

    function move(dir) {
      let speed = document.getElementById('speedSlider').value;
      fetch(`/move?dir=${dir}&speed=${speed}`);
    }

    function controlRelay(device, action) {
      fetch(`/relay?device=${device}&action=${action}`);
    }

    setInterval(() => {
      location.reload(); // Refresh every 2 seconds for live data
    }, 2000);
  </script>

  </body></html>
  )====";
  return html;
}

// ===== Web Routes =====
void handleRoot() {
  server.send(200, "text/html", htmlPage());
}

void handleMove() {
  String dir = server.arg("dir");
  int speed = constrain(server.arg("speed").toInt(), 0, maxSpeed);
  motorSpeed = speed;

  if (dir == "f") forward(speed);
  else if (dir == "b") backward(speed);
  else if (dir == "l") left(speed);
  else if (dir == "r") right(speed);
  else Stop();

  server.send(200, "text/plain", "OK");
}

void handleRelayControl() {
  String device = server.arg("device");
  String action = server.arg("action");

  if (device == "pump") {
    digitalWrite(PUMP_MOTOR_RELAY_PIN, (action == "on") ? HIGH : LOW);
  } else if (device == "gear") {
    gearMotorActive = (action == "on");
    if (!gearMotorActive) {
      digitalWrite(GEAR_MOTOR_RELAY_PIN, LOW); // turn off immediately
      digitalWrite(LED_PIN, HIGH); // solid if connected
    }
  }

  server.send(200, "text/plain", "OK");
}

// ===== Setup =====
void setup() {
  Serial.begin(115200);

  pinMode(in1, OUTPUT); pinMode(in2, OUTPUT); pinMode(ena, OUTPUT);
  pinMode(in3, OUTPUT); pinMode(in4, OUTPUT); pinMode(enb, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(GEAR_MOTOR_RELAY_PIN, OUTPUT);
  pinMode(PUMP_MOTOR_RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(GEAR_MOTOR_RELAY_PIN, LOW);
  digitalWrite(PUMP_MOTOR_RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nConnected to WiFi! IP: " + WiFi.localIP().toString());
  digitalWrite(LED_PIN, HIGH); // Connected indicator

  if (!bmp.begin()) {
    Serial.println("BMP180 not found!");
    while (1);
  }

  server.on("/", handleRoot);
  server.on("/move", handleMove);
  server.on("/relay", handleRelayControl);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();

  if (gearMotorActive) {
    unsigned long now = millis();
    if (now - lastToggleTime >= 20) {
      gearRelayState = !gearRelayState;
      digitalWrite(GEAR_MOTOR_RELAY_PIN, gearRelayState);
      digitalWrite(LED_PIN, gearRelayState);
      lastToggleTime = now;
    }
  }
}
