# Button Shifter Plugin

> Written By Saar N.
> Version v0.1

Thank you for trying the "**button shifter**" plugin.
The main idea behind this plugin is to allow you to better utilize your joystick buttons. The idea is to allow you to use the same button to do different actions, based on a combination of "custom set" buttons, with "custom action" buttons, all of which you will have to custom map in a configuration file.

The plugin, as of this writing, can handle up 6 (six) "shift" buttons that define a set of custom commands. You can configure up to 10 (ten) custom commands in each set.

Donation:
-
If you find this plugin useful, do consider [donating in the following link](https://www.paypal.com/donate?token=b7oey_0CJMomwEojP1E6D0EV94udhoc79YHpYXPlkSdDICgWmwxRloH4QsXeKd39m1qvvBnZx50eLqSh)
Thanks



## Restrictions
For each "custom command" line, you can assign one or more buttons, but do remember that those same buttons will fire the same command line in a different set, so map your buttons wisely.

## Do or Don't
The main use of this plugin is to allow you to reuse special joystick buttons that you might not have the option otherwise to to the lack of other free buttons.
Best practice would be to map your buttons using X-Plane mapping screen. But, if you have a use for the same button for two different commands, like: *Spring-Loaded* buttons, then this plugin might be of use to you.

# Files

Except for the plugin binary files, you should create "config" files with extension ".shift", so the plugin will read them.
The plugin has one configuration file as an example.
You could create files after your peripheral type or plane to easily distinguish their usage.

__Example:__
> moza_mtq.shift

> FF777.shift

## Config File Syntax

The format for the ".shift" files is:

    #N: represent a set number.  
    Any line after the "#" sign, is part of that set group.

You can define up to "6" sets: "#1".."#6".
You can define up to "10" lines in a set.
We split the line using the pipe "|" character.
Attributes comes in pairs: "{attrib}={value}"

Each set ("#") should be assigned in the "***keyboard***" or "***joystick***" command binding screen, in x-plane.
A button that was bind to a "custom command number", will always fire that line in any set.

How it looks in x-plane command screen:
-
**Example 1** - Binding to a group of buttons will be represented as:  
#1 = "button_shift/step_1_map_shift_buttons/shift_N"

**Example 2** - Binding a "Custom Command" line to a button:  
"button_shifter/step_2_map_a_button_to_a_command/command_0" (Custom Command 0)

Every command line should have a button attribute type: "*hold*" or "*tick*".
The "tick" type, means one time ("tick" is the default).

***Command Lines numbering starts with "0" up to "9".***

Here is an example from the config01.shift file:

    ;This is an example
    #1|name=HDG Button  
    cmd=sim/autopilot/heading_up    |button_type=hold  
    cmd=sim/autopilot/heading_down  |button_type=hold  
    cmd=sim/autopilot/heading_sync  |button_type=tick
In this example we defined "set" number "1". The "name" attribute is optional, and it will be displayed in the plugins menu.

- [ ] Each line after the "#1" represent a custom command.
- [ ] The plugin automatically trims each line, and each split value.
- [ ] Any line or value that starts with ";" is being ignored. Use it to write a comment.

## Usage Example
- [ ] Start X-Plane.
- [ ] Open the settings screen and navigate to the Joystick tab.
- [ ] Pick one or more "shift" button to change the set.
    - [ ] Search for commands that starts with: "button_shifter" (you don't need to map all "shift" options).
- [ ] Map the buttons that you would like to have a different usage, based on the shift state. Don't overdo it, it could have been used for only one special button on your joystick.
- [ ] Write down the "command" number you mapped for each button, remember that "command 0" represents the first line in the "set" of commands.
- [ ] Remember: you do not need to map all commands, only the one you really need.
- [ ] Now you can create your own "config.shift" file with the commands that you would like to use for each "command line". Make sure to sync between the "command" number and the "line" in the set of commands.

## Summary
I hope that the plugin will allow you to better tailor your joystick buttons to your needs.
Enjoy.

Saar