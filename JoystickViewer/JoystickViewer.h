#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class JoystickViewer: public BakkesMod::Plugin::BakkesModPlugin
	,public SettingsWindowBase // Uncomment if you wanna render your own tab in the settings menu
	//,public PluginWindowBase // Uncomment if you want to render your own plugin window
{

	//std::shared_ptr<bool> enabled;

	//Boilerplate
	void onLoad() override;
	void onUnload() override; // Uncomment and implement if you need a unload method

private:
	std::vector<ControllerInput> inputHistory;
	std::vector<ControllerInput> preJumpHistory;
	std::vector<ControllerInput> postJumpHistory;
	int inputHistoryLength;

	int posx, posy;
	int size;
	bool showCurrentPos;
	bool enable;
	int historyCountdown;
	int historyDuration;
	bool prevJump;

	void DrawInputArray(CanvasWrapper canvas, std::vector<ControllerInput> inputArray, uint32_t color, uint8_t end_opacity);

public:
	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
	//void RenderWindow() override; // Uncomment if you want to render your own plugin window
	void OnSetInput(CarWrapper cw, void* params);
	void Render(CanvasWrapper canvas);
};
