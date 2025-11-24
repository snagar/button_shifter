# Button Shifter Plugin

> Written by Saar N.
> Version v0.2

Thank you for trying the **Button Shifter** plugin.

The purpose of this plugin is to help you make better use of your joystick buttons by allowing the same physical button to perform different actions.

This is done by combining **“custom shift buttons”** with **“custom action buttons"**, all of which you configure in a plain-text configuration file.

As of this release, the plugin supports up to **6 (six)** shift buttons, each defining a separate set of custom commands. Within each set, you can define up to **10 (ten)** custom commands.

As of v0.2, the plugin will enable the first config file in the list (ordered by file name), and the first "set."
Make sure that your "favorite" or "default" configuration file will be sorted as the first one in the list.

### Donation

If you find this plugin useful, please consider [donating via this link](https://www.paypal.com/donate?token=b7oey_0CJMomwEojP1E6D0EV94udhoc79YHpYXPlkSdDICgWmwxRloH4QsXeKd39m1qvvBnZx50eLqSh).
Thank you!

---

## Installation

Extract the compressed plugin file into the "plugin" folder in X-Plane.
> "{X-Plane}/Resources/plugins"

---

## Restrictions

For each *custom command* line, you may assign one or more buttons. However, remember that these same buttons will trigger the same command line in **every** set — so map your buttons wisely.

---

## Do or Don’t

The main purpose of this plugin is to allow you to reuse special joystick buttons due to their scarcity.

The best practice is to map your buttons using X-Plane’s default button-mapping screen to the intended command.
However, if you need the same button to perform two different actions, this plugin may be helpful.
For example, a *spring-loaded* button could be used to change "heading" or "elevation" values.

---

# Files

Besides the plugin’s binary files, you will need to create **configuration files** with the extension `.shift`, which the plugin will automatically load.

A sample configuration file is included.
You may create files based on your peripheral type or aircraft to easily distinguish their usage.

**Examples:**

> moza_mtq.shift

> FF777.shift

---

## Config File Syntax

The format for `.shift` files is as follows:

```
#N: represents a set number.  
Any line after the "#" belongs to that set group.
```

* You can define up to **6 sets**: `#1` … `#6`.
* You can define up to **10 lines** per set.
* Each line is split using the pipe (`|`) character.
* Attributes appear in pairs: `{attribute}={value}`.

Each set (`#`) should be assigned in the **Keyboard** or **Joystick** command-binding screen in X-Plane.
A button bound to a *custom command number* will always trigger the corresponding line across all sets.

### How it appears in X-Plane:

**Example 1 — Binding shift buttons:**
`#1 = "button_shifter/step_1_map_shift_buttons/shift_N"`

**Example 2 — Binding a custom command to a button:**
`button_shifter/step_2_map_a_button_to_a_command/command_0` (Custom Command 0)

Every command line should include a button type: **hold** or **tick**.
* *tick* means a one-time press (default).
* If you want to set a command as "hold", make sure that the button you will use, can support it. Example: a knob can't support a hold type.

**Command line numbering starts at “0” and goes up to “9”.**

### Example from config01.shift:

```
;This is an example
#1|name=HDG Button  
cmd=sim/autopilot/heading_up    |button_type=hold  
cmd=sim/autopilot/heading_down  |button_type=hold  
cmd=sim/autopilot/heading_sync  |button_type=tick
```

In this example
* [ ] set number **#1** is defined.
* [ ] The `name` attribute is optional and will appear in the plugin menu.
* [ ] Each line after `#1` represents a custom command.
* [ ] The plugin automatically trims whitespace in each line and value.
* [ ] Any line that begins with `;` is ignored—use this for comments.

---

## Usage Example
### Setup your buttons / keys
* [ ] Start X-Plane.
* [ ] Open **Settings → Joystick**.
* [ ] Choose one or more *shift buttons* to switch between sets.
  * [ ] Look for commands that start with `button_shifter` (you do not need to map all shift options).

* [ ] Map the buttons that you want to behave differently depending on the shift state. Don’t overdo it—this is typically useful for only one or two special buttons.
* [ ] Write down the command numbers you assigned. Remember: **command 0 = first line in the set**, and so on.
* [ ] You do **not** need to map all commands, map only the ones you need.


* [ ] Create your own `.shift` file and define what each command line should do. Ensure the *command numbers* match the *line numbers* in each set.

---

## Summary

I hope this plugin helps you better tailor your joystick buttons to your specific needs.
Enjoy!

Saar
