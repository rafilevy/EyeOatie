#ifndef EyeOatieNode_h
#define EyeOatieNode_h

#include <Arduino.h>
#include <painlessMesh.h>

typedef String (*DataFuncPtr)();
typedef void (*ActionFuncPtr)(String[]);

class EyeOatieNode {
    public:
        EyeOatieNode(String name, String meshSsid, String meshPassword);

        void addData(String name, String type, DataFuncPtr getData);
        void addAction(String name, ActionFuncPtr actionCallback, String paramNames[], String paramTypes[], int noParams);

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
        DataFuncPtr dataGetterArray[10];

        int dataArraysHead;
        String getData(String name);
        String getDataType(String name);

        String actionNameArray[10];
        String actionParamsNameArray[10][5];
        String actionParamsTypeArray[10][5];
        ActionFuncPtr actionCallbackArray[10];
        int actionArraysHead;
        int actionParamNumbers[10];
        void performAction(String name, String params[]);
};

#endif