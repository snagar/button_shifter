//
// Created by xplane on 08/11/2025.
//

#include "plugin.h"
#include <cstdio>
#include <XPLMDataAccess.h>

// Create a global instance of the plugin class
static shifter::plugin gPlugin;
static shifter::xshifter gShifter;

namespace shifter
{
  // ---------------------------------------------------------------------
  // Constructor / Destructor
  // ---------------------------------------------------------------------
  plugin::plugin()
      : name_("Button Shifter v0.1"),
        signature_("button_shifter.snagar.dev"),
        description_("A custom Joystick button enabler")
  {

  }

  plugin::~plugin()
  {
  }

  bool plugin::start(char* outName, char* outSig, char* outDesc)
{
    XPLMEnableFeature ("XPLM_USE_NATIVE_PATHS", 1);
    XPLMEnableFeature ("XPLM_USE_NATIVE_WIDGET_WINDOWS", 1);

    std::snprintf(outName, 256, "%s", name_.c_str());
    std::snprintf(outSig, 256, "%s", signature_.c_str());
    std::snprintf(outDesc, 256, "%s", description_.c_str());

    // Define the main menu + ad-hoc commands
    gShifter.plugin_start(outName, outSig, outDesc);


    XPLMDebugString("[button_shifter] Plugin started successfully.\n");

    return true;
}

void plugin::stop()
{
    XPLMDebugString("[button_shifter] Plugin stopped.\n");
}

int plugin::enable()
{
    XPLMRegisterFlightLoopCallback (plugin::m_pluginCallback, -1, nullptr);
    // XPLMRegisterDrawCallback (plugin::m_drawCallback, shifter::s_drawPhase, 0, nullptr); // xplane v1141xxx and lower or v1151 + opengl

    XPLMDebugString("[button_shifter] Plugin enabled.\n");

    return 1;
}

void plugin::disable()
{
  XPLMUnregisterFlightLoopCallback (plugin::m_pluginCallback, nullptr);

  // XPLMUnregisterDrawCallback (m_drawCallback, xplm_Phase_Objects, 0, nullptr);
  // XPLMUnregisterDrawCallback (m_drawCallback, xplm_Phase_Window, 0, nullptr);
  // XPLMUnregisterDrawCallback (m_drawCallback, xplm_Phase_Modern3D, 0, nullptr);
  // XPLMUnregisterDrawCallback (m_drawCallback, shifter::s_drawPhase, 0, nullptr);

  XPLMDebugString("[button_shifter] Plugin disabled.\n");
}

void plugin::receiveMessage(XPLMPluginID inFromWho, int inMessage, void* inParam)
{
    // Handle messages from X-Plane or other plugins here
    (void)inFromWho;
    (void)inMessage;
    (void)inParam;
}
float
plugin::m_pluginCallback (float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void *inRefcon)
{
    return shifter::xshifter::m_pluginCallback (inElapsedSinceLastCall, inElapsedTimeSinceLastFlightLoop, inCounter, inRefcon);
}

int
plugin::m_drawCallback (XPLMDrawingPhase inPhase, int inIsBefore, void *inRefcon)
{
    // shifter::xshifter::m_drawCallback (inPhase, inIsBefore, inRefcon);
    return 1;
}

// ---------------------------------------------------------------------
// X-Plane Required C-style Entry Points
// ---------------------------------------------------------------------
extern "C" {

// Forward declaration
// PLUGIN_API int drawCallback (XPLMDrawingPhase inPhase, int inIsBefore, void *inRefcon);


PLUGIN_API int XPluginStart(char* outName, char* outSig, char* outDesc)
{
  // fetch x-plane metadata to use in the plugin
  // version
  XPLMGetVersions (&shifter::xplane_ver_i, &shifter::xplm_version, &shifter::host_id);
  shifter::xplane_ver_i = XPLMGetDatai (XPLMFindDataRef ("sim/version/xplane_internal_version")); // xplane version number as an integer
  // draw driver type
  shifter::xplane_using_modern_driver_b = XPLMGetDatai (XPLMFindDataRef ("sim/graphics/view/using_modern_driver"));

  // Determined Draw Phase
  if (shifter::xplane_using_modern_driver_b)
  {
    if (shifter::xplane_ver_i >= shifter::XP12_VERSION_NO)
    {
      //X-Plane 12, xplm_Phase_Window seem the only working phase but not the other phases
      shifter::s_drawPhase = xplm_Phase_Window;
    }
    else
    {
      // X-Plane 11
      shifter::s_drawPhase = xplm_Phase_Modern3D;
    }
  }
  else
  { // Open GL - X-Plane 11
    shifter::s_drawPhase = xplm_Phase_Objects;
  }

   ///////////////////////
  // Call the C++ bridge
  ///////////////////////
  gPlugin.start (outName, outSig, outDesc);

    return 1;
}

PLUGIN_API void XPluginStop(void)
{
    gPlugin.stop();
}

PLUGIN_API int XPluginEnable(void)
{
  gPlugin.enable ();

  return 1; // important so plugin will be in enable state
}

PLUGIN_API void XPluginDisable(void)
{
  gPlugin.disable ();

}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, int inMessage, void* inParam)
{
    gPlugin.receiveMessage(inFromWho, inMessage, inParam);
}

} // extern "C"

} // shifter