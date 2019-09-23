#ifndef EyeOatieNode_h
#define EyeOatieNode_h

#include <Arduino.h>
#include <painlessMesh.h>

class EyeOatieNode {
    public:
        EyeOatieNode(String name, String meshSsid, String meshPassword);

        void addData(String name, String type, std::function<String()> getData);
        void addAction(String name, std::function<void()> actionCallback);

        void update();
    private:
        void receivedCallback(uint32_t from, String& message);
        painlessMesh mesh;
        Scheduler scheduler;
        uint32_t nodeId;
        String nodeName;
        
        String getNodeInfo();

        String dataNameArray[10];
        String dataTypeArray[10];
        std::function<String()> dataGetterArray[10];
        int dataArraysHead;
        String getData(String name);
        String getDataType(String name);

        String actionNameArray[10];
        std::function<void()> actionCallbackArray[10];
        int actionArraysHead;
        void performAction(String name);
};

#endif