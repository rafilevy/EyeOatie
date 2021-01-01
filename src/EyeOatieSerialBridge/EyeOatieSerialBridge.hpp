#ifndef EyeOatieSerialBridge_h
#define EyeOatieSerialBridge_h

#include <Arduino.h>
#include <painlessMesh.h>

class EyeOatieSerialBridge {
    private:
        unsigned int node_id;
        painlessMesh mesh;

        void onReceive(unsigned int from, String& message);
        void onNewConnection(unsigned int from);

    public:
        void setMeshDebugMsgTypes(short unsigned int types);

        void init(String meshSsid, String meshPassword, short unsigned int port = 5555);

        void update();
};

#endif