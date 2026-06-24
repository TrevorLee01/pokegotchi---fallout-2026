#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <Preferences.h>

// Libraries used:
// Arduino.h: normal Arduino functions, pins, millis, tone, Serial
// Wire.h: I2C bus for the OLED and DS3231 RTC
// Adafruit_GFX.h: graphics base library for the OLED
// Adafruit_SSD1306.h: 128x64 OLED driver
// RTClib.h: DS3231 real-time clock driver
// Preferences.h: ESP32 flash storage for saved pet state

const byte GOTCHI_SDA_PIN = 11;
const byte GOTCHI_SCL_PIN = 12;
const byte GOTCHI_SW1_PIN = 4;
const byte GOTCHI_SW2_PIN = 5;
const byte GOTCHI_SW3_PIN = 6;
const byte GOTCHI_BUZZER_PIN = 7;
const byte GOTCHI_LDR_PIN = 1;

const byte GOTCHI_OLED_WIDTH = 128;
const byte GOTCHI_OLED_HEIGHT = 64;
const byte GOTCHI_OLED_ADDR = 0x3C;

const byte GOTCHI_STAT_MAX = 100;
const byte GOTCHI_FRIENDSHIP_EVOLVE = 80;
const byte GOTCHI_SYLVEON_FRIENDSHIP = 95;
const int GOTCHI_BRIGHT_LIGHT = 2800;
const int GOTCHI_DARK_LIGHT = 900;
const byte GOTCHI_DAY_START = 6;
const byte GOTCHI_NIGHT_START = 20;

const unsigned long GOTCHI_TICK_MS = 1000;
const unsigned long GOTCHI_SAVE_MS = 30000;

Adafruit_SSD1306 display(GOTCHI_OLED_WIDTH, GOTCHI_OLED_HEIGHT, &Wire, -1);
RTC_DS3231 rtc;
Preferences prefs;

enum Species {
  GOTCHI_NONE,
  GOTCHI_EEVEE,
  GOTCHI_SNORLAX,
  GOTCHI_SOLOSIS,
  GOTCHI_UMBREON,
  GOTCHI_LEAFEON,
  GOTCHI_SYLVEON,
  GOTCHI_GLACEON,
  GOTCHI_ESPEON
};

enum Screen {
  GOTCHI_STARTER_SCREEN,
  GOTCHI_HOME_SCREEN,
  GOTCHI_MENU_SCREEN,
  GOTCHI_STATS_SCREEN,
  GOTCHI_MESSAGE_SCREEN
};

struct PetState {
  uint32_t magic = 0x504F4B45;  // POKE
  uint16_t version = 1;
  Species species = GOTCHI_NONE;
  uint32_t createdUnix = 0;
  uint32_t lastUpdateUnix = 0;
  uint32_t careCount = 0;
  uint32_t playCount = 0;
  byte hunger = 70;
  byte happiness = 65;
  byte energy = 80;
  byte friendship = 50;
  bool asleep = false;
  bool evolved = false;
};

struct ButtonState {
  byte pin;
  bool stable = HIGH;
  bool lastRead = HIGH;
  unsigned long changedAt = 0;
};

PetState pet;
ButtonState sw1 = {GOTCHI_SW1_PIN};
ButtonState sw2 = {GOTCHI_SW2_PIN};
ButtonState sw3 = {GOTCHI_SW3_PIN};

Screen screen = GOTCHI_STARTER_SCREEN;
byte starterChoice = 0;
byte menuChoice = 0;
bool rtcReady = false;
unsigned long lastTick = 0;
unsigned long lastSave = 0;
unsigned long messageUntil = 0;
char messageText[22] = "";

byte clampStat(int value) {
  if (value < 0) return 0;
  if (value > GOTCHI_STAT_MAX) return GOTCHI_STAT_MAX;
  return value;
}

byte changeStat(byte value, int amount) {
  return clampStat((int)value + amount);
}

bool isDay(byte hour) {
  return hour >= GOTCHI_DAY_START && hour < GOTCHI_NIGHT_START;
}

bool isNight(byte hour) {
  return !isDay(hour);
}

