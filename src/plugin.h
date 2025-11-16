//
// Created by xplane on 08/11/2025.
//
#ifndef X_SMART_BUTTON_REMAP_PLUGIN_H
#define X_SMART_BUTTON_REMAP_PLUGIN_H

#include "XPLMPlugin.h"
#include "XPLMUtilities.h"
#include "XPLMProcessing.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include <string>

#include "xshifter.h"

namespace shifter
{
    class plugin
    {
    public:
        plugin();
        ~plugin();

        bool start(char* outName, char* outSig, char* outDesc);
        void stop();
        int  enable();
        void disable();
        void receiveMessage(XPLMPluginID inFromWho, int inMessage, void* inParam);
        static float m_pluginCallback (float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void *inRefcon); // cb
        static int m_drawCallback (XPLMDrawingPhase inPhase, int inIsBefore, void *inRefcon);
    private:
        std::string name_;
        std::string signature_;
        std::string description_;

        xshifter shifter_;
    };
} // shifter

/*
_ (trailing underscore)     name_ Member variable name: Common in modern C++ style guides
s_ Static member or variable "s_instance" Indicates shared state
k_  Constant
g_  Global
p_  Point or _ptr
t_ or T   Template
*/

#endif //X_SMART_BUTTON_REMAP_PLUGIN_H