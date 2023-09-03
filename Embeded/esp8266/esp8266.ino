#include <Wire.h>

#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"

#include <InfluxDbClient.h>

#include <InfluxDbCloud.h>


 #define INFLUXDB_URL "http://192.168.254.60:8086"

#define INFLUXDB_TOKEN "vHGp8PcbtpTQVmlplzQMecvZdfpSJcUNT1wOvBXXRyOzeWmW0AsOQMWMRijSoEKuwxlURtrhael_rAD9qfPUMw=="
#define INFLUXDB_ORG "ad3830e63761906d"
#define INFLUXDB_BUCKET "nikolaus"
  
#define TZ_INFO "UTC-4"

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

Point sensor("sensor_status");

#define BUFLEN 14

#define TRASHCAN_HEIGHT 140

char buffer[BUFLEN];

float distance_old;

#define WIFI_SSID "AC-Guest"
#define WIFI_PASSWORD ""

void setup() {
  Serial.begin(115200); /* begin serial for debug */
  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
  Serial.print("Hello");

  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  sensor.addTag("device", DEVICE);

}

void loop() {
  int i;

  sensor.clearFields();
  Wire.beginTransmission(8); /* begin with device address 8 */
  Wire.write("Hello Arduino"); /* sends hello string */
  Wire.endTransmission(); /* stop transmitting */

  Serial.println("## Reading data from arduino\n");
  Wire.requestFrom(8, 13); /* request & read data of size 13 from slave */
  for (i = 0; i < BUFLEN - 1; i++) {
    char c = Wire.read(); // receive byte as a character
    buffer[i] = c; 
  }
  buffer[i] = '\0'; // assume it is a string, add zero-terminator
  Serial.print("received data:");
  Serial.print(buffer);
  Serial.println();

  Serial.print("## Convert distance to int\n");
  int distance_new = atoi(buffer);
  Serial.print("Distance as int: ");
  Serial.print(distance_new);
  Serial.println();


  Serial.print("## Average distance\n");
  float distance = (distance_old + distance_new) / 2;
  Serial.print("Computed distance: ");
  Serial.print(distance_old);
  Serial.println();
  Serial.print(distance_new);
  Serial.println();
  Serial.print(distance);
  Serial.println();

  distance_old = distance;

  Serial.print("## Convert to percent\n");
  float percent = max( ((TRASHCAN_HEIGHT-distance))/ TRASHCAN_HEIGHT  , (float)0);
  Serial.print("Percent: ");
  Serial.print(percent);
  Serial.println();

  Serial.print("## Writing to influxdb\n");

  sensor.addField("trashcanID", 1);
  sensor.addField("distance", distance);
  sensor.addField("percent", percent);
  sensor.addField("Latitude", 43.477371);
  sensor.addField("Longitude", -80.549118);

  Serial.print("Writing: ");
  Serial.println(sensor.toLineProtocol());

  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  Serial.println("## Waiting 2 second");
  delay(2000);

}
