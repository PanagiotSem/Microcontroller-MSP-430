MSP430 - Embedded Systems Project (2025)

This repository contains the coursework for the "Embedded Systems Design" course at the Department of Electrical and 
Computer Engineering, DUTH. All code was written in C and targeted for the TI MSP430 microcontroller using 
IAR Embedded Workbench.


Part 1: TimerA Hardware Abstraction Layer (HAL)

In this part, I developed a custom HAL to simplify the configuration of the TimerA peripheral. Instead of messy bitwise
operations in the main code, I used structs and bitfields to map the registers.
Features: Supports general configuration, PWM output generation and counter management.
Registers: Direct mapping of TACTL, TAR, TACCTLx, TACCR0/1.

Part2: FIR Filtering and ADC12

This project implements a Finite Impulse Response (FIR) filter using discrete linear convolution. The system proccesses
live analog signals by sampling through the ADC12 module.
Implementation: Uses a circular buffer to store samples and performs convolution in the main loop.
Efficiency: Optimized for power by utilizing Interrupt Service Routines (ISR) and Low Power Modes (LPM) between samples.
Input: Data is simulated via .mac files and .txt datasets within the IAR enviroment.

Part3: Sudoku Solution Checker

A logic-based project where the MSP430 validates 9x9 Sudoku grids. The challenge was not just the logic, but the 
optimization of execution time.
Optimization: Compared two different methods: Approach A (baseline) and Approach B (performance-optimized).
Benchmarking: Execution time was precisely measured using the IAR Cycle Counter (CCTimer) to evaluate efficiency gains.

How to build:
1. Open the .c or .h files in IAR Embedded Workbench.
2. Targeted for the MSP430x1xx family.
3. For Part 2, ensure the provided .mac file is linked to simulate ADC input.
