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
        int noParams = meshRequest["params"].size();
        if (noParams == 0) {
            performAction(meshRequest["name"], {});
        }
        else {
            String params[noParams];
            for (int i = 0; i < noParams; i++) {
                params[i] = meshRequest["params"][i].as<String>();
            }
            performAction(meshRequest["name"], params);
        }
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
    String emptyStringArray[] = {};
    std::function<void(String[])> boundCallback = [&](String[]) { actionCallback(); };
    actionCallbackArray[actionArraysHead] = boundCallback;
    actionParamNumbers[actionArraysHead] = 0;
    actionArraysHead++;
}

void EyeOatieNode::addAction(String name, std::function<void(String[])> actionCallback, String paramNames[], String paramTypes[], int noParams) {
    actionNameArray[actionArraysHead] = name;
    actionCallbackArray[actionArraysHead] = actionCallback;
    actionParamNumbers[actionArraysHead] = noParams;
    for (int i = 0; i < noParams; i++) {
        actionParamsNameArray[actionArraysHead][i] = paramNames[i];
        actionParamsTypeArray[actionArraysHead][i] = paramTypes[i];
    }
    actionArraysHead++;
}

void EyeOatieNode::performAction(String name, String params[]) {
    for (int i = 0; i < actionArraysHead; i++) {
        if (actionNameArray[i].equals(name)) (actionCallbackArray[i])(params);
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
        JsonArray params = actionObject.createNestedArray("params");
        for (int j = 0; j < actionParamNumbers[i]; j++) {
            JsonObject paramObject = params.createNestedObject();
            paramObject["data"] = actionParamsNameArray[i][j];
            paramObject["type"] = actionParamsTypeArray[i][j];
        }
    }

    String str;
    serializeJson(doc, str);
    return str;
}