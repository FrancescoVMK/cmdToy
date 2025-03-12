# cmdToy

cmdToy is a CPU-based command-line fragment shader renderer inspired by ShaderToy. It runs locally in your terminal and supports multiple rendering modes. While functional, the implementation is not optimal and could benefit from refactoring (it's bad).

---

## Features
- **colors**
  - It uses ANSI color codes to print out colored chars.
  - It hase 8 unique colors: Black, Blue, Green, Cyan, Red, Magenta, Yellow, White. and for each it has a light and dark version for a total of 16 colors!

- **Ascii Scale**:
  - The default ascii scale is: `  ,.~!+:*Ivcow0XP$#RB@`

- **Multiple Rendering Modes**:
  - **0. Char Mode**: Prints one character at a time (single-threaded). *Slow*.
  - **1. Line Mode**: Prints one line at a time (single-threaded). *Slow*.
  - **2. Frame Mode**: Renders and prints entire frames (single-threaded). *Slow*.
  - **3. Frame Mode (Memcopy)**: Uses `memcpy` for faster buffering (single-threaded). *Normal speed*.
  - **4. Threaded Mode**: Multithreaded rendering with concurrent frame calculation/printing. *Fast*.

- **Customizable Threads**: if on mode 4 you can pass another int as argument to control thread count (default 10).
- **Print debug frame time**: Decomment the line `printf("Time taken: %f seconds\n", frameTime);` (it can cause some glitches in Threaded mode).

- **quality**: Zoomout or zoomin the cmd to change the quality.
---

## Build

Compile with:

    gcc renderingcmd.c -lm -O3 -o renderingcmd.out

---

## Usage

Run with a mode argument (0-4):

    ./renderingcmd.out 4

---

## Example Shader

A basic raymarching shader is included baldy made by me. For reference, see: [ShaderToy Example](https://www.shadertoy.com/view/X3GGzD).

---

## Platform

- **Linux only** (Windows console performance is too slow).

---

## Known Issues and TODOs

- **Refactoring**: Poorly structured code needs cleanup.
- **Math Functions**:
  - [ ] Separate math utilities into a module.
  - [ ] Add missing ShaderToy math functions.
  - [ ] Remove old macro function
  - [ ] Use only one naming scheme
  - [ ] See if passing result by reference is better or worse
  - [ ] Create a documentation to help port shaderToy shaders to cmdToy
- **Textures**:
  - [ ] Implement `iChannel` support.
- **Threads**:
  - [ ] Optimize thread management.
- **Colors**
  - [ ] Make it print real rgb color by prinring cells of 3 chars of diffrent color. (only at high resolutions)

---

## Notes

- My first C project — messy code and probably non optimal.
- If main image returns a color that is not in the range 1.0 and 0.0 it may cause a crash.
- Contributions welcome!

## Preview
![2025-02-15-18-34-58](https://github.com/user-attachments/assets/eb642519-bbc2-4537-9764-a8cc061706f6)

---
