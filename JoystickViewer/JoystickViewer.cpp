#include "pch.h"
#include "JoystickViewer.h"

BAKKESMOD_PLUGIN(JoystickViewer, "Joystick Viewer", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void JoystickViewer::onLoad()
{
	_globalCvarManager = cvarManager;
	LOG("Joystick Viewer Plugin loaded!");

	if (!gameWrapper) {
		screen_size = Vector2{ 4096,2160 };
	}
	else {
		screen_size = gameWrapper->GetScreenSize();
	}
	
	cvarManager->registerCvar("jsv_enable", JSV_ENABLE_DEFAULT, "Enable Joystick Viewer", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		enable = cvar.getBoolValue();
			});

	cvarManager->registerCvar("jsv_show_current_pos", JSV_SHOW_CURRENT_POS_DEFAULT, "actively show the current stick position", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		showCurrentPos = cvar.getBoolValue();
			});

	cvarManager->registerCvar("jsv_posx", JSV_POSX_DEFAULT, "Joysick Viewer X position", true, true, 0, true, 10000)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		posx = cvar.getIntValue();
			});
	cvarManager->registerCvar("jsv_posy", JSV_POSY_DEFAULT, "Joysick Viewer Y position", true, true, 0, true, 10000)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		posy = cvar.getIntValue();
			});
	cvarManager->registerCvar("jsv_size", JSV_SIZE_DEFAULT, "Joysick Viewer Size", true, true, 1, true, 400)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		size = cvar.getIntValue();
			});

	cvarManager->registerCvar("jsv_history_length", JSV_HISTORY_LENGTH_DEFAULT, "# of seconds of inputs to cache for view", true, true, 0.1, true, 5)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		// RL Physics runs at 120hz (unless lower frame rate, but we'll assume 120hz), so 120 controller inputs per second to store
		inputHistoryLength = (int)(cvar.getFloatValue() * 120);
		inputHistory.resize(inputHistoryLength);
		preJumpHistory.resize(inputHistoryLength);
		postJumpHistory.resize(inputHistoryLength);
			});
	
	cvarManager->registerCvar("jsv_history_duration", JSV_HISTORY_DURATION_DEFAULT, "# of seconds that the history will persist", true, true, 0.1, true, 20)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		historyDuration = (int)(cvar.getFloatValue() * 120);
			});

	// Colors
	cvarManager->registerCvar("jsv_color_default_start", JSV_COLOR_DEFAULT_START_DEFAULT, "Starting color of default view position")
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		default_start = LIN_TO_U32_COLOR(cvar.getColorValue());
			});

	cvarManager->registerCvar("jsv_color_default_end", JSV_COLOR_DEFAULT_END_DEFAULT, "Ending color of default view position")
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		default_end = LIN_TO_U32_COLOR(cvar.getColorValue());
			});

	cvarManager->registerCvar("jsv_color_pre_start", JSV_COLOR_PRE_START_DEFAULT, "Starting color of pre-jump view position")
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		pre_start = LIN_TO_U32_COLOR(cvar.getColorValue());
			});

	cvarManager->registerCvar("jsv_color_pre_end", JSV_COLOR_PRE_END_DEFAULT, "Ending color of pre-jump view position")
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		pre_end = LIN_TO_U32_COLOR(cvar.getColorValue());
			});

	cvarManager->registerCvar("jsv_color_post_start", JSV_COLOR_POST_START_DEFAULT, "Starting color of post-jump view position")
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		post_start = LIN_TO_U32_COLOR(cvar.getColorValue());
			});

	cvarManager->registerCvar("jsv_color_post_end", JSV_COLOR_POST_END_DEFAULT, "Ending color of post-jump view position")
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		post_end = LIN_TO_U32_COLOR(cvar.getColorValue());
			});

	cvarManager->registerCvar("jsv_color_outer_box", JSV_COLOR_OUTER_BOX_DEFAULT, "Color of outer box")
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		outer_box_color = LIN_TO_U32_COLOR(cvar.getColorValue());
			});

	cvarManager->registerCvar("jsv_color_jump_icon", JSV_COLOR_JUMP_ICON_DEFAULT, "Color of icon showing position when jump happened")
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		jump_icon_color = LIN_TO_U32_COLOR(cvar.getColorValue());
			});


	// Sizes / thicknesses
	cvarManager->registerCvar("jsv_size_icon", JSV_SIZE_ICON_DEFAULT, "Size of joystick coordinate icons", true, true, 2, true, 30)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		icon_size = cvar.getIntValue();
		icon_size += (icon_size % 2 != 0) ? 1 : 0; 
			});

	cvarManager->registerCvar("jsv_size_jump_icon", JSV_SIZE_JUMP_ICON_DEFAULT, "Size of joystick coordinate jump icon", true, true, 2, true, 30)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		jump_icon_size = cvar.getIntValue();
		jump_icon_size += (jump_icon_size % 2 != 0) ? 1 : 0;
			});

	cvarManager->registerCvar("jsv_size_line", JSV_SIZE_LINE_DEFAULT, "Size of joystick coordinate lines", true, true, 1.0, true, 30.0)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		line_size = cvar.getFloatValue();
			});

	// Manually initialize all class vars to make sure they are set.
	enable = cvarManager->getCvar("jsv_enable").getBoolValue();
	showCurrentPos = cvarManager->getCvar("jsv_show_current_pos").getBoolValue();
	posx = cvarManager->getCvar("jsv_posx").getIntValue();
	posy = cvarManager->getCvar("jsv_posy").getIntValue();
	size = cvarManager->getCvar("jsv_size").getIntValue();
	inputHistoryLength = (int)(cvarManager->getCvar("jsv_history_length").getFloatValue() * 120);
	inputHistory.resize(inputHistoryLength);
	postJumpHistory.resize(inputHistoryLength);
	historyDuration = (int)(cvarManager->getCvar("jsv_history_duration").getFloatValue() * 120);

	default_start = LIN_TO_U32_COLOR(cvarManager->getCvar("jsv_color_default_start").getColorValue());
	default_end = LIN_TO_U32_COLOR(cvarManager->getCvar("jsv_color_default_end").getColorValue());
	pre_start = LIN_TO_U32_COLOR(cvarManager->getCvar("jsv_color_pre_start").getColorValue());
	pre_end = LIN_TO_U32_COLOR(cvarManager->getCvar("jsv_color_pre_end").getColorValue());
	post_start = LIN_TO_U32_COLOR(cvarManager->getCvar("jsv_color_post_start").getColorValue());
	post_end = LIN_TO_U32_COLOR(cvarManager->getCvar("jsv_color_post_end").getColorValue());
	outer_box_color = LIN_TO_U32_COLOR(cvarManager->getCvar("jsv_color_outer_box").getColorValue());
	jump_icon_color = LIN_TO_U32_COLOR(cvarManager->getCvar("jsv_color_jump_icon").getColorValue());
	
	icon_size = cvarManager->getCvar("jsv_size_icon").getIntValue();
	jump_icon_size = cvarManager->getCvar("jsv_size_jump_icon").getIntValue();
	line_size = cvarManager->getCvar("jsv_size_line").getFloatValue();


	gameWrapper->HookEventWithCaller<CarWrapper>("Function TAGame.Car_TA.SetVehicleInput", std::bind(&JoystickViewer::OnSetInput, this, std::placeholders::_1, std::placeholders::_2));
	gameWrapper->RegisterDrawable(std::bind(&JoystickViewer::Render, this, std::placeholders::_1));

	prevJump = 0;
}

