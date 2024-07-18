#include "DataPars.h"
#include <sstream>
#include <regex>
#include <iostream>

using namespace std;

vector<std::string> parseData(const std::string& message) 
{
    vector<std::string> parts;
    istringstream iss(message);
    string token;

    while (getline(iss, token, '/')) {
        parts.push_back(token);
    }

    return parts;
}

bool parseSensorData(const std::string& part, vector<float>& data, string& table) 
{
    regex sensor_regex(R"((Sensor))");
    regex havc_regex(R"((HAVC))");
    regex ev_regex(R"((EV))");
    regex es_regex(R"((ES))");
    regex ventil_regex(R"((VENTIL))");
    regex ventil_control_regex(R"((VC))");

    string cleaned_part;

    if (regex_search(part, sensor_regex)) {
        cleaned_part = std::regex_replace(part, sensor_regex, "");
        table = "Sensor";
    }
    else if (regex_search(part, havc_regex)) {
        cleaned_part = std::regex_replace(part, havc_regex, "");
        table = "HAVC";
    }
    else if (regex_search(part, ev_regex)) {
        cleaned_part = std::regex_replace(part, ev_regex, "");
        table = "EV";
    }
    else if (regex_search(part, es_regex)) {
        cleaned_part = std::regex_replace(part, es_regex, "");
        table = "ES";
    }
    else if (regex_search(part, ventil_regex)) {
        cleaned_part = std::regex_replace(part, ventil_regex, "");
        table = "VENTIL";
    }
    else if (regex_search(part, ventil_control_regex)) {
        cleaned_part = std::regex_replace(part, ventil_control_regex, "");
        table = "VC";
    }
    else {
        return false;
    }

    istringstream tableStream(cleaned_part);
    string value;
    while (getline(tableStream, value, ',')) {
        try {
            data.push_back(std::stof(value));
        }
        catch (const invalid_argument& e) {
            return false;
        }
    }

    return true;
}
