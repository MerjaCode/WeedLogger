/*
 * T3chFlicks - Smart Buoy - Step 2
 * GPS basic example
 * Gets longitude and latitude, date, and time.
 * gps tx -> d4
 * gps rx -> d3
 */

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SdFat.h>

void printResults();
void writeResults();
void getInfo();

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(4, 3);

SdFat SD;
File logfile;

bool isValidEntry;

/* OLD const char swtchPins[9] = {5, 7, 8, 9, A0, A1, A2, A3, A4};
const char* headers = "LAT,LONG,DATE,ZTIME,Cheat,Rock,CT,Gopher-Rut-Saw,Hoat,Rye-GG,Moat,K-PL-RK,MG"; */
const char swtchPins[9] = {5, 7, 8, 9, A0, A1, A2, A3, A4};
const char* headers = "LAT,LONG,DATE,ZTIME,Cheat,Rock,CT,Gopher-Rut-Saw,Hoat,Rye-GG,Moat,K-PL-RK,MG";
// char *gpsHeader = "";
struct dataStruct
{
  double latitude;
  double longitude;
  unsigned long date;
  unsigned long time;
} gpsData;

void setup()
{
  Serial.begin(115200);
  ss.begin(9600);
  Serial.println(headers); // print headers

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1500);

  for (char pin : swtchPins)
  {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    pinMode(pin, INPUT);
  }

  while (!SD.begin())
  { // if you're using an Nano, you can use this line instead
    Serial.println(F("Card init failed!"));
    delay(1000);
    // error(2);
  }

  Serial.println(F("Card init success!"));

  char filename[12];
  strcpy(filename, "GPSLOG00.CSV");
  for (uint8_t i = 0; i < 100; i++)
  {
    filename[6] = '0' + i / 10;
    filename[7] = '0' + i % 10;
    Serial.print(F("Trying: "));
    Serial.println(filename);
    delay(100);

    // create if does not exist, do not open existing, write, sync after write
    if (!SD.exists(filename))
    {
      Serial.print(F("Writing to "));
      Serial.println(filename);
      break;
    }
  }

  logfile = SD.open(filename, FILE_WRITE);
  if (!SD.exists(filename))
  {
    Serial.print(F("Could not create "));
    Serial.println(filename);
  }

  logfile.println(headers); // print headers
}

void loop()
{
  while (ss.available() > 0)
  {
    if (gps.encode(ss.read()))
    {
      getInfo();
      printResults();
      writeResults();
      delay(500);
    }
  }
}

void getInfo()
{
  if (gps.location.isValid())
  {
    isValidEntry = true;
    gpsData.latitude = gps.location.lat();
    gpsData.longitude = gps.location.lng();
  }
  else
  {
    isValidEntry = false;
    // Serial.println("Invalid location");
  }
  if (gps.date.isValid())
  {
    gpsData.date = gps.date.value();
  }
  else
  {
    // isValidEntry = false; // don't really need because we really only want the location
    // Serial.println("Invalid date");
  }
  if (gps.time.isValid())
  {
    gpsData.time = gps.time.value();
  }
  else
  {
    // isValidEntry = false; // don't really need because we really only want the location
    Serial.println("Invalid time");
  }
}

void printResults()
{
  //  Serial.print("Location: ");

  if (isValidEntry)
  {
    // digitalWrite(LED_BUILTIN, HIGH);

    Serial.print(gpsData.latitude, 8);
    Serial.print(", ");
    Serial.print(gpsData.longitude, 8);
    //  Serial.print("  Date: ");
    Serial.print(",");
    Serial.print(gpsData.date);
    //  Serial.print("  Time: ");
    Serial.print(",");
    Serial.print(gpsData.time);
    Serial.println();
  }
  else
  {
    // digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Invalid Entry!");
  }

  for (int pin : swtchPins)
  { // was 10
    int digPin = digitalRead(pin);
    // Serial.print("(");
    // Serial.print(pin);
    // Serial.print(")");
    Serial.print(digPin);
    Serial.print(", ");
  }
  Serial.println();
}

void writeResults()
{

  logfile.print(gpsData.latitude, 8);
  logfile.print(",");
  logfile.print(gpsData.longitude, 8);
  logfile.print(",");
  logfile.print(gpsData.date);
  logfile.print(",");
  logfile.print(gpsData.time);

  for (int pin : swtchPins)
  { // was 10
    int digPin = digitalRead(pin);
    logfile.print(",");
    logfile.print(digPin);
  }
  logfile.println();
  logfile.flush();
}
