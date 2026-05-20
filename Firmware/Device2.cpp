// #include <Arduino.h>
// #include <ESP_LM35.h>
// #include <WiFi.h>
// #include <ArduinoJson.h>
// #include <HTTPClient.h>


// #define STATUS_LED_PIN 2

// void blinkLED();
// void setup_wifi();
// void recivedData_MEGA();
// void createDummyData(JsonDocument& doc);
// void sendBatchToInfluxDB(const JsonDocument& doc);
// void sendToInfluxDB(const String& measurement, const String& field, float value);
// void ReadAndSendMegaData();
// void debugPrint(const String& message);
// void debugPrintln(const String& message);
// void debugPrintf(const char* format, ...);
// void sendStaticDataToML_API();  
// void sendDataToML_API(const JsonDocument& doc);
// void sendDataTolocalserver();

// const char *token = "6mMMdGDHqx0A3SH_wRFx123onUzR_ZzocTCjOtO6Q7DogsaKiZr8Yzfucq9Lj4vadXpKMqknL2IiIGCqpNcQOw==";
// // const char *influxDBUrl = "http://192.168.40.120:8086/api/v2/write?org=VIKRAMA-SYNC&bucket=Vikarama_SYNC_ESS&precision=ns";
// // const char *localserverurl = "http://192.168.40.120:8000" ;
// const char *influxDBUrl = "http://192.168.71.120:8086/api/v2/write?org=VIKRAMA-SYNC&bucket=Vikarama_SYNC_ESS&precision=ns";
// const char *localserverurl = "http://192.168.71.120:5002" ;

// float cell1_voltage = 0.0;
// float cell2_voltage = 0.0;
// float cell3_voltage = 0.0;
// float total_voltage = 0.0;

// bool USE_DUMMY_DATA = false;  
// bool debug_flag = false;

// unsigned long lastDummyDataTime = 0;
// const unsigned long DUMMY_DATA_INTERVAL = 5000; 

// struct Config
// {
//   // WiFi settings
//   const char *ssid;
//   const char *password;
// };

// const Config config = {
//     "SNET",              // ssid
//     "12345678"
// };



// void setup() {
//     Serial.begin(9600); 
//     pinMode(STATUS_LED_PIN, OUTPUT);
//     setup_wifi(); 
//     debugPrintln("DATA OBJ FROM MEGA");
//     // recivedData_MEGA();
// }

// void loop() {
//   sendDataTolocalserver();
//   // ReadAndSendMegaData();
//   sendStaticDataToML_API();
//   delay(1000);
//     if (USE_DUMMY_DATA) {
//         unsigned long currentTime = millis();
//         if (currentTime - lastDummyDataTime >= DUMMY_DATA_INTERVAL) {
//             StaticJsonDocument<1024> doc;  
//             createDummyData(doc);
//             sendBatchToInfluxDB(doc);
//             lastDummyDataTime = currentTime;
//         }
//     } else {
//       //  ReadAndSendMegaData();
//       //  blinkLED();
//         sendDataTolocalserver();
//         ReadAndSendMegaData();
//         sendStaticDataToML_API();
//     }

// }

// void debugPrint(const String& message) {
//     if (debug_flag) {
//         Serial.print(message);
//     }
// }

// void debugPrintln(const String& message) {
//     if (debug_flag) {
//         Serial.println(message);
//     }
// }

// void debugPrintf(const char* format, ...) {
//     if (debug_flag) {
//         char buffer[256];
//         va_list args;
//         va_start(args, format);
//         vsnprintf(buffer, sizeof(buffer), format, args);
//         va_end(args);
//         Serial.print(buffer);
//     }
// }

// void blinkLED() {
//   static bool state = false;
//   digitalWrite(STATUS_LED_PIN, state ? HIGH : LOW);
//   state = !state;
// }

// void setup_wifi() {
//     delay(10);
//     WiFi.begin(config.ssid, config.password);
//     unsigned long startAttemptTime = millis();

//     while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 20000) {
//         blinkLED();
//         delay(100);
//     }
//     if (WiFi.status() != WL_CONNECTED) {
//         debugPrintln("Failed to connect to WiFi. Restarting...");
//         ESP.restart();
//     }
    
//     digitalWrite(STATUS_LED_PIN, HIGH);
//     debugPrintln("WiFi connected");
// }
// void sendBatchToInfluxDB(const JsonDocument& doc) {
//     if (WiFi.status() != WL_CONNECTED) {
//         debugPrintln("WiFi not connected. Attempting reconnection...");
//         setup_wifi();
//         return;
//     }

