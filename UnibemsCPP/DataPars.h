#pragma once
#ifndef DATAPARS_H
#define DATAPARS_H

#include <vector>
#include <string>


using namespace std;

vector<string> parseData(const string& message);
bool parseSensorData(const string& part, vector<float>& data, string& table);

#endif // !DATAPARS_H
