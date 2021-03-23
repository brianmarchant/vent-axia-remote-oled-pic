Vent-Axia Remote : OLED-PIC
===========================

This is replacement software (firmware) for the Vent-Axia Wired Remote Control (443283) that allows the stock LCD display to be replaced by a nicer OLED unit.

### Why would you want this?

* If you don't like the crappy LCD display and want to replace it with a nice WINSTAR-based 16x2 OLED display, it probably won't work with the existing firmware. You need this instead.
* You just like playing with things and want to change the default display somehow.
* You also find it hard to get the remote to send the "Commissioning Mode" command to the MVHR unit (more about this later).
* You've got a spare remote kicking around and want to use it for something else.
* You want to build a clone.

### What processor does it run on?

* The 443283 remote control that I have uses a PIC16F627A.
* It's possible that this may change, and other variants of this remote may be different - I don't know.

### Why doesn't the stock firmware work with an OLED display?

* It very nearly does... Unfortunately the WINSTAR displays that most 16x2 OLEDs use defaults to Right-to-Left mode, and the 443282 firmware doesn't write the command to set Left-to-Right mode. You just see the first character of each line on the display and nothing else.

### How do I use it?

* To reprogram the existing software (firmware) you'll need to wire up the in-circuit programming pads on the PCB to a PIC programmer such as the **PICKit 3** or an **ICD3**. The PIC re-programs without issue even with the display connected.
* It obviously doesn't do much on its own other than give you a version of software that you can change yourself, or support an OLED display if that's what you want to do.
* The software was written using [MikroC Pro for PIC](https://download.mikroe.com/setups/compilers/mikroc/pic/mikroc-pic-setup-v760.zip), which is free to download and use for projects with sub-1Kbyte output, which this is.

### What else might I need?

* To replace the LCD with an OLED, you'll need to desolder the original and use "mod-wire" to connect up the OLED replacement, unless you're lucky enough to find one with the right footprint (I couldn't).
* Most stuff that you'll need is explained in the source-code.
* Included are scripts so that a **PICKit 3** can be used within MikroC (which normally isn't the case). Alternatively you can program the PIC with MPLAB IDE, but it's huge.

### Is the hardware described anywhere?

* Yes. I'll write it up on my [project site](https://www.brianmarchant.com/articles/) at some point. If you're lucky then I'll have already done it by the time you read this. Photos and a schematic will be included - enough to build your own remote control if you want to, although the genuine ones are not that expensive.
* The protocol will also be explained above. Although you can mostly deduce it from the code, there are parts that are not shown.
* The WINSTAR WEH001602AWPP5N00000 OLED Display Module is well documented on-line, including its **WS0010** controller chip.

### What's different from the Vent-Axia code?

As this project was done entirely for my own personal use, it makes a couple of changes:

* It supports the WINSTAR OLED display drivers (obviously!)
* It replaces the awkward "hold SET then immediately hold SET and UP and DOWN" keys that I could hardly ever get to work, with a log-press on the SET button (which automatically sends the original sequence). It still doesn't work every time, but it's a lot easier to keep trying until it does.
* It displays "ENTER COMMISSION" when SET is held so you know it's trying.
* It's very easy to remove that feature if you want to keep the original behaviour.

### Is there anything that doesn't work?

* As above, entering Commissioning Mode is pretty hit-and-miss, but for me it was even worse with the stock software.
* Exiting from Commissioning Mode by pressing the UP key doesn't work for me, but neither did it with the stock software.
* An OLED display is pretty bright - the software should probably send a "dim" command after some time of inactivity, but code-space is very tight and I didn't implement this. Worth doing if you can fit it in (I believe you send the command **254,19** for dim and **254,23** for full).
* Everything else works as you would expect as far as I know.

### Whis is the licence for this project?

* The project is licensed under the **MIT** licence - see LICENCE.md
* The software presented here is entirely reverse-engineered by looking at the RS-232 protocol only. It is not taken from the original Vent-Axia code and in no way represents that company (there is nothing wrong with the original code!).

### Links

* The original remote from [Vent-Axia](https://www.vent-axia.com/product/sentinel-kinetic-wired-remote-controller)
* The 16x2 OLED display that I used was from [CPC](https://cpc.farnell.com/winstar/weh001602awpp5n00000/oled-display-module-16x2-white/dp/SC15044)
* OLED brightness discussion on the [picaxe forum](https://picaxeforum.co.uk/threads/winstar-oled-brightness-control-version-2-now-using-lcd-contrast-pin-axe133.31030/)