//     HTTPClient http;
//     http.begin(influxDBUrl);
//     http.addHeader("Content-Type", "text/plain");
//     http.addHeader("Authorization", String("Token ") + token);

//     String payload = "";
    
//     // Process each group
//     for (int group = 1; group <= 3; group++) {
//         String groupStr = "group" + String(group);
//         if (doc.containsKey(groupStr)) {
//             // Add group voltage measurement
//             if (doc[groupStr].containsKey("voltage")) {
//                 payload += "group_metrics,group=" + String(group) + " ";
//                 payload += "voltage=" + String(doc[groupStr]["voltage"].as<float>()) + "\n";
//             }
            
//             // Process cells
//             for (int cell = 1; cell <= 3; cell++) {
//                 String cellStr = "cell" + String(cell);
//                 if (doc[groupStr].containsKey(cellStr)) {
//                     payload += "battery_metricstest,group=" + String(group) + ",cell=" + String(cell) + " ";
//                     payload += "current=" + String(doc[groupStr][cellStr]["current"].as<float>()) + ",";
//                     payload += "temperature=" + String(doc[groupStr][cellStr]["temperature"].as<float>()) + ",";
//                     payload += "voltage=" + String(doc[groupStr][cellStr]["voltage"].as<float>()) + ",";
//                     payload += "status=" + String(doc[groupStr][cellStr]["status"].as<int>());
                    
//                     if (doc[groupStr].containsKey("connect_state")) {
//                         payload += ",connect_state=" + String(doc[groupStr]["connect_state"].as<bool>());
//                     }
                    
//                     payload += "\n";
//                 }
//             }
//         }
//     }

//     // Debug print payload
//     debugPrintln("Sending payload to InfluxDB:");
//     debugPrintln(payload);

//     if (payload.length() > 0) {
//         int httpResponseCode = http.POST(payload);
//         if (httpResponseCode > 0) {
//             debugPrintf("HTTP Response code: %d\n", httpResponseCode);
//         } else {
//             debugPrintf("Error code: %d\n", httpResponseCode);
//             debugPrintln(http.errorToString(httpResponseCode));
//         }
//     }
    
//     http.end();
// }



// void recivedData_MEGA() {
//   if (Serial.available() > 0) {
//     String jsonData = Serial.readStringUntil('\n');
//     StaticJsonDocument<1024> doc;
//     serializeJsonPretty(doc, Serial);
//   }
// }

// void sendToInfluxDB(const String& measurement, const String& field, float value) {
//     if (WiFi.status() == WL_CONNECTED) {
//         HTTPClient http;
//         String data = measurement + " " + field + " " + String(value);
//         http.begin(influxDBUrl);
//         http.addHeader("Content-Type", "text/plain");
//         int httpResponseCode = http.POST(data);
//         if (httpResponseCode > 0) {
//         } else {
//         }
//         http.end();
//     }
// }

// void createDummyData(JsonDocument& doc) {
//     for (int group = 1; group <= 3; group++) {
//         String groupStr = "group" + String(group);
//         float groupVoltage = 0; 
        
//         for (int cell = 1; cell <= 3; cell++) {
//             String cellStr = "cell" + String(cell);
//             float cellVoltage = random(360, 420) / 100.0;      
//             doc[groupStr][cellStr]["voltage"] = cellVoltage;
//             doc[groupStr][cellStr]["current"] = random(100, 150) / 100.0;      
//             doc[groupStr][cellStr]["temperature"] = random(250, 300) / 10.0;   
//             doc[groupStr][cellStr]["status"] = random(1, 4);
//             doc[groupStr]["connect_state"] = random(0, 2) == 1;
//             groupVoltage += cellVoltage;
//         }

//         doc[groupStr]["voltage"] = groupVoltage + random(-10, 10) / 100.0;
//     }
// }

// void ReadAndSendMegaData() {
//     if (!Serial.available()) {
//         return;
//     }
//     String jsonData = Serial.readStringUntil('\n');
//     debugPrintln("Received from Mega: ");
//     debugPrintln(jsonData);
//     StaticJsonDocument<4096> doc;
//     DeserializationError error = deserializeJson(doc, jsonData);
//     if (error) {
//         debugPrint("JSON Parse Error: ");
//         debugPrintln(error.c_str());
//         digitalWrite(STATUS_LED_PIN, LOW);
//         return;
//     }