void JoystickViewer::onUnload() {
	LOG("Joystick Viewer Plugin unloading.");
	gameWrapper->UnregisterDrawables();
}

void JoystickViewer::OnSetInput(CarWrapper cw, void* params) {
	if (!gameWrapper->IsInFreeplay()) {
		return;
	}

	ControllerInput* ci = (ControllerInput*)params;
	if (enable) {
		// Update the input history with the new inputs
		if (inputHistory.size() >= inputHistoryLength) {
			int numToRemove = 1 + inputHistory.size() - inputHistoryLength;
			inputHistory.erase(inputHistory.begin(), inputHistory.begin() + numToRemove);
		}
		inputHistory.push_back(*ci);
		

		if (postJumpHistory.size() < inputHistory.size()) {
			postJumpHistory.push_back(*ci);
		}
	}

	// if rising edge of jump button
	if (!prevJump && ci->Jump == 1) {
		historyCountdown = historyDuration;

		// Shallow copy input history
		preJumpHistory = inputHistory;
		postJumpHistory.clear();
		postJumpHistory.push_back(*ci); // This makes sure post has a line connecting to the jump coordinate
	}
	prevJump = ci->Jump;

	if (historyCountdown > 0) { historyCountdown--; }
}

void JoystickViewer::Render(CanvasWrapper canvas) {
	if (!gameWrapper->IsInFreeplay() ||
		!enable) {
		return;
	}

    // Always render the box
	canvas.SetColor(U32_TO_LIN_COLOR(outer_box_color));
	canvas.SetPosition(Vector2({ posx - size / 2, posy - size / 2 }));
	canvas.DrawBox(Vector2{ size, size });

	if (historyCountdown > 0) {
		// Render the cached values
		JoystickViewer::DrawInputArray(canvas, preJumpHistory, pre_start, pre_end);
		if (postJumpHistory.size() > 0) {
			JoystickViewer::DrawInputArray(canvas, postJumpHistory, post_start, post_end);
		}

		// Draw the jump position above everything else
		canvas.SetColor(U32_TO_LIN_COLOR(jump_icon_color));
		canvas.SetPosition(CALC_INPUT_POSITION(preJumpHistory.back(), posx, posy, size, jump_icon_size));
		canvas.FillBox(Vector2{ jump_icon_size, jump_icon_size });
	}

	// By default, show the current joystick position 
	else if (historyCountdown == 0 && showCurrentPos) {
		JoystickViewer::DrawInputArray(canvas, inputHistory, default_start, default_end);
	}
}