const char* speciesName(Species species) {
  switch (species) {
    case GOTCHI_EEVEE: return "Eevee";
    case GOTCHI_SNORLAX: return "Snorlax";
    case GOTCHI_SOLOSIS: return "Solosis";
    case GOTCHI_UMBREON: return "Umbreon";
    case GOTCHI_LEAFEON: return "Leafeon";
    case GOTCHI_SYLVEON: return "Sylveon";
    case GOTCHI_GLACEON: return "Glaceon";
    case GOTCHI_ESPEON: return "Espeon";
    default: return "No Pokemon";
  }
}

uint32_t nowUnix() {
  if (rtcReady) return rtc.now().unixtime();
  return millis() / 1000;
}

byte currentHour() {
  if (rtcReady) return rtc.now().hour();
  return (millis() / 3600000UL) % 24;
}

byte currentMinute() {
  if (rtcReady) return rtc.now().minute();
  return (millis() / 60000UL) % 60;
}

int lightRaw() {
  return analogRead(GOTCHI_LDR_PIN);
}

bool petExists() {
  return pet.species != GOTCHI_NONE;
}

void beep(int freq = 1600, int ms = 40) {
  tone(GOTCHI_BUZZER_PIN, freq, ms);
}

bool pressed(ButtonState& button) {
  const unsigned long gotchiDebounceMs = 35;
  bool reading = digitalRead(button.pin);
  unsigned long now = millis();

  if (reading != button.lastRead) {
    button.lastRead = reading;
    button.changedAt = now;
  }

  if (now - button.changedAt < gotchiDebounceMs) return false;
  if (reading == button.stable) return false;

  button.stable = reading;
  return button.stable == LOW;
}

void savePet() {
  prefs.putBytes("pet", &pet, sizeof(pet));
}

bool loadPet() {
  if (!prefs.isKey("pet")) return false;
  if (prefs.getBytesLength("pet") != sizeof(pet)) return false;
  prefs.getBytes("pet", &pet, sizeof(pet));
  return pet.magic == 0x504F4B45 && pet.version == 1;
}

void showMessage(const char* text) {
  snprintf(messageText, sizeof(messageText), "%s", text);
  messageUntil = millis() + 1600;
  screen = GOTCHI_MESSAGE_SCREEN;
  beep(2200, 50);
}

void chooseStarter() {
  pet = PetState();
  if (starterChoice == 0) pet.species = GOTCHI_EEVEE;
  if (starterChoice == 1) pet.species = GOTCHI_SNORLAX;
  if (starterChoice == 2) pet.species = GOTCHI_SOLOSIS;
  pet.createdUnix = nowUnix();
  pet.lastUpdateUnix = pet.createdUnix;
  savePet();
  showMessage("Welcome!");
}

void updatePet() {
  if (!petExists()) return;

  uint32_t now = nowUnix();
  if (pet.lastUpdateUnix == 0 || now <= pet.lastUpdateUnix) {
    pet.lastUpdateUnix = now;
    return;
  }

  uint32_t minutes = (now - pet.lastUpdateUnix) / 60;
  if (minutes == 0) return;

  byte hour = currentHour();
  int light = lightRaw();

  for (uint32_t i = 0; i < minutes; i++) {
    if (i % 12 == 0) pet.hunger = changeStat(pet.hunger, -1);
    if (i % 20 == 0) pet.happiness = changeStat(pet.happiness, -1);

    if (pet.asleep || isNight(hour)) {
      pet.energy = changeStat(pet.energy, 1);
    } else if (i % 15 == 0) {
      pet.energy = changeStat(pet.energy, -1);
    }

    if (pet.hunger > 65 && pet.happiness > 65 && i % 30 == 0) {
      pet.friendship = changeStat(pet.friendship, 1);
    }

    if ((pet.hunger < 20 || pet.happiness < 20) && i % 20 == 0) {
      pet.friendship = changeStat(pet.friendship, -1);
    }
  }

  pet.asleep = isNight(hour) && light < GOTCHI_DARK_LIGHT;
  pet.lastUpdateUnix += minutes * 60;
}

const char* feedPet() {
  pet.hunger = changeStat(pet.hunger, 18);
  pet.happiness = changeStat(pet.happiness, 3);
  pet.friendship = changeStat(pet.friendship, 2);
  pet.careCount++;
  return "Yum!";
}

