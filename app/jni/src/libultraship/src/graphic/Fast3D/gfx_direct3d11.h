#ifdef ENABLE_DX11

#ifndef GFX_DIRECT3D11_H
#define GFX_DIRECT3D11_H

#include "gfx_rendering_api.h"
#include <imgui.h>

extern struct GfxRenderingAPI gfx_direct3d11_api;
ImTextureID gfx_d3d11_get_texture_by_id(int id);

#endif

#endif
