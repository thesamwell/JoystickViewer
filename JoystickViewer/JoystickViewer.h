#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "PersistentStorage.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

// Helpful macros
#define CALC_INPUT_POSITION(input, posx, posy, size, iconSize) \
    (Vector2{posx + (int)(size / 2 * input.Steer) - iconSize / 2, posy + (int)(size / 2 * input.Pitch) - iconSize / 2})

#define LIN_TO_U32_COLOR(c) \
	((((int)(c.R) & 0xFF) << 24) | (((int)(c.G) & 0xFF) << 16) | (((int)(c.B) & 0xFF) << 8) | ((int)(c.A) & 0xFF))

#define U32_TO_LIN_COLOR(c) ((c >> 24)&0xFF), ((c >> 16)&0xFF), ((c>>8)&0xFF), (c&0xFF)

// Default Variable Values:
#define JSV_ENABLE_DEFAULT "1"
#define JSV_EN_TRAINING_DEFAULT "1"
#define JSV_EN_ONLINE_GAME_DEFAULT "1"
#define JSV_SHOW_CURRENT_POS_DEFAULT "1"
#define JSV_PAUSE_ON_JUMP_DEFAULT "1"
#define JSV_SHOW_GRID_DEFAULT "1"
#define JSV_POSX_DEFAULT "200"
#define JSV_POSY_DEFAULT "200"
#define JSV_SIZE_DEFAULT "180"
#define JSV_HISTORY_LENGTH_DEFAULT "0.186"
#define JSV_HISTORY_DURATION_DEFAULT "4.0"
#define JSV_COLOR_DEFAULT_START_DEFAULT "#FFFFFFFF"
#define JSV_COLOR_DEFAULT_END_DEFAULT "#FFFFFF11"
#define JSV_COLOR_PRE_START_DEFAULT "(0.000000, 255.000000, 195.000000, 255.000000)"
#define JSV_COLOR_PRE_END_DEFAULT "(0.000000, 0.000000, 255.000000, 255.000000)"
#define JSV_COLOR_POST_START_DEFAULT "(255.000000, 0.000000, 0.000000, 255.000000)"
#define JSV_COLOR_POST_END_DEFAULT "(255.000000, 232.500015, 0.000000, 255.000000)"
#define JSV_COLOR_OUTER_BOX_DEFAULT "(193.749985, 189.950974, 189.950974, 204.000000)"
#define JSV_COLOR_JUMP_ICON_DEFAULT "#FFFFFFFF"
#define JSV_SIZE_ICON_DEFAULT "5"
#define JSV_SIZE_JUMP_ICON_DEFAULT "9"
#define JSV_SIZE_LINE_DEFAULT "5"


class JoystickViewer: public BakkesMod::Plugin::BakkesModPlugin, public SettingsWindowBase
{
private:
	std::shared_ptr<PersistentStorage> persistentStorage;

	// History Variables
	int inputHistoryLength;
	int historyCountdown;
	int historyDuration;
	std::vector<ControllerInput> inputHistory;
	std::vector<ControllerInput> preJumpHistory;
	std::vector<ControllerInput> postJumpHistory;

	// Enables
	bool enable;
	bool en_training;
	bool en_online_game;
	bool showGrid;
	bool showCurrentPos;
	bool pauseOnJump;

	// Other Variables
	bool prevJump; // Last jump state for detecting rising edge
	Vector2 screen_size;
	
	// Positions
	int posx, posy;
	int size;

	// Colors 
	uint32_t default_start;
	uint32_t default_end;
	uint32_t pre_start;
	uint32_t pre_end;
	uint32_t post_start;
	uint32_t post_end;
	uint32_t outer_box_color;
	uint32_t jump_icon_color;

	// Sizes
	int icon_size;
	int jump_icon_size;
	float line_size;

	bool IsValidScene();
	void OnSetInput(CarWrapper cw, void* params);
	void StoreInput(ControllerInput* ci);
	void Render(CanvasWrapper canvas);
	void DrawInputArray(CanvasWrapper canvas, std::vector<ControllerInput> inputArray, uint32_t color_start, uint32_t color_end);
	uint32_t MapColors(uint32_t color_a, uint32_t color_b, int i, int max);
	void onLoad() override;
	void onUnload() override;
	void RenderSettings() override;
};
