#include "EyeOatieNode.hpp"

#include <Arduino.h>
#include <painlessMesh.h>
#include <ArduinoJson.h>

NodeFunction::NodeFunction(const char *name, node_function_ptr f_ptr, function_type type) 
    : input_schema(512), name(name), f_ptr(f_ptr), type(type) {}

void EyeOatieNode::init(String ssid, String password, short unsigned int port) {
    mesh.init(ssid, password, &userScheduler, port);
    mesh.onReceive([this](unsigned int from, String &message) {
        this->onReceive(from, message);
    });
    mesh.setContainsRoot();

    node_id = mesh.getNodeId();
}

void EyeOatieNode::setMeshDebugMsgTypes(short unsigned int types) {
    mesh.setDebugMsgTypes(types);
}

void EyeOatieNode::update() {
    mesh.update();
}

void EyeOatieNode::onReceive(unsigned int from, String &message) {
    Serial.println(message);
    DynamicJsonDocument doc(512);
    DeserializationError err = deserializeJson(doc, message);
    if (err) {
        Serial.println(String(err.c_str()));
        return;
    }
    
    char m_type = doc["type"].as<char>();
    unsigned short request_id = doc["request_id"];
    DynamicJsonDocument response(1024);
    response["to"] = from;
    response["from"] = node_id;
    response["request_id"] = request_id;
    if (m_type == 0) { //getNodeInfoReq
        getNodeInfo(response);
    } else if (m_type == 2) { //functionRequest
        const char *name = doc["function"];
        JsonVariant args = doc["args"];
        if (name == nullptr or args == nullptr) {
            Serial.println("Error parsing function name or args");
            return;
        }
        NodeFunction *function = getFunction(name);
        if (function == nullptr) {
            Serial.println("Cannot find function");
            return;
        }
        response["type"] = 3;
        const char *res = runFunction(function, args);
        response["value"] = res;

    }
    String serialized;
    serializeJson(response, serialized);
    mesh.sendSingle(from, serialized);
}

NodeFunction& EyeOatieNode::registerFunction(const char *name, node_function_ptr f_ptr, function_type type) {
    NodeFunction f(name, f_ptr, type);
    functions.push_back(f);
    return f;
}

NodeFunction *EyeOatieNode::getFunction(const char *name) {
    for (int i = 0; i<functions.size(); i++) {
        NodeFunction func = functions[i];
        if (strcmp(name, func.name) == 0) return &func;
    }
    return nullptr;
}

const char *EyeOatieNode::runFunction(NodeFunction *f, JsonVariant args) {
    return f->f_ptr(args).c_str();
}

void EyeOatieNode::getNodeInfo(DynamicJsonDocument doc) {
    doc["type"] = 1;
    for (int i = 0; i<functions.size(); i++) {
        doc["functions"][i]["name"] = functions[i].name;
        doc["functions"][i]["args"] = (functions[i].input_schema).as<JsonObject>();
    }
}