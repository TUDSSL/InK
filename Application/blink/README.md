## InK blinking LED

### Hardware requirement
This demo is developed for [MSP430FR5969 LaunchPad](http://www.ti.com/tool/MSP-EXP430FR5969) with [TI Code Composer Studio™ IDE](http://www.ti.com/design-resources/embedded-development/ccs-development-tools.html).

Mainly the following components on the board are used:

* Button S1 (P4.5) and S2 (P1.1)
* Red LED 1 (P4.6) and green LED 2 (P1.0)
* A 32-kHz crystal (Y4)

### Features

* Pushing button S1 triggers the red LED to blink quickly for four times
* Pushing button S2 triggers the green LED to blink slowly for four times
* The red LED has higher priority over the green LED, so red LED blinking could preempt green LED blinking
* Power failure resilience
    * For example, if power is lost at the second blinking of the green LED, after power is back online the green LED will continue blinking and finish the whole process of blinking four times
    * Priority and preemption are kept during power failure and could be restored after power recovery
* Ultra low power enabled by using RTC, LFXT and LPM3