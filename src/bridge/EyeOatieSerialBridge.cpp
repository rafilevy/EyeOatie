#include "EyeOatieSerialBridge.h"

EyeOatieSerialBridge::EyeOatieSerialBridge(String meshSsid = "EyeOatieMesh", String meshPassword= "def@ultMeshPa$$12") {
    mesh.setDebugMsgTypes(ERROR | STARTUP);
    mesh.init(meshSsid, meshPassword, &scheduler);

    using namespace std::placeholders;
    std::function<void(uint32_t, String&)> boundOnReceive = std::bind(&EyeOatieSerialBridge::receivedCallback, this, _1, _2);
    mesh.onReceive(boundOnReceive);

    std::function<void(uint32_t)> boundOnNewConnection = std::bind(&EyeOatieSerialBridge::newConnectionCallback, this, _1);
    mesh.onNewConnection(boundOnNewConnection);
}

void EyeOatieSerialBridge::update() {
    mesh.update();
    if (Serial.available()) {
        String input = Serial.readString();
        if (input != "") {
            DynamicJsonDocument serialRequest(512);
            DeserializationError error = deserializeJson(serialRequest, input);
            if (error) return;
            uint32_t recipient = serialRequest["nodeId"];
            mesh.sendSingle(recipient, input);
        }
    }
}

void EyeOatieSerialBridge::receivedCallback(uint32_t from, String& message) {
    Serial.print(message);
}

void EyeOatieSerialBridge::newConnectionCallback(uint32_t from) {
    DynamicJsonDocument doc(64);
    JsonObject meshMessage = doc.to<JsonObject>();

    meshMessage["type"] = "nodeInfoRequest";
    String str;

    serializeJson(meshMessage, str);
    mesh.sendSingle(from, str);
}