const char* playWithPet() {
  if (pet.energy < 10 || pet.hunger < 10) {
    pet.happiness = changeStat(pet.happiness, -2);
    return "Too tired.";
  }
  pet.happiness = changeStat(pet.happiness, 12);
  pet.friendship = changeStat(pet.friendship, 4);
  pet.energy = changeStat(pet.energy, -9);
  pet.hunger = changeStat(pet.hunger, -5);
  pet.playCount++;
  pet.careCount++;
  return "Fun!";
}

const char* restPet() {
  pet.asleep = !pet.asleep;
  if (pet.asleep) return "Sleepy...";
  return "Awake!";
}

const char* careForPet() {
  pet.happiness = changeStat(pet.happiness, 7);
  pet.friendship = changeStat(pet.friendship, 5);
  pet.careCount++;
  return "Happy!";
}

const char* evolvePet() {
  if (pet.species != GOTCHI_EEVEE || pet.evolved) return "No evolve.";
  if (pet.friendship < GOTCHI_FRIENDSHIP_EVOLVE) return "Needs bond.";

  byte hour = currentHour();
  int light = lightRaw();

  if (pet.friendship >= GOTCHI_SYLVEON_FRIENDSHIP && pet.playCount >= 12) {
    pet.species = GOTCHI_SYLVEON;
  } else if (light >= GOTCHI_BRIGHT_LIGHT) {
    pet.species = GOTCHI_LEAFEON;
  } else if (light <= GOTCHI_DARK_LIGHT) {
    pet.species = GOTCHI_GLACEON;
  } else if (isNight(hour)) {
    pet.species = GOTCHI_UMBREON;
  } else {
    pet.species = GOTCHI_ESPEON;
  }

  pet.evolved = true;
  pet.happiness = GOTCHI_STAT_MAX;
  pet.friendship = GOTCHI_STAT_MAX;
  return "Evolved!";
}

void header(const char* title) {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(title);
  display.drawLine(0, 9, GOTCHI_OLED_WIDTH - 1, 9, SSD1306_WHITE);
}

void statBar(int x, int y, const char* label, byte value) {
  display.setCursor(x, y);
  display.print(label);
  display.drawRect(x + 32, y, 72, 8, SSD1306_WHITE);
  byte fill = map(value, 0, GOTCHI_STAT_MAX, 0, 70);
  display.fillRect(x + 33, y + 1, fill, 6, SSD1306_WHITE);
}

void drawStarter() {
  const char* starters[] = {"Eevee", "Snorlax", "Solosis"};
  display.clearDisplay();
  header("Choose starter");
  for (byte i = 0; i < 3; i++) {
    display.setCursor(10, 18 + i * 14);
    display.print(i == starterChoice ? "> " : "  ");
    display.print(starters[i]);
  }
  display.display();
}

void drawHome() {
  display.clearDisplay();
  header(speciesName(pet.species));

  byte hour = currentHour();
  byte minute = currentMinute();

  display.setCursor(0, 12);
  if (hour < 10) display.print("0");
  display.print(hour);
  display.print(":");
  if (minute < 10) display.print("0");
  display.print(minute);
  display.print(pet.asleep ? "  zZ" : "  awake");

  statBar(0, 25, "Food", pet.hunger);
  statBar(0, 38, "Joy ", pet.happiness);
  statBar(0, 51, "Bond", pet.friendship);

  display.setCursor(86, 12);
  display.print("L:");
  display.print(lightRaw() / 16);
  display.display();
}

void drawMenu() {
  const char* items[] = {"Feed", "Play", "Rest", "Care", "Evolve", "Stats"};
  display.clearDisplay();
  header(speciesName(pet.species));

  for (byte row = 0; row < 4; row++) {
    byte item = (menuChoice / 4) * 4 + row;
    if (item >= 6) break;
    display.setCursor(8, 15 + row * 12);
    display.print(item == menuChoice ? "> " : "  ");
    display.print(items[item]);
  }
  display.display();
}

