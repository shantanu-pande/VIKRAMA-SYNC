#include <Arduino.h>
#include <ArduinoJson.h>

#define VOLTAGE_REFERENCE 5.0    // Operating voltage for Arduino
#define ADC_RESOLUTION 1024.0   // 10-bit ADC for Arduino
#define SENSITIVITY 0.185    // V/A for ACS712 30A version
#define SAMPLES 10              // Number of samples for averaging
#define ACS_OFFSET 512        // Adjust this value to get zero at no current

#define CELL1_MAX_VOLTAGE 3.9   // Max voltage for cell 1
#define CELL2_MAX_VOLTAGE 7.8   // Max voltage for cell 2
#define CELL3_MAX_VOLTAGE 11.7  // Max voltage for cell 3
#define POT_MAX_VOLTAGE 3.5     // Max voltage after potentiometer

#define SCALE_FACTOR_1 (CELL1_MAX_VOLTAGE / POT_MAX_VOLTAGE)
#define SCALE_FACTOR_2 (CELL2_MAX_VOLTAGE / POT_MAX_VOLTAGE)
#define SCALE_FACTOR_3 (CELL3_MAX_VOLTAGE / POT_MAX_VOLTAGE)


float read_current_sensor(int pin);
float read_temperature_sensor(int pin);
void update_voltage(int battery_index, float &cell1_voltage, float &cell2_voltage, float &cell3_voltage);

// Global Variables
// pin def of relay
const int RELAY[4] = {2, 3, 4, 5}; 

// Pin definitions for each battery
const int battery_pins[3][5] = {
  {A0, A1, A2, A3, A4}, // Battery 1: Voltage1, Voltage2, Voltage3, Current, Temperature
  {A5, A6, A7, A8, A9}, // Battery 2: Voltage1, Voltage2, Voltage3, Current, Temperature
  {A10, A11, A12, A13, A14} // Battery 3: Voltage1, Voltage2, Voltage3, Current, Temperature
};
float cell1_voltage[3] = {0.0, 0.0, 0.0};
float cell2_voltage[3] = {0.0, 0.0, 0.0};
float cell3_voltage[3] = {0.0, 0.0, 0.0};
float temperature[3] = {0.0, 0.0, 0.0};
float current[3] = {0.0, 0.0, 0.0};


void setup() {
  Serial.begin(115200);
  // Serial2.begin(115200);


  for (int i = 0; i < 4; i++) {
    pinMode(RELAY[i], OUTPUT);
    digitalWrite(RELAY[i], HIGH);
    for (int j = 0; j < 5; j++) {
      pinMode(battery_pins[i][j], INPUT);
    }
  }
}

void loop() {
  StaticJsonDocument<2048> doc;

  for (int i = 0; i < 3; i++) {
    update_voltage(i, cell1_voltage[i], cell2_voltage[i], cell3_voltage[i]);
    // temperature[i] = 25;
    temperature[i] = read_temperature_sensor(battery_pins[i][4]);
    // current[i] = 0;
    current[i] = read_current_sensor(battery_pins[i][3]);

    String group = "group" + String(i + 1);
    JsonObject groupObj = doc.createNestedObject(group);
    groupObj["voltage"] = cell3_voltage[i];

    JsonObject cell1 = groupObj.createNestedObject("cell1");
    cell1["current"] = current[i];
    cell1["temperature"] = temperature[i];
    cell1["voltage"] = cell1_voltage[i];
    
    uint8_t status_1;
    if ((cell1_voltage[i] > 3.7)&&(cell1_voltage[i] < 4.2)) {
      status_1 = 0;
    } 
    else if(cell1_voltage[i] <= 0){
    status_1 = 2;
    }
    else {
      status_1 = 1;
    }

    if(temperature[i] > 50){
      status_1 = 3;
    }
    cell1["status"] = status_1;

    JsonObject cell2 = groupObj.createNestedObject("cell2");
    cell2["current"] = current[i];
    cell2["temperature"] = temperature[i];

    uint8_t status_2;
    if (cell2_voltage[i] <= cell1_voltage[i])
    {
      status_2 = 2;
      cell2["voltage"] = 0;
    }
    else if ((cell2_voltage[i] > 7.4)&&(cell2_voltage[i] < 8.4)) {
      status_2 = 0;
      cell2["voltage"] = cell2_voltage[i]-cell1_voltage[i];
    }
    else {
      status_2 = 1;
      cell2["voltage"] = cell2_voltage[i]-cell1_voltage[i];
    }

    if(temperature[i] > 50){
      status_2 = 3;
    }
    cell2["status"] = status_2;

    JsonObject cell3 = groupObj.createNestedObject("cell3");
    cell3["current"] = current[i];
    cell3["temperature"] = temperature[i];
    
    uint8_t status_3;
    if (cell3_voltage[i] <= cell2_voltage[i])
    {
      status_3 = 2;
      cell3["voltage"] = 0;
    }
    else if ((cell3_voltage[i] > 11)&&(cell3_voltage[i] < 12.6)) {
      status_3 = 0;
      cell3["voltage"] = cell3_voltage[i]-cell2_voltage[i];
    }
    else {
      status_3 = 1;
      cell3["voltage"] = cell3_voltage[i]-cell2_voltage[i];
    }
    
    if(temperature[i] > 50){
      status_3 = 3;
    }

    cell3["status"] = status_3;
    bool CONN = (!status_1 && !status_2 && !status_3)?1:0;
    groupObj["connect_state"] = CONN;
    
    if (CONN) {
      digitalWrite(RELAY[i], HIGH);
    } else {
      digitalWrite(RELAY[i], LOW);
    }
  }


  serializeJson(doc, Serial);
  Serial.println();

  delay(1000);
}

void update_voltage(int battery_index, float &cell1_voltage, float &cell2_voltage, float &cell3_voltage) {
  cell1_voltage = (analogRead(battery_pins[battery_index][0]) * VOLTAGE_REFERENCE / ADC_RESOLUTION) * SCALE_FACTOR_1;
  cell2_voltage = (analogRead(battery_pins[battery_index][1]) * VOLTAGE_REFERENCE / ADC_RESOLUTION) * SCALE_FACTOR_2;
  cell3_voltage = (analogRead(battery_pins[battery_index][2]) * VOLTAGE_REFERENCE / ADC_RESOLUTION) * SCALE_FACTOR_3;
}

float read_current_sensor(int pin) {
  float sum = 0;
  for (int i = 0; i < SAMPLES; i++) {
    int sensorValue = analogRead(pin);
    sum += sensorValue;
    delayMicroseconds(100);
  }
  float avgValue = sum / SAMPLES;
  float voltage = (avgValue - ACS_OFFSET) * (VOLTAGE_REFERENCE / ADC_RESOLUTION);
  float current = voltage / SENSITIVITY;
  return -current;
}

float read_temperature_sensor(int pin) {
  int rawValue = analogRead(pin);
  float voltage = (rawValue * VOLTAGE_REFERENCE) / ADC_RESOLUTION;
  float temperature = (voltage * 100.0); // LM35 scale is 10mV/°C
  return temperature;
}