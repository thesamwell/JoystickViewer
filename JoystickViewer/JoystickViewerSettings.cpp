#include "pch.h"
#include "JoystickViewer.h"

void JoystickViewer::RenderSettings() {
	// Enable Joystick View Checkbox
	CVarWrapper enableCvar = cvarManager->getCvar("jsv_enable");
	if (!enableCvar) { return; }
	bool cv_enable = enableCvar.getBoolValue();
	if (ImGui::Checkbox("Enable Plugin", &cv_enable)) {
		enableCvar.setValue(cv_enable);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(enableCvar.getDescription().c_str());
	}

	// Joystick Show Current Pos Checkbox
	CVarWrapper scpCvar = cvarManager->getCvar("jsv_show_current_pos");
	if (!scpCvar) { return; }
	bool cv_showCurrentPos = scpCvar.getBoolValue();
	if (ImGui::Checkbox("Show Current Joystick Position", &cv_showCurrentPos)) {
		scpCvar.setValue(cv_showCurrentPos);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(scpCvar.getDescription().c_str());
	}

	ImGui::Separator();
	ImGui::Text("Coordinates and size of view:");

	// X pos slider
	CVarWrapper posxCvar = cvarManager->getCvar("jsv_posx");
	if (!posxCvar) { return; }
	int cv_posx  = posxCvar.getIntValue();
	if (ImGui::SliderInt("X Position", &cv_posx, posxCvar.GetMinimum(), screen_size.X)) {
		posxCvar.setValue(cv_posx);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(posxCvar.getDescription().c_str());
	}

	// Y pos slider
	CVarWrapper posyCvar = cvarManager->getCvar("jsv_posy");
	if (!posyCvar) { return; }
	int cv_posy = posyCvar.getIntValue();
	if (ImGui::SliderInt("Y Position", &cv_posy, posyCvar.GetMinimum(), screen_size.Y)) {
		posyCvar.setValue(cv_posy);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(posyCvar.getDescription().c_str());
	}

	// Size slider
	CVarWrapper sizeCvar = cvarManager->getCvar("jsv_size");
	if (!sizeCvar) { return; }
	int cv_size = sizeCvar.getIntValue();
	if (ImGui::SliderInt("Size", &cv_size, sizeCvar.GetMinimum(), sizeCvar.GetMaximum())) {
		sizeCvar.setValue(cv_size);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(sizeCvar.getDescription().c_str());
	}

	// Add Reset button
	if (ImGui::Button("Reset Positions to Default")) {
		posxCvar.setValue(JSV_POSX_DEFAULT);
		posyCvar.setValue(JSV_POSY_DEFAULT);
		sizeCvar.setValue(JSV_SIZE_DEFAULT);
	}


	ImGui::Separator();
	ImGui::Text("History Length: Denotes how long the trail of inputs is in the view by number of seconds");

	// Input History Length slider
	CVarWrapper historylengthCvar = cvarManager->getCvar("jsv_history_length");
	if (!historylengthCvar) { return; }
	float history_length = historylengthCvar.getFloatValue();
	if (ImGui::SliderFloat("History Length", &history_length, historylengthCvar.GetMinimum(), historylengthCvar.GetMaximum())) {
		historylengthCvar.setValue(history_length);
	}
	if(ImGui::IsItemHovered()) {
		ImGui::SetTooltip(historylengthCvar.getDescription().c_str());
	}

	ImGui::Text("History Duration: Denotes for how many seconds after a jump input that the input history before/after that jump be displayed before it disappears.");

	// Input History Duration slider
	CVarWrapper historydurationCvar = cvarManager->getCvar("jsv_history_duration");
	if (!historydurationCvar) { return; }
	float history_duration = historydurationCvar.getFloatValue();
	if (ImGui::SliderFloat("History Duration", &history_duration, historydurationCvar.GetMinimum(), historydurationCvar.GetMaximum())) {
		historydurationCvar.setValue(history_duration);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(historydurationCvar.getDescription().c_str());
	}

	// Add Reset button
	if (ImGui::Button("Reset Durations to Default")) {
		historylengthCvar.setValue(JSV_HISTORY_LENGTH_DEFAULT);
		historydurationCvar.setValue(JSV_HISTORY_DURATION_DEFAULT);
	}

	ImGui::Separator();
	ImGui::Text("Sizes / Thicknesses:");

	ImGui::Text("Size of the icons (the squares that show each joystick coordinate registered by the game)");

	// Sizes / thicknesses
	// Size slider
	CVarWrapper iconSizeCvar = cvarManager->getCvar("jsv_size_icon");
	if (!iconSizeCvar) { return; }
	int cv_sizeIcon = iconSizeCvar.getIntValue();
	if (ImGui::SliderInt("Icon Size", &cv_sizeIcon, iconSizeCvar.GetMinimum(), iconSizeCvar.GetMaximum())) {
		iconSizeCvar.setValue(cv_sizeIcon);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(iconSizeCvar.getDescription().c_str());
	}

	ImGui::Text("Size of the icon noting the joystick position specifically when the jump occurred");

	CVarWrapper jumpIconSizeCvar = cvarManager->getCvar("jsv_size_jump_icon");
	if (!jumpIconSizeCvar) { return; }
	int cv_sizeJumpIcon = jumpIconSizeCvar.getIntValue();
	if (ImGui::SliderInt("Jump Icon Size", &cv_sizeJumpIcon, jumpIconSizeCvar.GetMinimum(), jumpIconSizeCvar.GetMaximum())) {
		jumpIconSizeCvar.setValue(cv_sizeJumpIcon);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(jumpIconSizeCvar.getDescription().c_str());
	}

	ImGui::Text("Thickness of the lines connecting the icons");

	CVarWrapper lineSizeCvar = cvarManager->getCvar("jsv_size_line");
	if (!lineSizeCvar) { return; }
	float cv_sizeLine = lineSizeCvar.getFloatValue();
	if (ImGui::SliderFloat("Line Size", &cv_sizeLine, lineSizeCvar.GetMinimum(), lineSizeCvar.GetMaximum())) {
		lineSizeCvar.setValue(cv_sizeLine);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip(lineSizeCvar.getDescription().c_str());
	}

	if (ImGui::Button("Reset Sizes to Defaults")) {
		iconSizeCvar.setValue(JSV_SIZE_ICON_DEFAULT);
		jumpIconSizeCvar.setValue(JSV_SIZE_JUMP_ICON_DEFAULT);
		lineSizeCvar.setValue(JSV_SIZE_LINE_DEFAULT);
	}

	ImGui::Separator();
	ImGui::Text("Colors:");

	ImGui::Text("Each color has a start and end. Start refers to the most recent input of that group, and end is the oldest input of that group.");
	ImGui::Text("The colors along that input chain are mapped linearly from the start to end color. Both the connecting lines and the icons get the same color.");
	ImGui::Text("\nDefault: This sets the color of the view that shows joystick inputs if the jump button is not pressed. \nThis view is toggled by the \"Show Current Joystick Position\" checkbox above.");

	// Colors
	// For "jsv_color_default_start"
	CVarWrapper defaultStartColorCvar = cvarManager->getCvar("jsv_color_default_start");
	if (!defaultStartColorCvar) { return; }
	LinearColor defaultStartColor = defaultStartColorCvar.getColorValue() / 255;
	if (ImGui::ColorEdit4("Default Start Color", &defaultStartColor.R)) {
		defaultStartColorCvar.setValue(defaultStartColor * 255);
	}

	// For "jsv_color_default_end"
	CVarWrapper defaultEndColorCvar = cvarManager->getCvar("jsv_color_default_end");
	if (!defaultEndColorCvar) { return; }
	LinearColor defaultEndColor = defaultEndColorCvar.getColorValue() / 255;
	if (ImGui::ColorEdit4("Default End Color", &defaultEndColor.R)) {
		defaultEndColorCvar.setValue(defaultEndColor * 255);
	}

	ImGui::Text("Pre-Jump: For the input chain that happened before the jump button was pressed");

	// For "jsv_color_pre_start"
	CVarWrapper preStartColorCvar = cvarManager->getCvar("jsv_color_pre_start");
	if (!preStartColorCvar) { return; }
	LinearColor preStartColor = preStartColorCvar.getColorValue() / 255;
	if (ImGui::ColorEdit4("Pre Start Color", &preStartColor.R)) {
		preStartColorCvar.setValue(preStartColor * 255);
	}

	// For "jsv_color_pre_end"
	CVarWrapper preEndColorCvar = cvarManager->getCvar("jsv_color_pre_end");
	if (!preEndColorCvar) { return; }
	LinearColor preEndColor = preEndColorCvar.getColorValue() / 255;
	if (ImGui::ColorEdit4("Pre End Color", &preEndColor.R)) {
		preEndColorCvar.setValue(preEndColor * 255);
	}

	ImGui::Text("Post-Jump: For the input chain that happened after the jump button was pressed");

	// For "jsv_color_post_start"
	CVarWrapper postStartColorCvar = cvarManager->getCvar("jsv_color_post_start");
	if (!postStartColorCvar) { return; }
	LinearColor postStartColor = postStartColorCvar.getColorValue() / 255;
	if (ImGui::ColorEdit4("Post Start Color", &postStartColor.R)) {
		postStartColorCvar.setValue(postStartColor * 255);
	}

	// For "jsv_color_post_end"
	CVarWrapper postEndColorCvar = cvarManager->getCvar("jsv_color_post_end");
	if (!postEndColorCvar) { return; }
	LinearColor postEndColor = postEndColorCvar.getColorValue() / 255;
	if (ImGui::ColorEdit4("Post End Color", &postEndColor.R)) {
		postEndColorCvar.setValue(postEndColor * 255);
	}

	ImGui::Text("Outer box: The bounding box for the view");

	// For "jsv_color_outer_box"
	CVarWrapper outerBoxColorCvar = cvarManager->getCvar("jsv_color_outer_box");
	if (!outerBoxColorCvar) { return; }
	LinearColor outerBoxColor = outerBoxColorCvar.getColorValue() / 255;
	if (ImGui::ColorEdit4("Outer Box Color", &outerBoxColor.R)) {
		outerBoxColorCvar.setValue(outerBoxColor * 255);
	}

	ImGui::Text("Jump Icon: The icon showing the joystick position when the jump occurred");

	// For "jsv_color_jump_icon"
	CVarWrapper jumpIconColorCvar = cvarManager->getCvar("jsv_color_jump_icon");
	if (!jumpIconColorCvar) { return; }
	LinearColor jumpIconColor = jumpIconColorCvar.getColorValue() / 255;
	if (ImGui::ColorEdit4("Jump Icon Color", &jumpIconColor.R)) {
		jumpIconColorCvar.setValue(jumpIconColor * 255);
	}
	
	if (ImGui::Button("Reset Colors to Defaults")) {
		// Reset each color to its default value
		defaultStartColorCvar.setValue(JSV_COLOR_DEFAULT_START_DEFAULT);
		defaultEndColorCvar.setValue(JSV_COLOR_DEFAULT_END_DEFAULT);

		preStartColorCvar.setValue(JSV_COLOR_PRE_START_DEFAULT);
		preEndColorCvar.setValue(JSV_COLOR_PRE_END_DEFAULT);

		postStartColorCvar.setValue(JSV_COLOR_POST_START_DEFAULT);
		postEndColorCvar.setValue(JSV_COLOR_POST_END_DEFAULT);

		outerBoxColorCvar.setValue(JSV_COLOR_OUTER_BOX_DEFAULT);
		jumpIconColorCvar.setValue(JSV_COLOR_JUMP_ICON_DEFAULT);
	}
}