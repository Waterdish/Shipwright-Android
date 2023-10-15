//
//  gfx_metal.h
//  libultraship
//
//  Created by David Chavez on 16.08.22.
//

#ifdef __APPLE__

#ifndef GFX_METAL_H
#define GFX_METAL_H

#include "gfx_rendering_api.h"
#include <ImGui/backends/imgui_impl_sdl2.h>

extern struct GfxRenderingAPI gfx_metal_api;

ImTextureID gfx_metal_get_texture_by_id(int id);

bool Metal_IsSupported();

bool Metal_Init(SDL_Renderer* renderer);
void Metal_NewFrame(SDL_Renderer* renderer);
void Metal_RenderDrawData(ImDrawData* draw_data);

#endif
#endif
