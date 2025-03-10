#include "pch.h"
#include "JoystickViewer.h"


BAKKESMOD_PLUGIN(JoystickViewer, "Joystick Viewer", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;


void JoystickViewer::onLoad()
{
	_globalCvarManager = cvarManager;
	LOG("Joystick Viewer Plugin loaded!");
	// !! Enable debug logging by setting DEBUG_LOG = true in logging.h !!
	//DEBUGLOG("JoystickViewer debug mode enabled");

	// LOG and DEBUGLOG use fmt format strings https://fmt.dev/latest/index.html
	//DEBUGLOG("1 = {}, 2 = {}, pi = {}, false != {}", "one", 2, 3.14, true);

	//cvarManager->registerNotifier("my_aweseome_notifier", [&](std::vector<std::string> args) {
	//	LOG("Hello notifier!");
	//}, "", 0);

	//auto cvar = cvarManager->registerCvar("template_cvar", "hello-cvar", "just a example of a cvar");
	//auto cvar2 = cvarManager->registerCvar("template_cvar2", "0", "just a example of a cvar with more settings", true, true, -10, true, 10 );

	auto cvar_jsv_enable = cvarManager->registerCvar("jsv_enable", "0", "Enable Joystick Viewer", true, true, 0, true, 1);

	auto cvar_jsv_show_current_pos = cvarManager->registerCvar("jsv_show_current_pos", "0", "actively show the current stick position", true, true, 0, true, 1);

	auto cvar_jsv_posx = cvarManager->registerCvar("jsv_posx", "100", "Joysick Viewer X position", true, true, 0, true, 10000);
	auto cvar_jsv_posy = cvarManager->registerCvar("jsv_posy", "100", "Joysick Viewer Y position", true, true, 0, true, 10000);
	auto cvar_jsv_size = cvarManager->registerCvar("jsv_size", "100", "Joysick Viewer Size", true, true, 1, true, 400);

	posx = cvar_jsv_posx.getIntValue();
	posy = cvar_jsv_posy.getIntValue();
	size = cvar_jsv_size.getIntValue();
	showCurrentPos = cvar_jsv_show_current_pos.getBoolValue();
	enable = cvar_jsv_enable.getBoolValue();

	
	auto cvar_jsv_history_length = cvarManager->registerCvar("jsv_history_length", "1.0", "# of seconds of inputs to cache for view", true, true, 0.1, true, 5);
	// RL Physics runs at 120hz (unless lower frame rate, but we'll assume 120hz), so 120 controller inputs per second to store
	inputHistoryLength = (int)(cvar_jsv_history_length.getFloatValue() * 120);
	if (inputHistoryLength % 2 != 0) { inputHistoryLength += 1; }
	inputHistory.resize(inputHistoryLength);
	preJumpHistory.resize(inputHistoryLength / 2);
	postJumpHistory.resize(inputHistoryLength / 2);

	// The max setting here will prevent it from ever disappearing until next button press
	auto cvar_jsv_history_duration = cvarManager->registerCvar("jsv_history_duration", "5", "# of seconds that the history will persist", true, true, 0.1, true, 20);
	historyDuration = (int)(cvar_jsv_history_duration.getFloatValue() * 120);

	gameWrapper->HookEventWithCaller<CarWrapper>("Function TAGame.Car_TA.SetVehicleInput", std::bind(&JoystickViewer::OnSetInput, this, std::placeholders::_1, std::placeholders::_2));
	gameWrapper->RegisterDrawable(std::bind(&JoystickViewer::Render, this, std::placeholders::_1));

	prevJump = 0;


	//cvar.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
	//	LOG("the cvar with name: {} changed", cvarName);
	//	LOG("the new value is: {}", newCvar.getStringValue());
	//});

	//cvar2.addOnValueChanged(std::bind(&JoystickViewer::YourPluginMethod, this, _1, _2));

	// enabled decleared in the header
	//enabled = std::make_shared<bool>(false);
	//cvarManager->registerCvar("TEMPLATE_Enabled", "0", "Enable the TEMPLATE plugin", true, true, 0, true, 1).bindTo(enabled);

	//cvarManager->registerNotifier("NOTIFIER", [this](std::vector<std::string> params){FUNCTION();}, "DESCRIPTION", PERMISSION_ALL);
	//cvarManager->registerCvar("CVAR", "DEFAULTVALUE", "DESCRIPTION", true, true, MINVAL, true, MAXVAL);//.bindTo(CVARVARIABLE);
	//gameWrapper->HookEvent("FUNCTIONNAME", std::bind(&TEMPLATE::FUNCTION, this));
	//gameWrapper->HookEventWithCallerPost<ActorWrapper>("FUNCTIONNAME", std::bind(&JoystickViewer::FUNCTION, this, _1, _2, _3));
	//gameWrapper->RegisterDrawable(bind(&TEMPLATE::Render, this, std::placeholders::_1));


	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", [this](std::string eventName) {
	//	LOG("Your hook got called and the ball went POOF");
	//});
	// You could also use std::bind here
	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", std::bind(&JoystickViewer::YourPluginMethod, this);
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

		if (postJumpHistory.size() < preJumpHistory.size()) {
			postJumpHistory.push_back(*ci);
		}
	}

	// if the jump button was pressed --------- note this might need to go somewhere else depending if we want each jump to trigger a new  recording
	if (!prevJump && ci->Jump == 1) {
		historyCountdown = historyDuration;

		// Copy the first half of the inputs to the pre-jump history
		std::copy(inputHistory.end() - inputHistory.size() / 2, inputHistory.end(), preJumpHistory.begin());
		postJumpHistory.clear(); // This initiates the next half of the inputs being saved
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
	canvas.SetColor(255, 255, 255, 100);
	canvas.SetPosition(Vector2({ posx - size / 2, posy - size / 2 }));
	canvas.DrawBox(Vector2({ size, size }));

	if (historyCountdown > 0) {
		

		// Render the cached values however you want to do that
		JoystickViewer::DrawInputArray(canvas, preJumpHistory, 0x0000FFFF, 150);
		if (postJumpHistory.size() > 0) {
			JoystickViewer::DrawInputArray(canvas, postJumpHistory, 0xFF0000FF, 150);
		}
	}

	// By default, show the current joystick position 
	else if (historyCountdown == 0 && showCurrentPos) {
		JoystickViewer::DrawInputArray(canvas, inputHistory, IM_COL32_WHITE, 30);
	}
}

void JoystickViewer::DrawInputArray(CanvasWrapper canvas, std::vector<ControllerInput> inputArray, uint32_t color, uint8_t end_opacity) {
	int iconSize = 6;

	int i = 0;
	Vector2 prevPos;
	for (auto it = inputArray.rbegin(); it != inputArray.rend(); ++it) {
		ControllerInput input = *it;

		Vector2 currentPos = { posx + (int)(size / 2 * input.Steer) - iconSize/2, posy + (int)(size / 2 * input.Pitch) - iconSize/2 };

		int mapped_opacity = (color & 0xFF) - static_cast<float>(i) / inputArray.size() * ((color & 0xFF) - end_opacity);

		canvas.SetColor((color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, mapped_opacity);
		canvas.SetPosition(currentPos);
		canvas.FillBox(Vector2({ iconSize,iconSize }));

		if (i > 0) {
			currentPos.X += iconSize / 2;
			currentPos.Y += iconSize / 2;
			canvas.DrawLine(currentPos, prevPos);
		}

		prevPos = currentPos;
		i++;
	}
}