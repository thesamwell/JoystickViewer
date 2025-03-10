# JoystickViewer

This plugin provides additional features and customizability to the popular "JoystickSelfCheck" Bakkesmod plugin for Rocket League:

Features:
- Capture and display a freeze-frame of joystick inputs immediately before and after a jump.
   - This is useful for improving flip cancels, stalls, and learning how joystick movements affect the car.
- Customize the location, size, color, gradient, and duration of the displayed inputs
- View the exact joystick inputs you make, checking drift, deadzone, etc.

I was inspired to make this after failing maktufs and mustys hundreds of times and wanted to see exactly why :>

Here's some examples: 

![Maktuf Reset](./example1.png)
- The blue line represents inputs before the jump. It gets brighter as the time nears when the jump happened.
- The pink line represents inputs after the jump. It gets more red as time advances further after the jump happened.
- The white square represents the joystick input at the exact moment the jump registered.

![Single Wall Dash](./example2.png)
- Driving forward on the right side wall. Darker blue indicates a turn down the wall before the jump.
- Lighter blue indicates turning back up the wall right before the jump, as well as a moderate forward tilt.
- Pink shows that right after the jump, the joystick is tilted slightly further up before returning to neutral as time progresses.