void JoystickViewer::DrawInputArray(CanvasWrapper canvas, std::vector<ControllerInput> inputArray, uint32_t color_start, uint32_t color_end) {
	int i = 0;
	Vector2 prevPos;
	for (auto it = inputArray.rbegin(); it != inputArray.rend(); ++it) {
		ControllerInput input = *it;

		Vector2 currentPos = CALC_INPUT_POSITION(input, posx, posy, size, icon_size);

		canvas.SetColor(U32_TO_LIN_COLOR(JoystickViewer::MapColors(color_start, color_end, i, inputArray.size())));
		canvas.SetPosition(currentPos);
		canvas.FillBox(Vector2({ icon_size,icon_size }));

		if (i > 0) {
			currentPos.X += icon_size / 2;
			currentPos.Y += icon_size / 2 - static_cast<int>(line_size/2); // Make sure the line is centered with the icon
			if (std::abs(currentPos.X - prevPos.X) >= icon_size || std::abs(currentPos.Y - prevPos.Y) >= icon_size) {
				canvas.DrawLine(currentPos, prevPos, line_size);
			}
		}

		prevPos = currentPos;
		i++;
	}
}

uint32_t JoystickViewer::MapColors(uint32_t color_a, uint32_t color_b, int i, int max) {
	// Clamp i
	i = (i < 0) ? 0 : (i > max) ? max : i;
	float ratio = static_cast<float>(i) / static_cast<float>(max);
	uint32_t res = 0;
	uint8_t ca, cb;
	for (int j = 0; j < 4; j++) {
		ca = (color_a >> 8 * j) & 0xFF;
		cb = (color_b >> 8 * j) & 0xFF;
		res |= static_cast<uint32_t>(ca + (cb - ca) * ratio) << 8*j;
	}
	return res;
}