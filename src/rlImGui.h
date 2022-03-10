#pragma once

#include "raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

// basic API
void rlImGuiSetup(bool dark);
void rlImGuiBegin();
void rlImGuiEnd();
void rlImGuiShutdown();

// Advanced StartupAPI
void rlImGuiBeginInitImGui();
void rlImGuiEndInitImGui();
void rlImGuiReloadFonts();

// image API
void rlImGuiImage(const Texture *image);
void rlImGuiImageSize(const Texture *image, int width, int height);
void rlImGuiImageRect(const Texture* image, int destWidth, int destHeight, Rectangle sourceRect);

#ifdef __cplusplus
}
#endif