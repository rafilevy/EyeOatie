#include "EyeOatieSerialBridge.hpp"

#define PRINT_NODE_MSG(msg) Serial.printf("NODE_MSG:%s\n", msg.c_str())

void EyeOatieSerialBridge::init(const String& meshSsid, const String& meshPassword, short unsigned int port) {
    mesh.init(meshSsid, meshPassword, port, WIFI_AP);
    
    mesh.onReceive([this](unsigned int from, String &message) {
        this->onReceive(message);
    });
    mesh.onNewConnection([this](unsigned int from) {
        this->onNewConnection(from);
    });

    mesh.setRoot();
    mesh.setContainsRoot();

    node_id = mesh.getNodeId();
}

void EyeOatieSerialBridge::update() {
    mesh.update();
    if (Serial.available() > 0) {
        String input = Serial.readString();
        if (input != "") {
            //Deserialize the message received from the server
            DynamicJsonDocument serial_request(256);
            DeserializationError error = deserializeJson(serial_request, input);
            if (error) {
                Serial.printf("DEBUG:%s\n", error.c_str());
                return;
            }

            if (serial_request["type"] == 128) { //Request to get all nodess
                std::list<unsigned int> nodes = mesh.getNodeList();
                const size_t node_count = nodes.size();
                const size_t doc_size = JSON_OBJECT_SIZE(3) //type, request_id, values
                    + JSON_ARRAY_SIZE(node_count);
                DynamicJsonDocument doc(doc_size);

                doc["from"] = node_id;
                doc["type"] = 130; //NODE_LIST_RESPONSE

                JsonArray ids = doc.createNestedArray("ids");
                for (unsigned int nid : nodes) {
                    ids.add(nid);
                }

                String serialized;
                serializeJson(doc, serialized);
                PRINT_NODE_MSG(serialized);
            } else {
                serial_request["from"] = node_id; //Set me as the sender
                String serialized;
                serializeJson(serial_request, serialized);
                mesh.sendSingle(serial_request["to"], serialized);
            }
        }
    }
}

void EyeOatieSerialBridge::onReceive(String& message) {
    PRINT_NODE_MSG(message);
}

void EyeOatieSerialBridge::onNewConnection(unsigned int from) {
    DynamicJsonDocument doc(64); //Size calculated with ArduinoJson assistant
    doc["type"] = 128;
    doc["to"] = node_id;
    doc["from"] = from;
    String serialized;
    serializeJson(doc, serialized);
    PRINT_NODE_MSG(serialized);
}