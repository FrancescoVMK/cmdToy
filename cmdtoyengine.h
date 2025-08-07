#ifndef CMDTOYRENDER_H
#define CMDTOYRENDER_H

#include "cmdtoymath.h" // Include your math types

// --- Public Global Variables ---
// These variables are intended to be accessed by the user's mainImage function.

// iTime: Current time in seconds, typically used for animation
extern float iTime;

// iMouse: Mouse coordinates (x, y), typically normalized or screen-space
extern vec2 iMouse;

// iResolution: Screen resolution (width, height) in pixels
extern vec2 iResolution;

// --- Public Function ---

/**
 * @brief Initializes and starts the ASCII art renderer.
 *
 * This function sets up the terminal, allocates necessary buffers,
 * and enters an infinite loop to continuously render frames.
 *
 * @param mode The rendering mode (e.g., 0 for char, 1 for line, 2 for frame, 3 for memcpy, 4 for threaded).
 * @param maxThreadNumber The maximum number of threads to use for rendering (only applicable for threaded mode).
 * @param mainImage A function pointer to your shader-like main image function.
 * This function will be called for each pixel to determine its color.
 * @return 0 on successful initialization (though it runs indefinitely).
 */
int cmdToyRender(int mode, int maxThreadNumber, vec3 (*mainImage)(vec2 * fragCoord));

#endif // CMDTOTOYRENDER_H
