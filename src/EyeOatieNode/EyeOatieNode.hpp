#ifndef EyeOatieNode_h
#define EyeOatieNode_h

#include <Arduino.h>
#include <painlessMesh.h>
#include <ArduinoJson.h>

typedef String (*node_function_ptr)(JsonVariant args);

enum function_type {
    DATA_FUNCTION,
    ACTION_FUNCTION
};
struct NodeFunction {
    const char *name;
    DynamicJsonDocument input_schema;
    node_function_ptr f_ptr;
    function_type type;

    NodeFunction(const char *name, node_function_ptr f_ptr, function_type type);
    template<typename T>
    NodeFunction& addArgument(const char *name, T value);
};
template <typename T>
NodeFunction& NodeFunction::addArgument(const char *name, T default_value)  {
    input_schema["name"] = default_value;
    return *this;
}

class EyeOatieNode {
    private:
        unsigned int node_id;
        painlessMesh mesh;
        Scheduler userScheduler;

        void onReceive(unsigned int from, String &message);

        NodeFunction *getFunction(const char *name);
        const char* runFunction(NodeFunction *function, JsonVariant args);
        std::vector<NodeFunction> functions;

        void getNodeInfo(DynamicJsonDocument doc);
        
    public:
        void setMeshDebugMsgTypes(short unsigned int types);

        void init(String ssid, String password, short unsigned int port = 5555);

        void update();

        NodeFunction& registerFunction(const char *name, node_function_ptr f_ptr, function_type type);
};

#endif