void drawStats() {
  uint32_t age = 0;
  if (pet.createdUnix > 0 && nowUnix() > pet.createdUnix) {
    age = (nowUnix() - pet.createdUnix) / 86400;
  }

  display.clearDisplay();
  header("Stats");
  display.setCursor(0, 13);
  display.print(speciesName(pet.species));
  display.print(" age ");
  display.print(age);
  display.print("d");
  display.setCursor(0, 25);
  display.print("Energy ");
  display.print(pet.energy);
  display.setCursor(0, 37);
  display.print("Light ");
  display.print(lightRaw());
  display.setCursor(0, 49);
  display.print("Care ");
  display.print(pet.careCount);
  display.display();
}

void drawMessage() {
  display.clearDisplay();
  header(speciesName(pet.species));
  display.setCursor(0, 25);
  display.print(messageText);
  display.display();
}

void drawScreen() {
  if (screen == GOTCHI_STARTER_SCREEN) drawStarter();
  else if (screen == GOTCHI_HOME_SCREEN) drawHome();
  else if (screen == GOTCHI_MENU_SCREEN) drawMenu();
  else if (screen == GOTCHI_STATS_SCREEN) drawStats();
  else if (screen == GOTCHI_MESSAGE_SCREEN) drawMessage();
}

void doMenuAction() {
  const char* result = "";

  if (menuChoice == 0) result = feedPet();
  if (menuChoice == 1) result = playWithPet();
  if (menuChoice == 2) result = restPet();
  if (menuChoice == 3) result = careForPet();
  if (menuChoice == 4) result = evolvePet();
  if (menuChoice == 5) {
    screen = GOTCHI_STATS_SCREEN;
    beep();
    return;
  }

  savePet();
  showMessage(result);
}

void setup() {
  Serial.begin(115200);

  pinMode(GOTCHI_SW1_PIN, INPUT_PULLUP);
  pinMode(GOTCHI_SW2_PIN, INPUT_PULLUP);
  pinMode(GOTCHI_SW3_PIN, INPUT_PULLUP);
  pinMode(GOTCHI_BUZZER_PIN, OUTPUT);
  pinMode(GOTCHI_LDR_PIN, INPUT);
  analogReadResolution(12);

  Wire.begin(GOTCHI_SDA_PIN, GOTCHI_SCL_PIN);
  Wire.setClock(400000);

  display.begin(SSD1306_SWITCHCAPVCC, GOTCHI_OLED_ADDR);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Pokegotchi");
  display.println("Booting...");
  display.display();

  rtcReady = rtc.begin();
  if (rtcReady && rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  prefs.begin("pokegotchi", false);
  if (loadPet() && petExists()) screen = GOTCHI_HOME_SCREEN;
  else screen = GOTCHI_STARTER_SCREEN;

  drawScreen();
}

void loop() {
  bool left = pressed(sw1);
  bool select = pressed(sw2);
  bool right = pressed(sw3);
  bool anyButton = left || select || right;
  unsigned long now = millis();

  if (now - lastTick >= GOTCHI_TICK_MS) {
    lastTick = now;
    updatePet();
    drawScreen();
  }

  if (screen == GOTCHI_MESSAGE_SCREEN && now > messageUntil) {
    screen = GOTCHI_HOME_SCREEN;
    drawScreen();
  }

  if (screen == GOTCHI_STARTER_SCREEN) {
    if (left) starterChoice = (starterChoice + 2) % 3;
    if (right) starterChoice = (starterChoice + 1) % 3;
    if (select) chooseStarter();
  } else if (screen == GOTCHI_HOME_SCREEN) {
    if (anyButton) {
      menuChoice = 0;
      screen = GOTCHI_MENU_SCREEN;
      beep();
    }
  } else if (screen == GOTCHI_MENU_SCREEN) {
    if (left) menuChoice = (menuChoice + 5) % 6;
    if (right) menuChoice = (menuChoice + 1) % 6;
    if (select) doMenuAction();
  } else if (screen == GOTCHI_STATS_SCREEN) {
    if (anyButton) {
      screen = GOTCHI_HOME_SCREEN;
      beep();
    }
  }

  if (anyButton) drawScreen();

  if (petExists() && now - lastSave >= GOTCHI_SAVE_MS) {
    lastSave = now;
    savePet();
  }
}
