//
//  gfx_metal_shader.h
//  libultraship
//
//  Created by David Chavez on 16.08.22.
//

#ifdef __APPLE__

#ifndef GFX_METAL_SHADER_H
#define GFX_METAL_SHADER_H

#include <stdio.h>
#include "gfx_cc.h"

MTL::VertexDescriptor* gfx_metal_build_shader(char buf[4096], size_t& num_floats, const CCFeatures& cc_features,
                                              bool three_point_filtering);

#endif /* GFX_METAL_SHADER_H */
#endif
