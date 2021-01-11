#ifndef EyeOatieSerialBridge_h
#define EyeOatieSerialBridge_h

#include <Arduino.h>
#include <painlessMesh.h>

class EyeOatieSerialBridge {
    private:
        unsigned int node_id;
        unsigned char node_number = 0;
        painlessMesh mesh;

        void onReceive(String& message);
        void onNewConnection(unsigned int from);

    public:
        void init(const String& meshSsid, const String& meshPassword, short unsigned int port = 5555);

        void update();
};

#endif