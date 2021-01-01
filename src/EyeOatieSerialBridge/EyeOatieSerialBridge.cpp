#include "EyeOatieSerialBridge.hpp"

void EyeOatieSerialBridge::init(String meshSsid, String meshPassword, short unsigned int port) {
    mesh.init(meshSsid, meshPassword, port, WIFI_AP);
    
    mesh.onReceive([this](unsigned int from, String &message) {
        this->onReceive(from, message);
    });
    mesh.onNewConnection([this](unsigned int from) {
        this->onNewConnection(from);
    });

    mesh.setRoot();
    mesh.setContainsRoot();

    node_id = mesh.getNodeId();
}

void EyeOatieSerialBridge::setMeshDebugMsgTypes(short unsigned int types) {
    mesh.setDebugMsgTypes(types);
}

void EyeOatieSerialBridge::update() {
    mesh.update();
    if (Serial.available() > 0) {
        String input = Serial.readString();
        if (input != "") {
            DynamicJsonDocument serialRequest(512);
            DeserializationError error = deserializeJson(serialRequest, input);
            if (error) {
                Serial.printf("DEBUG:%s", error.c_str());
                return;
            }
            serialRequest["from"] = node_id;
            String res;
            serializeJson(serialRequest, res);
            if (serialRequest.containsKey("brodcast") && serialRequest["broadcast"].as<bool>()) {
                mesh.sendBroadcast(res);
            }
            if (serialRequest.containsKey("to")) {
                short unsigned recipient = serialRequest["to"];
                mesh.sendSingle(recipient, res);
            }
        }
    }
}

void EyeOatieSerialBridge::onReceive(unsigned int from, String& message) {
    Serial.printf("NODE_MSG:%s", message.c_str());
}

void EyeOatieSerialBridge::onNewConnection(unsigned int from) {
    DynamicJsonDocument doc(64);
    doc["type"] = 4;
    doc["to"] = node_id;
    doc["from"] = from;
    String str;
    serializeJson(doc, str);
    Serial.printf("NODE_MSG:%s",str);
}