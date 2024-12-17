#ifndef PINDEF_H
#define PINDEF_H

#define PIN_LED_DATA 5 // pin for LEDs, must be `PWM`

#define PIN_ENC_CLK 9    // pin for encoder `CLK` signal, must be on Port B
#define PIN_ENC_DAT 10   // pin for encoder `DAT` signal, must be on Port B
#define PIN_ENC_SWITCH 8 // pin for encoder switch, must be on external interrupt (pin 8, INT0, Port B)

#define PIN_RANDOMSEED A7 // pin for `randomSeed` sampling, must be an unconnected analog pin

// TODO: External clock on X1/X2. Either move pins back onto Ports A/B cross, or move switch off of INTO (PB) and all pins onto Port A

/* PINOUT DIAGRAMS FOR ATTINYX4 / ATTINYX5

                             ATtinyX4
                            ┌───╥┬───╖
          VCC [2.7-5.5V] - ≥╡ ● ╙┘   ╠≥ - GND
       10(0),X1,PCI8,PB0 - =╡   D    ╠= - 0(10),A0,PCI0,PA0
        9(1),X2,PCI9,PB1 - =╡   I    ╠= - 1(9),A1,TXD,PCI1,PA1
  RESET,11(11),PCI11,PB3 - =╡   P    ╠= - 2(8),A2,RXD,PCI2,PA2
     8(2),INT0,PCI10,PB2 - ≈╪   1    ╠= - 3(7),A3,PCI3,PA3
        7(3),A7,PCI7,PA7 - ≈╡   4    ╠= - 4(6),A4,SCK/SCL,PCI4,PA4
 6(4),A6,SDA/DI,PCI6,PA6 - ≈╡        ╠≈ - 5(5),A5,DO,PCI5,PA5
                            ╘════════╝

                             ATtinyX5
                            ┌───╥┬───╖
     RESET,5,A0,PCI5,PB5 - =╡ ● ╙┘   ╠≤ - VCC [2.7-5.5V]
        3,A3,X1,PCI3,PB3 - =╡   D    ╬= - 2,INT0,A1,SCK/SCL,PCI2,PB2
        4,A2,X2,PCI4,PB4 - ≈╡   8    ╠≈ - 1,DO,RXD,PCI1,PB1
                     GND - ≤╡        ╠≈ - 0,DI,TXD,SDA,PCI0,PB0
                            ╘════════╝

    = - Pin
    ≈ - PWM Pin
  ≥/≤ - VCC / GND Pins (inward to chip is VCC, outward is GND)
 4(6) - Arduino Pins, with CW(CCW) pinout (CCW on ATtinyX4 only)
  PCI - Pin Change Interrupt (PCINT) pins
  INT - External Interrupt pins (also see: ╬ or ╪, pointing inward to chip)
RESET - Reset Pin (fuses must be set to use)
DI/DO - Data In/Data Out (aka MOSI/MISO)
PA/PB - Port pins, Port A and Port B (PA on ATtinyX4 only)

Sourced from ATTinyCore by SpenceKonde
GitHub: https://github.com/SpenceKonde/ATTinyCore
ATtinyX4 Pinout: https://raw.githubusercontent.com/SpenceKonde/ATTinyCore/refs/heads/v2.0.0-devThis-is-the-head-submit-PRs-against-this/avr/extras/Pinout_x4.jpg
ATtinyX5 Pinout: https://raw.githubusercontent.com/SpenceKonde/ATTinyCore/refs/heads/v2.0.0-devThis-is-the-head-submit-PRs-against-this/avr/extras/Pinout_x5.jpg

Datasheets:
ATtinyX4: https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7701_Automotive-Microcontrollers-ATtiny24-44-84_Datasheet.pdf
ATtniyX5: https://ww1.microchip.com/downloads/en/devicedoc/atmel-2586-avr-8-bit-microcontroller-attiny25-attiny45-attiny85_datasheet.pdf

AVR Fuse Calculator (for using RESET pin as I/O):
https://www.engbedded.com/fusecalc/

*/

// Warnings to ensure ATtinyX4's Arduino pinmapping is defined correctly
#ifdef __AVR_ATtinyX4__ // ensure ATtinyX4 platform, proceed with warning checks

// SETUP: Uncomment ONE of the two definitions below to select the desired pinmapping:
#define USE_CW_PINMAPPING // this project uses CLOCKWISE (CW) pinmapping
// #define USE_CCW_PINMAPPING // this project uses COUNTERCLOCKWISE (CCW) pinmapping

#ifdef USE_CW_PINMAPPING
#ifdef USE_CCW_PINMAPPING
#error "Both CW and CCW pinmappings are specified - only one may be selected!"
#else
#ifdef PINMAPPING_CCW
#error "Project uses CW pin mapping, but CCW pinmapping is defined! Ensure ini board_build.variant is tinyX4_reverse (optional if board is attiny84_cw_pinmap), or omitted (if board is attiny84)"
#else
#ifndef PINMAPPING_CW
#error "Project uses CW pin mapping, but PINMAPPING_CW is undefined! Ensure ini board_build.variant is omitted (if board is attiny84), or that it's tinyX4 (optional, or if board is attiny84_cw_pinmap)"
#endif
#endif
#endif
#else
#ifndef USE_CCW_PINMAPPING
#error "Neither CW or CCW pinmappings are specified - one must be selected!"
#else
#ifdef PINMAPPING_CW
#error "Project uses CCW pin mapping, but CW pinmapping is defined! Ensure ini board_build.variant is omitted (if board is attiny84), or that it's tinyX4 (optional, or if board is attiny84_cw_pinmap)"
#else
#ifndef PINMAPPING_CCW
#error "Project uses CCW pin mapping, but PINMAPPING_CCW is undefined! Ensure ini board_build.variant is tinyX4_reverse (optional if board is attiny84_cw_pinmap), or omitted (if board is attiny84)"
#endif
#endif
#endif
#endif
// for more info see: https://github.com/SpenceKonde/ATTinyCore/blob/77ae92aafb2294f9838c3e15575c2b5477066439/avr/extras/ATtiny_x4.md#pin-mapping-options
#endif // --- end pinmapping warnings

#endif // PINDEF_H