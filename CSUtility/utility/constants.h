/****************************************************************************
  constants.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

// The flag tells whether the axis Y of a DIB must be inverted while loading/saving or not.
const bool INVERT_AXIS_Y = true;

// The flag forces the axis Y of a DIB to be directed downward.
const bool DOWNWARD_AXIS_Y = INVERT_AXIS_Y; //true;

// Minimal standard noise deviation returned by any noise estimator.
const float MIN_NOISE_DEVIATION = 0.1f;

// Maximal intensity of an image entry.
const int MAX_INTENSITY_VALUE = 255;

// Maximal number of sub-frames (cameras) that constitute the overall image of a hall.
const int MAX_SUBFRAME_NUM = 6;

// Input video format: every even byte of a scan line contains image intensity value.
const FOURCC MY_BI_YUY2 = mmioFOURCC( 'Y', 'U', 'Y', '2' );

// Input video format: grayscaled, 8-bits bitmap.
const FOURCC MY_BI_Y800 = mmioFOURCC( 'Y', '8', '0', '0' );

// Input video format: grayscaled, 8-bits bitmap.
const FOURCC MY_BI_GRAY = mmioFOURCC( 'G', 'R', 'A', 'Y' );

// Transparent background.
const COLORREF MY_TRANSPARENT = 0xFFFFFFFF;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Colors.
///////////////////////////////////////////////////////////////////////////////////////////////////
enum MyColorIndex
{
  MY_BLACK = 0, // RGB(0x00,0x00,0x00), // Black
  MY_MAROON,    // RGB(0x80,0x00,0x00), // Maroon
  MY_GREEN,     // RGB(0x00,0x80,0x00), // Green
  MY_NAVY,      // RGB(0x00,0x00,0x80), // Navy
  MY_OLIVE,     // RGB(0x80,0x80,0x00), // Olive
  MY_PURPLE,    // RGB(0x80,0x00,0x80), // Purple
  MY_TEAL,      // RGB(0x00,0x80,0x80), // Teal
  MY_GRAY,      // RGB(0x80,0x80,0x80), // Gray
  MY_RED,       // RGB(0xFF,0x00,0x00), // Red
  MY_LIME,      // RGB(0x00,0xFF,0x00), // Lime
  MY_BLUE,      // RGB(0x00,0x00,0xFF), // Blue
  MY_YELLOW,    // RGB(0xFF,0xFF,0x00), // Yellow
  MY_FUCHSIA,   // RGB(0xFF,0x00,0xFF), // Fuchsia
  MY_AQUA,      // RGB(0x00,0xFF,0xFF), // Aqua
  MY_WHITE,     // RGB(0xFF,0xFF,0xFF), // White
  MY_SILVER     // RGB(0xC0,0xC0,0xC0)  // Silver
};

const RGBQUAD MyColors16[16] = { {0x00,0x00,0x00,0x00},    // Black
                                 {0x00,0x00,0x80,0x00},    // Maroon
                                 {0x00,0x80,0x00,0x00},    // Green
                                 {0x80,0x00,0x00,0x00},    // Navy
                                 {0x00,0x80,0x80,0x00},    // Olive
                                 {0x80,0x00,0x80,0x00},    // Purple
                                 {0x80,0x80,0x00,0x00},    // Teal
                                 {0x80,0x80,0x80,0x00},    // Gray
                                 {0x00,0x00,0xFF,0x00},    // Red
                                 {0x00,0xFF,0x00,0x00},    // Lime
                                 {0xFF,0x00,0x00,0x00},    // Blue
                                 {0x00,0xFF,0xFF,0x00},    // Yellow
                                 {0xFF,0x00,0xFF,0x00},    // Fuchsia
                                 {0xFF,0xFF,0x00,0x00},    // Aqua
                                 {0xFF,0xFF,0xFF,0x00},    // White
                                 {0xC0,0xC0,0xC0,0x00} };  // Silver