//     if (WiFi.status() != WL_CONNECTED) {
//         debugPrintln("WiFi disconnected, attempting reconnect...");
//         setup_wifi();
//         return;
//     }


//     HTTPClient http;
//     http.begin("http://192.168.71.120:5000/predict");
//     http.addHeader("Content-Type", "application/json");


//     int httpResponseCode = http.POST(jsonData);

//     if (httpResponseCode > 0) {
//         String response = http.getString();
//         debugPrintf("HTTP Response code: %d\n", httpResponseCode);
//         debugPrintln("Server response: " + response);
//         digitalWrite(STATUS_LED_PIN, HIGH);
//     } else {
//         debugPrintf("Error sending: %d\n", httpResponseCode);
//         digitalWrite(STATUS_LED_PIN, LOW);
//     }
//     http.end();
//     delay(100);
//     }


// void sendDataToML_API(const JsonDocument& doc) {
//     debugPrintln("Starting ML API request...");

//     if (WiFi.status() != WL_CONNECTED) {
//         debugPrintln("WiFi not connected!");
//         return;
//     }

//     HTTPClient http;
//     http.begin("http://192.168.40.120:5001/predict");
//     http.addHeader("Content-Type", "application/json");
    
//     String jsonString;
//     serializeJson(doc, jsonString);
    
//     debugPrintln("Request payload:");
//     debugPrintln(jsonString);
    
//     int httpResponseCode = http.POST(jsonString);
    
//     if (httpResponseCode > 0) {
//         String response = http.getString();
//         debugPrintln("Raw ML Response: ");
//         debugPrintln(response);
        
//         // Check if response is empty
//         if (response.length() == 0) {
//             debugPrintln("Empty response received");
//             http.end();
//             return;
//         }

//         // Parse ML response with error details
//         StaticJsonDocument<512> mlResponse;
//         DeserializationError mlError = deserializeJson(mlResponse, response);
        
//         if (!mlError) {
//             // Validate required fields exist
//             if (!mlResponse.containsKey("backup_cells_needed") ||
//                 !mlResponse.containsKey("fault_probability") ||
//                 !mlResponse.containsKey("remaining_life_hours") ||
//                 !mlResponse.containsKey("status")) {
//                 debugPrintln("Missing required fields in response");
//                 http.end();
//                 return;
//             }

//             // Send to InfluxDB
//             HTTPClient influxHttp;
//             influxHttp.begin(influxDBUrl);
//             influxHttp.addHeader("Content-Type", "text/plain");
//             influxHttp.addHeader("Authorization", String("Token ") + token);
            
//             String influxPayload = "ml_predictions ";
//             influxPayload += "backup_cells=" + String(mlResponse["backup_cells_needed"].as<int>()) + ",";
//             influxPayload += "fault_probability=" + String(mlResponse["fault_probability"].as<float>()) + ",";
//             influxPayload += "remaining_life=" + String(mlResponse["remaining_life_hours"].as<float>()) + ",";
//             influxPayload += "status=\"" + String(mlResponse["status"].as<const char*>()) + "\"";
            
//             debugPrintln("InfluxDB payload: " + influxPayload);
            
//             int influxResponse = influxHttp.POST(influxPayload);
//             if (influxResponse > 0) {
//                 debugPrintln("ML predictions stored in InfluxDB");
//             } else {
//                 debugPrintln("Failed to store ML predictions: " + String(influxResponse));
//             }
//             influxHttp.end();
//         } else {
//             debugPrintln("Parse error: " + String(mlError.c_str()));
//             debugPrintln("Response was: " + response);
//         }
//     } else {
//         debugPrintln("Error from ML API: " + http.errorToString(httpResponseCode));
//     }
//     http.end();
// }

