# Features

The latest release of QEMU STM32 includes the following features.  Note that only basic functionality may be provided - there will inevitably be features (especially uncommon features) that are not emulated.  I have included links to the relevant commits or files.
* Built on top of QEMU version 2.1.3
* Basic [STM32F1 microcontroller](https://github.com/beckus/qemu_stm32/blob/stm32/hw/arm/stm32.c) with the following peripherals:
  * [ADC](https://github.com/beckus/qemu_stm32/blob/stm32/hw/arm/stm32_adc.c)
  * [AFIO](https://github.com/beckus/qemu_stm32/blob/stm32/hw/gpio/stm32_afio.c)
  * [DAC](https://github.com/beckus/qemu_stm32/blob/stm32/hw/arm/stm32_dac.c)
  * [EXTI](https://github.com/beckus/qemu_stm32/blob/stm32/hw/gpio/stm32_exti.c)
  * [GPIO](https://github.com/beckus/qemu_stm32/blob/stm32/hw/gpio/stm32_gpio.c)
  * [IWDG](https://github.com/beckus/qemu_stm32/blob/stm32/hw/arm/stm32_iwdg.c)
  * [RCC](https://github.com/beckus/qemu_stm32/blob/stm32/hw/arm/stm32_rcc.c)
  * [RTC](https://github.com/beckus/qemu_stm32/blob/stm32/hw/timer/stm32_rtc.c)
  * [TIMER](https://github.com/beckus/qemu_stm32/blob/stm32/hw/timer/stm32_timer.c) - 
    Note that some modes such as counting down and one shot mode do not appear to work quite right and need some work.
  * [UART](https://github.com/beckus/qemu_stm32/blob/stm32/hw/char/stm32_uart.c)
* Machines:
  * [Olimex STM32-P103 development board](https://github.com/beckus/qemu_stm32/blob/stm32/hw/arm/stm32_p103.c)
  * [Olimex Olimexino STM32 Maple Development Board](https://github.com/beckus/qemu_stm32/blob/stm32/hw/arm/stm32_maple.c)
  * [Blue Pill (STM32F103C8)](https://github.com/beckus/qemu_stm32/blob/stm32/hw/arm/stm32_f103c8.c)
* Changes to core QEMU functionality (in ARMv7 code):
  * [Added external Systick source](https://github.com/beckus/qemu_stm32/commit/3ff4c22bb0d5f73e7bdbb825a38027d12420fe4e)
  * [Fixed interrupt handling (specifically for compatibility with FreeRTOS)](https://github.com/beckus/qemu_stm32/commit/88a7608eba13ac882259341ca043c492f7be181c)
* The [stm32_p103_demos project](https://github.com/beckus/stm32_p103_demos) contains demos that run in this emulator.

The [Pebble Smartwatch](https://www.pebble.com/) also has a QEMU emulator with STM32 functionality (it was originally based on the QEMU STM32 project).  It is located at: https://github.com/pebble/qemu .  While I have not tried the Pebble emulator, it seems quite extensive, with more peripherals (e.g. I2C and SPI) and the ability to emulate different STM32 families. **UPDATE: The company that developed Pebble has shut down, and I am not sure if anyone will continue to work on the Pebble emulator.**


# What's New

**December 5, 2018**
- Ported DMA peripheral ported from pebble/qemu. See [Pull Request 23](https://github.com/beckus/qemu_stm32/pull/23) for details. Thank you to [baselsayeh](https://github.com/baselsayeh) for submitting this.
- Improvements to error logging (log to stderr instead of stdout). See [Pull Request 25](https://github.com/beckus/qemu_stm32/pull/25) for details. Thank you to [kousu](https://github.com/kousu) for submitting this.

**October 18, 2018**
- Ported CRC peripheral from pebble/qemu. See [Pull Request 22](https://github.com/beckus/qemu_stm32/pull/22) for details. Thank you to [baselsayeh](https://github.com/baselsayeh) for submitting this.

**August 15, 2018**
- Added support for [STM32F103C8 (Blue Pill)](https://wiki.stm32duino.com/index.php?title=Blue_Pill).  Thank you to [baselsayeh](https://github.com/baselsayeh) for submitting this.

**July 24, 2018**
The following changes have been added in the stm32 branch, though not yet included in a release.
- Added Dockerfile to build a container with qemu_stm32 binaries and examples from [stm32_p103_demos](https://github.com/beckus/stm32_p103_demos).  Thank you to [glatard](https://github.com/glatard) for submitting this.
- Fixes to the RTC interrupt handling.  Thank you to [slaviber](https://github.com/slaviber).
- Fix build error in recent versions of perl.  Thank you to [benquike](https://github.com/benquike).

**July 15, 2016**
- Added IWDG peripheral implementation.  Thank you to [vinifr](https://github.com/vinifr) for submitting this.  The peripheral is in the stm32 branch but not yet included in a release.

**April 2, 2016**
- Added ADC, DAC, and RTC peripheral implementations.  Thank you to Hariri Yasser, Fatima Zohra Lahlou, and Jean-Michel Friedt for submitting these.  The peripherals are in the stm32 branch but not yet included in a release.
- Corresponding tests were also added to the [stm32_p103_demos project](https://github.com/beckus/stm32_p103_demos).  The ADC demo now works (previously it only worked on real hardware).

**June 6, 2015 - [Release v0.1.3] (https://github.com/beckus/qemu_stm32/releases/tag/stm32_v0.1.3)**
- Added timer peripheral (thank you to [hankhank](https://github.com/hankhank) for writing this)
- Merged in QEMU v2.1.3
- Fixed bugs in GPIO module (see [commit](https://github.com/beckus/qemu_stm32/commit/3e846fc6272ea226cd3fa5bd255a71e30be0e9b5) for more details)
- Miscellaneous code cleanup

# Usage
The STM32 implementation is kept in the ["stm32"](https://github.com/beckus/qemu_stm32/tree/stm32) branch (the "master" branch contains the unmodified QEMU code).
The "stm32" branch may contain commits that make the software unstable or perhaps even uncompilable.
Therefore, I recommend that you use the latest [tagged release](https://github.com/beckus/qemu_stm32/releases) that looks like this: "stm32_v0.x.x".
These are considered to be relatively stable, and at least minimally tested.

See the [README file](https://github.com/beckus/qemu_stm32/blob/stm32/README) for more details on compilation, etc..

# Goals
High level goals (in no particular order):
* Make compliant with the new QOM framework.  Make use of properties as much as possible to provide visibility into the virtual machine.  For example, the UARTs can have properties indicating their baud rate and configuration.  Also, provide properties to be used for simulating exceptional situations.  For example, the UART could provide properties used to simulate parity and framing errors.  These properties provide debugging advantages over bare hardware and may even allow for automated testing.
* Provide complete unit testing of the STM32 implementation – if the unit tests pass you should be confident nothing is broken.
* When appropriate, modify core QEMU or add new generic components (e.g. The clock tree objects and unit testing framework).  Submit these components to the main QEMU project if possible.
* Continue to add new STM32 peripherals (SPI, timers, etc.) and enhance existing peripherals.
* Add new development boards, for example the low cost STM32 discovery boards.  Someone has also suggested supporting the CrazyFlie Nano Quadcopter.  Also add support for the Cortex M4 architecture.

I have started a [wiki page with a to-do list](https://github.com/beckus/qemu_stm32/wiki/To-Do), much of which will never become reality.

# Status
With a few exceptions, I am not actively developing new functionality.  But I do perform maintenance as I have time, including:
* Merging in patches from contributors.
* Merging in new releases of QEMU.
* Fixing defects.
* Making occasional limited changes.
* Responding to questions.

I have not yet submitted any patches to QEMU mainstream for a couple of reasons:

1. This project is still in a state of flux, and I would like the flexibility to change it without going through a patch process.
2. I suspect it will take some time and effort to go through the submission process - time I would rather put into the project itself.  I have also made changes to the core framework, which may not be acceptable in the main QEMU branch or which will need to be modified.  I don't see much disadvantage to leaving this as a fork.  That being said, there are core components that could benefit other parts of QEMU (i.e. The clock tree objects and unit testing framework) that should be submitted.

# Contributions
I welcome and encourage any source code contributions.

Thank you to the following people for submitting patches:
* [baselsayeh](https://github.com/baselsayeh)
* [benquike](https://github.com/benquike)
* [glatard](https://github.com/glatard)
* [hankhank](https://github.com/hankhank)
* [kousu](https://github.com/kousu)
* [krasin](https://github.com/krasin)
* [mv0](https://github.com/mv0)
* [slaviber](https://github.com/slaviber)
* [vinifr](https://github.com/vinifr)
* Hariri Yasser and Fatima Zohra Lahlou (at the Université de Franche-Comté), under the supervision of Jean-Michel Friedt 
