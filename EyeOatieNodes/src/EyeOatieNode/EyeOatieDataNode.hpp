#ifndef EyeOatieDataNode_h
#define EyeOatieDataNode_h

#include <Arduino.h>
#include <painlessMesh.h>
#include <ArduinoJson.h>
#include "../TypeNames/TypeNames.hpp"

enum NodeType { DATA, ACTION, DATA_ACTION};

template <typename T, typename ...Args>
using function_t = T (*)(Args...);

template <typename T, typename ...Args>
class EyeOatieDataNode {
    private:
        const char *_name;

        function_t<T, Args...> _function_ptr;

        unsigned int _node_id;
        painlessMesh _mesh;
        
        NodeType _node_type;

        void onReceive(unsigned int from, String &message);
        
    public:
        EyeOatieDataNode(const char *name, function_t<T, Args...> function_ptr);

        void init(const String& ssid, const String& password, short unsigned int port = 5555);

        void update();
};

//Constructor
template <typename T, typename ...Args>
EyeOatieDataNode<T, Args...>::EyeOatieDataNode(const char *name, function_t<T, Args...> function_ptr) : _name(name), _function_ptr(function_ptr), _node_type(DATA) {}

//Initializer function
template <typename T, typename ...Args>
void EyeOatieDataNode<T, Args...>::init(const String& ssid, const String& password, short unsigned int port) {
    _mesh.init(ssid, password, port);
    _mesh.setContainsRoot();

    _mesh.onReceive([this](unsigned int from, String &message) {
        this->onReceive(from, message);
    });

    _node_id = _mesh.getNodeId();
}

template <typename T, typename ...Args>
void EyeOatieDataNode<T, Args...>::update() {
    _mesh.update();
}

template <typename T, typename ...Args>
void EyeOatieDataNode<T, Args...>::onReceive(unsigned int from, String &message) {
    DynamicJsonDocument doc(256);
    deserializeJson(doc, message);

    char type = doc["type"];
    
    DynamicJsonDocument response(256);
    response["to"] = from;
    response["from"] = _node_id;
    if (type == 0) { //nodeInfoRequest
        response["type"] = 4;
        JsonObject value = response.createNestedObject("value"); 
        value["name"] = _name;
        value["type"] = _node_type;
        value["return"] = TypeName<T>::get();
        JsonArray args = value.createNestedArray("args");
        char _[] = { (args.add(TypeName<Args>::get()), 0)... };
    } else if (type == 1) { //dataRequest
        response["type"] = 5;
        JsonArray args = doc["args"];
        char i = 0;
        T data = _function_ptr((args[i++].as<Args>())...);
        response["value"] = data;
    }
    String serialized;
    serializeJson(response, serialized);
    Serial.println(serialized);
    _mesh.sendSingle(from, serialized);
}
#endif