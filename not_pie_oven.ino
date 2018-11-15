#include "max6675.h"

unsigned long previousMillis = 0;
const long interval = 1000;

int timer = 1;
int incomingByte = 0;

int mon = 0;
int heating = 0;
int powerI = 0;
int power = 4;
int temp = 0;
int duration = 0;

int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(2, OUTPUT);
  Help();
}

void loop() {

  if (Serial.available()) {

    incomingByte = Serial.read();

    if (incomingByte == 109) {
      if (mon == 0) {
        mon = 1;
      } else {
        Help();
        mon = 0;
      }
    }

    if (incomingByte == 112) {
      powerI = Serial.parseInt();
      if (powerI < 0 || powerI > 1) {
        Serial.println("Invalid Power State! ");
        Reset();
      } else {
        power = powerI;
      }
    }
    if (incomingByte == 116) {
      temp = Serial.parseInt();
      if (temp < 0 || temp > 300) {
        Serial.println("Invalid Temprature!");
        Reset();
      } else {
        Serial.print("Temprature set to ");
        Serial.print(temp);
        Serial.write(0xC2);
        Serial.write(0xB0);
        Serial.println("C");
        delay(500);
      }
    }

    if (incomingByte == 100) {
      duration = (Serial.parseInt()) * 60;
      if ((duration < 0 || duration > 21600) && duration != 30000) {
        Serial.println("Invalid Time!");
        Reset();
      } else if (duration == 30000) {
        Serial.println("UNLIMITED DURATION!!!!");
        delay(500);
      } else {
        Serial.print("Duration set to ");
        Serial.print(duration / 60);
        Serial.println(" minutes");
        delay(500);
      }
    }
  }

  if (mon == 1) {
    Temp();
    delay(100);
  }

  if (power == 1) {
    mon = 0;
    Serial.println("");
    Serial.println("Not Pie Oven Activate!");
    Serial.print("Heating to ");
    Serial.print(temp);
    Serial.write(0xC2);
    Serial.write(0xB0);
    Serial.print(" for ");
    if (duration == 30000) {
      Serial.println("UNLIMITED DURATION!!!!");
    } else {
      Serial.print(duration / 60);
      Serial.println(" minutes");
    }
    delay(1000);
    power = 2;
  }

  if (power == 2) {
    Serial.print("Not Pie Oven ");

    if ((thermocouple.readCelsius() + 1) < temp) {
      heating = 1;
    }

    if (thermocouple.readCelsius() >= temp) {
      heating = 0;
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(2, LOW);
    } else if (heating == 1) {
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(2, HIGH);
      Serial.print("heating");
    }
    if (heating == 0) {
      Serial.print("cooling");
    }

    Serial.print(" - Current temp:");
    Serial.print(thermocouple.readCelsius());
    Serial.write(0xC2);
    Serial.write(0xB0);
    Serial.print("C target ");
    Serial.print(temp);
    Serial.write(0xC2);
    Serial.write(0xB0);
    Serial.print("C - ");
    if (duration == 30000) {
      Serial.println("UNLIMITED DURATION!!!!");
    } else {
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        duration --;
      }
      Serial.print(duration / 60);
      Serial.print(".");
      if ((duration % 60) >= 10) {
        Serial.print(duration % 60);
      } else {
        Serial.print("0");
        Serial.print(duration % 60);
      }
      Serial.println(" minutes remaining");
    }
    delay(100);
  }

  if (duration != 30000 && (0 >= duration && power == 2)) {
    Serial.println("");
    Serial.println("      Timer Complete");
    power = 3;
  }

  if (power == 3 || power == 0) {
    Serial.println("Not Pie Oven Powering Down");
    Serial.println("      Enjoy Your PCB");
    delay(1000);
    Reset();
    power = 4;
  }

}

void Reset() {
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(2, LOW);
  power = 0;
  temp = 0;
  duration = 0;
  Help();
}

void Help() {
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("**************************");
  Serial.println("     Not Pie Oven 1.0");
  Serial.println("**************************");
  delay(500);
  Serial.println("");
  Serial.println("To set Temprature send \"tx\" where x is the temprature in degrees");
  Serial.println("To set Duration send \"dx\" where x is the time in minutes");
  Serial.println("To set power send \"px\" where x is 1 or 0 for on or off");
  Serial.println("Temprature limited from 0 to 300 degrees");
  Serial.println("Timer duration is liited to 360 minutes");
  Serial.println("Enter \"500\" in the duration feild for unlimited duration");
  Serial.println("Enter \"m\" to togle the temprature monitor when idle");
  Serial.println("");
  delay(500);
}

void Temp() {
  if (power == 4) {
    Serial.print("Not Pie Oven at:");
    Serial.print(thermocouple.readCelsius());
    Serial.write(0xC2);
    Serial.write(0xB0);
    Serial.println("");
  } else {
    mon = 0;
  }
}