// // void sendStaticDataToML_API() {
// //     const char* jsonData = R"({
// //         "group1": {
// //             "voltage": 0,
// //             "cell1": {"current": 0, "temperature": 61.03516, "voltage": 0, "status": 3},
// //             "cell2": {"current": 0, "temperature": 61.03516, "voltage": 0, "status": 3},
// //             "cell3": {"current": 0, "temperature": 61.03516, "voltage": 0, "status": 3},
// //             "connect_state": false
// //         },
// //         "group2": {
// //             "voltage": 0,
// //             "cell1": {"current": 0, "temperature": 83.49609, "voltage": 0, "status": 3},
// //             "cell2": {"current": 0, "temperature": 83.49609, "voltage": 0, "status": 3},
// //             "cell3": {"current": 0, "temperature": 83.49609, "voltage": 0, "status": 3},
// //             "connect_state": false
// //         },
// //         "group3": {
// //             "voltage": 0,
// //             "cell1": {"current": 0, "temperature": 69.33594, "voltage": 0, "status": 3},
// //             "cell2": {"current": 0, "temperature": 69.33594, "voltage": 0, "status": 3},
// //             "cell3": {"current": 0, "temperature": 69.33594, "voltage": 0, "status": 3},
// //             "connect_state": false
// //         }
// //     })";

// // StaticJsonDocument<1024> doc;
// //     DeserializationError error = deserializeJson(doc, jsonData);
// //     if (error) {
// //         debugPrintln("JSON deserialization failed");
// //         return;
// //     }

// //     // Add fields and send to ML API
// //     doc["voltage_trend"] = -0.001;
// //     doc["time_span"] = 100;

// //     // Send to ML API and get response
// //     HTTPClient http;
// //     http.begin("http://192.168.40.120:5001/predict");
// //     http.addHeader("Content-Type", "application/json");
    
// //     String jsonString;
// //     serializeJson(doc, jsonString);
    
// //     int httpResponseCode = http.POST(jsonString);
    
// //     if (httpResponseCode > 0) {
// //         String response = http.getString();
// //         debugPrintln("ML Response: " + response);
        
// //         // Parse ML response
// //         StaticJsonDocument<512> mlResponse;
// //         DeserializationError mlError = deserializeJson(mlResponse, response);
        
// //         if (!mlError) {
// //             // Send ML predictions to InfluxDB
// //             HTTPClient influxHttp;
// //             influxHttp.begin(influxDBUrl);
// //             influxHttp.addHeader("Content-Type", "text/plain");
// //             influxHttp.addHeader("Authorization", String("Token ") + token);
            
// //             // Format prediction data for InfluxDB
// //             String influxPayload = "ml_predictions ";
// //             influxPayload += "backup_cells=" + String(mlResponse["backup_cells_needed"].as<int>()) + ",";
// //             influxPayload += "fault_probability=" + String(mlResponse["fault_probability"].as<float>()) + ",";
// //             influxPayload += "remaining_life=" + String(mlResponse["remaining_life_hours"].as<float>()) + ",";
// //             influxPayload += "status=\"" + String(mlResponse["status"].as<const char*>()) + "\"";
            
// //             int influxResponse = influxHttp.POST(influxPayload);
// //             if (influxResponse > 0) {
// //                 debugPrintln("ML predictions stored in InfluxDB");
// //             } else {
// //                 debugPrintln("Failed to store ML predictions: " + String(influxResponse));
// //             }
// //             influxHttp.end();
// //         } else {
// //             debugPrintln("Failed to parse ML response");
// //         }
// //     } else {
// //         debugPrintln("Error from ML API: " + http.errorToString(httpResponseCode));
// //     }
// //     http.end();
// // }

// void sendStaticDataToML_API() {
//     const char* jsonData = R"({
//         "group1": {
//             "voltage": 0,
//             "cell1": {"current": 0, "temperature": 61.03516, "voltage": 0, "status": 3},
//             "cell2": {"current": 0, "temperature": 61.03516, "voltage": 0, "status": 3},
//             "cell3": {"current": 0, "temperature": 61.03516, "voltage": 0, "status": 3},
//             "connect_state": false
//         },
//         "group2": {
//             "voltage": 0,
//             "cell1": {"current": 0, "temperature": 83.49609, "voltage": 0, "status": 3},
//             "cell2": {"current": 0, "temperature": 83.49609, "voltage": 0, "status": 3},
//             "cell3": {"current": 0, "temperature": 83.49609, "voltage": 0, "status": 3},
//             "connect_state": false
//         },
//         "group3": {
//             "voltage": 0,
//             "cell1": {"current": 0, "temperature": 69.33594, "voltage": 0, "status": 3},
//             "cell2": {"current": 0, "temperature": 69.33594, "voltage": 0, "status": 3},
//             "cell3": {"current": 0, "temperature": 69.33594, "voltage": 0, "status": 3},
//             "connect_state": false
//         }
//     })";

//     // Deserialize initial data
//     StaticJsonDocument<1024> doc;
//     DeserializationError error = deserializeJson(doc, jsonData);
//     if (error) {
//         debugPrintln("JSON deserialization failed");
//         return;
//     }

