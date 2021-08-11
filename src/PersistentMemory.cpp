/*
  CryptoGarage - PersistentMemory

  (implementation)
*/

#include "PersistentMemory.h"


PersistentMemory::PersistentMemory(String settingsfile, bool create) : filename("/" + settingsfile + ".json"), json(1024) {
  printDebug("[PMEM] Open vault " + settingsfile);
  //noInterrupts();
  File file = LittleFS.open(filename, "r");
  if (!file && create) {
    initsuccess = true;
    changed = true;
  } else if (file && !deserializeJson(json, file)) {
    initsuccess = true;
  }
  file.close();
  createifnotfound = create;
  //interrupts();
}

PersistentMemory::~PersistentMemory() {}

PersistentMemory::operator bool() {
  return initsuccess;
}

void PersistentMemory::format() {
#ifdef ARDUINO_ARCH_ESP32
  //noInterrupts();
  File dir = LittleFS.open("/");
  File file = dir.openNextFile();
  while(file){
	String filename = file.name();
	if (filename.endsWith(".json")) {
		LittleFS.remove(file.name());
    }
	file = dir.openNextFile();
  }
  //interrupts();
#else
  //noInterrupts();
  Dir dir = LittleFS.openDir("/");
  while (dir.next()) {
    if (dir.fileName().endsWith(".json")) {
      LittleFS.remove(dir.fileName());
    }
  }
  //interrupts();
#endif
  printDebug("[PMEM] Format!");
}

bool PersistentMemory::remove(String &vault) {
  String name = vault + ".json";
  printDebug("[PMEM] Removing vault " + vault);
  //noInterrupts();
  bool out = LittleFS.remove(name); 
  //interrupts();
  return out;
}

int PersistentMemory::listVaults(String * arr, int size) {
#ifdef ARDUINO_ARCH_ESP32
  //noInterrupts();
  File dir = LittleFS.open("/");
  int i = 0;
  File file = dir.openNextFile();
  while(file){
	String filename = file.name();
	if (filename.endsWith(".json")) {
      if(i < size){
        filename.replace(".json", "");
        arr[i++] = filename.substring(1);
      }
    }
	file = dir.openNextFile();
  }
  //interrupts();
  return i;
#else
  //noInterrupts();
  Dir dir = LittleFS.openDir("/");
  int i = 0;
  while (dir.next()) {
    if (dir.fileName().endsWith(".json")) {
      if(i < size){
        String filename = dir.fileName();
        filename.replace(".json", "");
        arr[i++] = filename;
      }
    }
  }
  //interrupts();
  return i;
#endif

}

String PersistentMemory::toJSON(String &vault) {
  //noInterrupts();
  File file = LittleFS.open("/" + vault + ".json", "r");
  String out = "";
  int size = file.size();
  if (size){
    uint8_t buf[size + 1];
    file.read(buf, size);
    buf[size] = '\0';
    out = (char*)buf;
  }
  file.close();
  //interrupts();
  return out;
}

void PersistentMemory::commit() {
  if (changed) {
	//noInterrupts();
    File file = LittleFS.open(filename, "w");
    serializeJsonPretty(json, file);
    file.close();
	//interrupts();
  }
}

String PersistentMemory::readString(const char * key, String fallback) {
  JsonVariant data = json[key];
  if (!data.isNull()) {
    return data;
  } else {
    if (createifnotfound) {
      writeString(key, fallback);
    }
    return fallback;
  }
}

String PersistentMemory::readString(String &key, String fallback) {
  return readString(key.c_str(), fallback);
}

void PersistentMemory::writeString(const char * key, String value) {
  json[key] = value;
  changed = true;
}

void PersistentMemory::writeString(String &key, String value) {
  return writeString(key.c_str(), value);
}

bool PersistentMemory::readBool(const char * key, bool fallback) {
  return readInt(key, fallback);
}

bool PersistentMemory::readBool(String &key, bool fallback) {
  return readInt(key.c_str(), fallback);
}

void PersistentMemory::writeBool(const char * key, bool value) {
  json[key] = (int)value;
  changed = true;
}

void PersistentMemory::writeBool(String &key, bool value) {
  return writeBool(key.c_str(), value);
}

int PersistentMemory::readInt(const char * key, int fallback) {
  JsonVariant data = json[key];
  if (!data.isNull()) {
	  return data;
  } else {
    if (createifnotfound) {
      json[key] = fallback;
      changed = true;
    }
    return fallback;
  }
}

int PersistentMemory::readInt(String &key, int fallback) {
  return readInt(key.c_str(), fallback);
}

void PersistentMemory::writeInt(const char * key, int value) {
  json[key] = value;
  changed = true;
}

void PersistentMemory::writeInt(String &key, int value) {
  return writeInt(key.c_str(), value);
}

double PersistentMemory::readDouble(const char * key, double fallback) {
  JsonVariant data = json[key];
  if (!data.isNull()) {
    return data.as<double>();
  } else {
    if (createifnotfound) {
      json[key] = fallback;
      changed = true;
    }
    return fallback;
  }
}

double PersistentMemory::readDouble(String &key, double fallback) {
  return readDouble(key.c_str(), fallback);
}

void PersistentMemory::writeDouble(const char * key, double value) {
  json[key] = value;
  changed = true;
}

void PersistentMemory::writeDouble(String &key, double value) {
  return writeDouble(key.c_str(), value);
}
