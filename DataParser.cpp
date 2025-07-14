#include "DataParser.h"

DataParser::DataParser() {
  _delimiter = ',';
  _fieldCount = 0;
}

void DataParser::parseData(const char* data, char delimiter) {
  _delimiter = delimiter;
  _fieldCount = 0;

  char* mutableData = strdup(data); // Duplicate to safely modify
  char* token = strtok(mutableData, &_delimiter);

  while (token != NULL && _fieldCount < 50) {
    _fields[_fieldCount++] = String(token);
    token = strtok(NULL, &_delimiter);
  }

  free(mutableData); // Clean up
}

void DataParser::parseData(const String& data, char delimiter) {
  parseData(data.c_str(), delimiter); // Convert to C-style string
}

String DataParser::getField(int index) {
  if (index >= 0 && index < _fieldCount) {
    return _fields[index];
  } else {
    return "";
  }
}

int DataParser::getFieldCount() {
  return _fieldCount;
}
