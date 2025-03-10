#include "pch.h"
#include "JoystickViewer.h"

void JoystickViewer::RenderSettings() {
	ImGui::TextUnformatted("Joystick Viewer is pretty cool.");


	// Button test 
	if (ImGui::Button("Test Button")) {
		LOG("Button test has been tested button test test ttt");
	}

	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Activate test button");
	}


	// Enable Joystick View Checkbox
	CVarWrapper enableCvar = cvarManager->getCvar("jsv_enable");
	if (!enableCvar) { return; }
	enable = enableCvar.getBoolValue();
	if (ImGui::Checkbox("Enable Plugin", &enable)) {
		enableCvar.setValue(enable);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(enableCvar.getDescription().c_str());
	}

	// Joystick Show Current Pos Checkbox
	CVarWrapper scpCvar = cvarManager->getCvar("jsv_show_current_pos");
	if (!scpCvar) { return; }
	showCurrentPos = scpCvar.getBoolValue();
	if (ImGui::Checkbox("Show Current Joystick Pos", &showCurrentPos)) {
		scpCvar.setValue(showCurrentPos);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(scpCvar.getDescription().c_str());
	}


	// X pos slider
	CVarWrapper posxCvar = cvarManager->getCvar("jsv_posx");
	if (!posxCvar) { return; }
	posx  = posxCvar.getIntValue();
	if (ImGui::SliderInt("X Position", &posx, posxCvar.GetMinimum(), posxCvar.GetMaximum())) {
		posxCvar.setValue(posx);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(posxCvar.getDescription().c_str());
	}

	// Y pos slider
	CVarWrapper posyCvar = cvarManager->getCvar("jsv_posy");
	if (!posyCvar) { return; }
	posy = posyCvar.getIntValue();
	if (ImGui::SliderInt("Y Position", &posy, posyCvar.GetMinimum(), posyCvar.GetMaximum())) {
		posyCvar.setValue(posy);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(posyCvar.getDescription().c_str());
	}

	// Size slider
	CVarWrapper sizeCvar = cvarManager->getCvar("jsv_size");
	if (!sizeCvar) { return; }
	size = sizeCvar.getIntValue();
	if (ImGui::SliderInt("Size", &size, sizeCvar.GetMinimum(), sizeCvar.GetMaximum())) {
		sizeCvar.setValue(size);
	}
	if (ImGui::IsItemHovered()) {;
		ImGui::SetTooltip(sizeCvar.getDescription().c_str());
	}

	// Input History Length slider
	CVarWrapper historylengthCvar = cvarManager->getCvar("jsv_history_length");
	if (!historylengthCvar) { return; }
	float history_length = historylengthCvar.getFloatValue();
	if (ImGui::SliderFloat("History Length", &history_length, historylengthCvar.GetMinimum(), historylengthCvar.GetMaximum())) {
		historylengthCvar.setValue(history_length);
		// RL Physics runs at 120hz (unless lower frame rate, but we'll assume 120hz), so 120 controller inputs per second to store
		inputHistoryLength = (int)(history_length * 120);
		if (inputHistoryLength % 2 != 0) { inputHistoryLength += 1; }
		inputHistory.resize(inputHistoryLength);
		preJumpHistory.resize(inputHistoryLength / 2);
		postJumpHistory.resize(inputHistoryLength / 2);
	}
	if(ImGui::IsItemHovered()) {
		ImGui::SetTooltip(historylengthCvar.getDescription().c_str());
	}

	// Input History Duration slider
	CVarWrapper historydurationCvar = cvarManager->getCvar("jsv_history_duration");
	if (!historydurationCvar) { return; }
	float history_duration = historydurationCvar.getFloatValue();
	if (ImGui::SliderFloat("History Duration", &history_duration, historydurationCvar.GetMinimum(), historydurationCvar.GetMaximum())) {
		historydurationCvar.setValue(history_duration);
		// RL Physics runs at 120hz (unless lower frame rate, but we'll assume 120hz)
		historyDuration = (int)(history_duration * 120);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(historydurationCvar.getDescription().c_str());
	}


}