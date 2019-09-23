#include "EyeOatieNode.h"

EyeOatieNode::EyeOatieNode(String name, String meshSsid = "EyeOatieMesh", String meshPassword = "def@ultMeshPa$$12") {
    mesh.setDebugMsgTypes(ERROR | STARTUP);
    mesh.init(meshSsid, meshPassword, &scheduler);

    using namespace std::placeholders;
    std::function<void(uint32_t, String&)> boundOnReceive = std::bind(&EyeOatieNode::receivedCallback, this, _1, _2);
    mesh.onReceive(boundOnReceive);

    nodeId = mesh.getNodeId();
    nodeName = name;
}

void EyeOatieNode::receivedCallback(uint32_t from, String& message) {
    DynamicJsonDocument meshRequest(1024);
    DeserializationError error = deserializeJson(meshRequest, message);
    if (error) {
        Serial.println("Error deserializing json");
        Serial.println(error.c_str());
        return;
    }

    if (meshRequest["type"] == "nodeInfoRequest") {
        mesh.sendSingle(from, getNodeInfo());
    } else if (meshRequest["type"] == "getDataRequest") {
        String data = getData(meshRequest["name"]);
        String type = getDataType(meshRequest["name"]);
        DynamicJsonDocument dataResponseDocument(512);
        JsonObject dataResponse = dataResponseDocument.to<JsonObject>();
        dataResponse["nodeId"] = nodeId;
        dataResponse["name"] = meshRequest["data"];
        dataResponse["data"] = data;
        dataResponse["type"] = type;
        String msg;
        serializeJson(dataResponseDocument, msg);
        mesh.sendSingle(from, msg);
    } else if (meshRequest["type"] == "actionRequest") {
        performAction(meshRequest["name"]);
    }
}

void EyeOatieNode::update() {
    mesh.update();
}

void EyeOatieNode::addData(String name, String type, std::function<String()> getData) {
    dataNameArray[dataArraysHead] = name;
    dataTypeArray[dataArraysHead] = type;
    dataGetterArray[dataArraysHead] = getData;
    dataArraysHead++;
}

String EyeOatieNode::getData(String name) {
    for (int i = 0; i < dataArraysHead; i++) {
        if (dataNameArray[i].equals(name)) return (dataGetterArray[i])();
    }
    return "";
}

String EyeOatieNode::getDataType(String name) {
    for (int i = 0; i < dataArraysHead; i++) {
        if (dataNameArray[i].equals(name)) return dataTypeArray[i];
    }
    return "";
}

void EyeOatieNode::addAction(String name, std::function<void()> actionCallback) {
    actionNameArray[actionArraysHead] = name;
    actionCallbackArray[actionArraysHead] = actionCallback;
    actionArraysHead++;
}

void EyeOatieNode::performAction(String name) {
    for (int i = 0; i < actionArraysHead; i++) {
        if (actionNameArray[i].equals(name)) (actionCallbackArray[i])();
    }
}

String EyeOatieNode::getNodeInfo() {
    DynamicJsonDocument doc(1024);
    JsonObject nodeInfo = doc.to<JsonObject>();

    nodeInfo["name"] = nodeName;
    nodeInfo["nodeId"] = nodeId;

    JsonArray data = nodeInfo.createNestedArray("data");
    for (int i = 0; i < dataArraysHead; i++) {
        JsonObject dataObject = data.createNestedObject();
        dataObject["name"] = dataNameArray[i];
        dataObject["type"] = dataTypeArray[i];
    }

    JsonArray actions = nodeInfo.createNestedArray("actions");
    for (int i = 0; i < actionArraysHead; i++) {
        JsonObject actionObject = data.createNestedObject();
        actionObject["name"] = actionNameArray[i];
    }

    String str;
    serializeJson(doc, str);
    return str;
}