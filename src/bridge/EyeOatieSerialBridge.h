#ifndef EyeOatieSerialBridge_h
#define EyeOatieSerialBridge_h

#include <Arduino.h>
#include <painlessMesh.h>

class EyeOatieSerialBridge {
    public:
        EyeOatieSerialBridge(String meshSsid, String meshPassword);

        void update();
    private:
        painlessMesh mesh;
        Scheduler scheduler;
        void receivedCallback(uint32_t from, String& message);
        void newConnectionCallback(uint32_t from);
};

#endif