//     // Add fields for ML API
//     doc["voltage_trend"] = -0.001;
//     doc["time_span"] = 100;

//     // Send to ML API
//     HTTPClient http;
//     http.begin("http://192.168.71.120:5001/predict");
//     http.addHeader("Content-Type", "application/json");
    
//     String jsonString;
//     serializeJson(doc, jsonString);
//     int httpResponseCode = http.POST(jsonString);
    
//     if (httpResponseCode > 0) {
//         String response = http.getString();
//         debugPrintln("ML Response: " + response);
        
//         // Parse ML API response
//         StaticJsonDocument<2048> mlResponse;
//         DeserializationError mlError = deserializeJson(mlResponse, response);
        
//         if (!mlError) {
//             const JsonArray predictions = mlResponse["predictions"];
//             for (JsonObject prediction : predictions) {
//                 int groupId = prediction["group_id"];
//                 bool connectState = prediction["connect_state"];
//                 float groupVoltage = prediction["group_voltage"];
//                 JsonObject predDetails = prediction["prediction"];
                
//                 int backupCells = predDetails["backup_cells_needed"];
//                 float faultProbability = predDetails["fault_probability"];
//                 float remainingLife = predDetails["remaining_life_hours"];
//                 const char* status = predDetails["status"];
                
//                 // Send data to InfluxDB
//                 HTTPClient influxHttp;
//                 influxHttp.begin(influxDBUrl);
//                 influxHttp.addHeader("Content-Type", "text/plain");
//                 influxHttp.addHeader("Authorization", String("Token ") + token);
                
//                 String influxPayload = "ml_predictions,group_id=" + String(groupId);
//                 influxPayload += " connect_state=" + String(connectState ? "true" : "false") + ",";
//                 influxPayload += "group_voltage=" + String(groupVoltage) + ",";
//                 influxPayload += "backup_cells_needed=" + String(backupCells) + ",";
//                 influxPayload += "fault_probability=" + String(faultProbability) + ",";
//                 influxPayload += "remaining_life_hours=" + String(remainingLife) + ",";
//                 influxPayload += "status=\"" + String(status) + "\"";
                
//                 int influxResponse = influxHttp.POST(influxPayload);
//                 if (influxResponse > 0) {
//                     debugPrintln("Data for Group " + String(groupId) + " stored in InfluxDB");
//                 } else {
//                     debugPrintln("Failed to store Group " + String(groupId) + " data: " + String(influxResponse));
//                 }
//                 influxHttp.end();
//             }
//         } else {
//             debugPrintln("Failed to parse ML response");
//         }
//     } else {
//         debugPrintln("Error from ML API: " + http.errorToString(httpResponseCode));
//     }
//     http.end();
// }


// void sendDataTolocalserver(){
//   if(Serial.available() > 0){
//     String jsonData = Serial.readStringUntil('\n');
//     // StaticJsonDocument<1024> doc;
//     // serializeJson(doc, Serial);
//     HTTPClient http;
//     http.begin(localserverurl);
//     http.addHeader("Content-Type", "application/json");
//     // int httpResponseCode = http.POST(jsonData);
//     http.POST(jsonData);  
//     // if(httpResponseCode > 0){
//     //   String response = http.getString();
//     //   debugPrintln("Server response: " + response);

//     // }else{
//     //   debugPrintln("Error code: " + httpResponseCode);
//     //   debugPrintln(http.errorToString(httpResponseCode));
//     // }
//     http.end();
//   }
// }

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>


const char* ssid = "SNET";
const char* password = "12345678";

// Server details
const char* serverUrl = "http://192.168.71.120:5002";

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
}

void sendData(const String& jsonString) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverUrl);
        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.POST(jsonString);
        http.end();
    }
}

void loop() {
  digitalWrite(2, LOW);
    // Serial.println("Data sent to server");

  if (Serial.available() > 0) {
    StaticJsonDocument<2048> doc;
    String jsonData = Serial.readStringUntil('\n');
    Serial.println(jsonData);
    // sendData(jsonData);
    // DeserializationError error = deserializeJson(doc, jsonData);
    // if (error) {
    //   return;
    // }
    // digitalWrite(2, HIGH);
    // serializeJsonPretty(doc, Serial);
    // Serial.println(jsonData);
    // Serial.println("Data sent to server");
  }
}