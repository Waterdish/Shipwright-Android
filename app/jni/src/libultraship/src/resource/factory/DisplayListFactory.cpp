#include "resource/factory/DisplayListFactory.h"
#include "resource/type/DisplayList.h"
#include "spdlog/spdlog.h"

#define ARRAY_COUNT(arr) (s32)(sizeof(arr) / sizeof(arr[0]))

namespace LUS {
std::shared_ptr<IResource> DisplayListFactory::ReadResource(std::shared_ptr<ResourceInitData> initData,
                                                            std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<DisplayList>(initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion) {
        case 0:
            factory = std::make_shared<DisplayListFactoryV0>();
            break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load DisplayList with version {}", resource->GetInitData()->ResourceVersion);
        return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

std::shared_ptr<IResource> DisplayListFactory::ReadResourceXML(std::shared_ptr<ResourceInitData> initData,
                                                               tinyxml2::XMLElement* reader) {
    auto resource = std::make_shared<DisplayList>(initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion) {
        case 0:
            factory = std::make_shared<DisplayListFactoryV0>();
            break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load DisplayList with version {}", resource->GetInitData()->ResourceVersion);
        return nullptr;
    }

    factory->ParseFileXML(reader, resource);

    return resource;
}

void DisplayListFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<IResource> resource) {
    std::shared_ptr<DisplayList> displayList = std::static_pointer_cast<DisplayList>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, displayList);

    while (reader->GetBaseAddress() % 8 != 0) {
        reader->ReadInt8();
    }

    while (true) {
        Gfx command;
        command.words.w0 = reader->ReadUInt32();
        command.words.w1 = reader->ReadUInt32();

        displayList->Instructions.push_back(command);

        uint8_t opcode = (uint8_t)(command.words.w0 >> 24);

        // These are 128-bit commands, so read an extra 64 bits...
        if (opcode == G_SETTIMG_OTR_HASH || opcode == G_DL_OTR_HASH || opcode == G_VTX_OTR_HASH ||
            opcode == G_BRANCH_Z_OTR || opcode == G_MARKER || opcode == G_MTX_OTR) {
            command.words.w0 = reader->ReadUInt32();
            command.words.w1 = reader->ReadUInt32();

            displayList->Instructions.push_back(command);
        }

        if (opcode == G_ENDDL) {
            break;
        }
    }
}

std::unordered_map<std::string, uint32_t> renderModes = { { "G_RM_ZB_OPA_SURF", G_RM_ZB_OPA_SURF },
                                                          { "G_RM_AA_ZB_OPA_SURF", G_RM_AA_ZB_OPA_SURF },
                                                          { "G_RM_AA_ZB_OPA_DECAL", G_RM_AA_ZB_OPA_DECAL },
                                                          { "G_RM_AA_ZB_OPA_INTER", G_RM_AA_ZB_OPA_INTER },
                                                          { "G_RM_AA_ZB_TEX_EDGE", G_RM_AA_ZB_TEX_EDGE },
                                                          { "G_RM_AA_ZB_XLU_SURF", G_RM_AA_ZB_XLU_SURF },
                                                          { "G_RM_AA_ZB_XLU_DECAL", G_RM_AA_ZB_XLU_DECAL },
                                                          { "G_RM_AA_ZB_XLU_INTER", G_RM_AA_ZB_XLU_INTER },
                                                          { "G_RM_FOG_SHADE_A", G_RM_FOG_SHADE_A },
                                                          { "G_RM_FOG_PRIM_A", G_RM_FOG_PRIM_A },
                                                          { "G_RM_PASS", G_RM_PASS },
                                                          { "G_RM_ADD", G_RM_ADD },
                                                          { "G_RM_NOOP", G_RM_NOOP },
                                                          { "G_RM_ZB_OPA_SURF", G_RM_ZB_OPA_SURF },
                                                          { "G_RM_ZB_OPA_DECAL", G_RM_ZB_OPA_DECAL },
                                                          { "G_RM_ZB_XLU_SURF", G_RM_ZB_XLU_SURF },
                                                          { "G_RM_ZB_XLU_DECAL", G_RM_ZB_XLU_DECAL },
                                                          { "G_RM_OPA_SURF", G_RM_OPA_SURF },
                                                          { "G_RM_ZB_CLD_SURF", G_RM_ZB_CLD_SURF },
                                                          { "G_RM_ZB_OPA_SURF2", G_RM_ZB_OPA_SURF2 },
                                                          { "G_RM_AA_ZB_OPA_SURF2", G_RM_AA_ZB_OPA_SURF2 },
                                                          { "G_RM_AA_ZB_OPA_DECAL2", G_RM_AA_ZB_OPA_DECAL2 },
                                                          { "G_RM_AA_ZB_OPA_INTER2", G_RM_AA_ZB_OPA_INTER2 },
                                                          { "G_RM_AA_ZB_TEX_EDGE2", G_RM_AA_ZB_TEX_EDGE2 },
                                                          { "G_RM_AA_ZB_XLU_SURF2", G_RM_AA_ZB_XLU_SURF2 },
                                                          { "G_RM_AA_ZB_XLU_DECAL2", G_RM_AA_ZB_XLU_DECAL2 },
                                                          { "G_RM_AA_ZB_XLU_INTER2", G_RM_AA_ZB_XLU_INTER2 },
                                                          { "G_RM_ADD2", G_RM_ADD2 },
                                                          { "G_RM_ZB_OPA_SURF2", G_RM_ZB_OPA_SURF2 },
                                                          { "G_RM_ZB_OPA_DECAL2", G_RM_ZB_OPA_DECAL2 },
                                                          { "G_RM_ZB_XLU_SURF2", G_RM_ZB_XLU_SURF2 },
                                                          { "G_RM_ZB_XLU_DECAL2", G_RM_ZB_XLU_DECAL2 },
                                                          { "G_RM_ZB_CLD_SURF2", G_RM_ZB_CLD_SURF2 } };

static Gfx GsSpVertexOtR2P1(char* filePathPtr) {
    Gfx g;
    g.words.w0 = G_VTX_OTR_FILEPATH << 24;
    g.words.w1 = (uintptr_t)filePathPtr;

    return g;
}

static Gfx GsSpVertexOtR2P2(int vtxCnt, int vtxBufOffset, int vtxDataOffset) {
    Gfx g;
    g.words.w0 = (uintptr_t)vtxCnt;
    g.words.w1 = (uintptr_t)((vtxBufOffset << 16) | vtxDataOffset);

    return g;
}

void DisplayListFactoryV0::ParseFileXML(tinyxml2::XMLElement* reader, std::shared_ptr<IResource> resource) {
    std::shared_ptr<DisplayList> dl = std::static_pointer_cast<DisplayList>(resource);

    auto child = reader->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();

        Gfx g = gsDPPipeSync();

        if (childName == "PipeSync") {
            g = gsDPPipeSync();
        } else if (childName == "Texture") {
            g = gsSPTexture(child->IntAttribute("S"), child->IntAttribute("T"), child->IntAttribute("Level"),
                            child->IntAttribute("Tile"), child->IntAttribute("On"));
        } else if (childName == "SetPrimColor") {
            g = gsDPSetPrimColor(child->IntAttribute("M"), child->IntAttribute("L"), child->IntAttribute("R"),
                                 child->IntAttribute("G"), child->IntAttribute("B"), child->IntAttribute("A"));
        } else if (childName == "SetPrimDepth") {
            g = gsDPSetPrimDepth(child->IntAttribute("Z"), child->IntAttribute("DZ"));
        } else if (childName == "SetFillColor") {
            g = gsDPSetFillColor(child->IntAttribute("C"));
        } else if (childName == "SetFogColor") {
            g = gsDPSetFogColor(child->IntAttribute("R"), child->IntAttribute("G"), child->IntAttribute("B"),
                                child->IntAttribute("A"));
        } else if (childName == "SetBlendColor") {
            g = gsDPSetBlendColor(child->IntAttribute("R"), child->IntAttribute("G"), child->IntAttribute("B"),
                                  child->IntAttribute("A"));
        } else if (childName == "SetEnvColor") {
            g = gsDPSetEnvColor(child->IntAttribute("R"), child->IntAttribute("G"), child->IntAttribute("B"),
                                child->IntAttribute("A"));
        } else if (childName == "Grayscale") {
            g = gsSPGrayscale(child->BoolAttribute("Enabled"));
        } else if (childName == "SetGrayscaleColor") {
            g = gsDPSetGrayscaleColor(child->IntAttribute("R"), child->IntAttribute("G"), child->IntAttribute("B"),
                                      child->IntAttribute("A"));
        } else if (childName == "SetDepthSource") {
            g = gsDPSetDepthSource(child->IntAttribute("Mode"));
        } else if (childName == "SetAlphaCompare") {
            g = gsDPSetAlphaCompare(child->IntAttribute("Mode"));
        } else if (childName == "SetAlphaDither") {
            g = gsDPSetAlphaDither(child->IntAttribute("Type"));
        } else if (childName == "SetColorDither") {
            g = gsDPSetColorDither(child->IntAttribute("Type"));
        } else if (childName == "SetCombineKey") {
            g = gsDPSetCombineKey(child->IntAttribute("Type"));
        } else if (childName == "SetTextureFilter") {
            g = gsDPSetTextureFilter(child->IntAttribute("Mode"));
        } else if (childName == "SetTextureLOD") {
            g = gsDPSetTextureLOD(child->IntAttribute("Mode"));
        } else if (childName == "SetTextureDetail") {
            g = gsDPSetTextureDetail(child->IntAttribute("Type"));
        } else if (childName == "SetTexturePersp") {
            g = gsDPSetTexturePersp(child->IntAttribute("Enable"));
        } else if (childName == "PerspNormalize") {
            g = gsSPPerspNormalize(child->IntAttribute("S"));
        } else if (childName == "FogPosition") {
            g = gsSPFogPosition(child->IntAttribute("Min"), child->IntAttribute("Max"));
        } else if (childName == "FogFactor") {
            g = gsSPFogFactor(child->IntAttribute("FM"), child->IntAttribute("FO"));
        } else if (childName == "NumLites") {
            g = gsSPNumLights(child->IntAttribute("Lites"));
        } else if (childName == "Segment") {
            g = gsSPSegment(child->IntAttribute("Seg"), child->IntAttribute("Base"));
        }
        /*else if (childName == "Line3D")
        {
                g = gsSPLine3D(child->IntAttribute("V0"), child->IntAttribute("V1"), child->IntAttribute("Flag"));
        }
        */
        /*else if (childName == "Hilite2Tile")
        {
                g = gsDPSetHilite2Tile(child->IntAttribute("Tile"), child->IntAttribute("Hilite"),
        child->IntAttribute("Width"), child->IntAttribute("Height"));
        }*/
        else if (childName == "Matrix") {
            std::string fName = child->Attribute("Path");
            std::string param = child->Attribute("Param");

            uint8_t paramInt = 0;

            if (param == "G_MTX_PUSH") {
                paramInt = G_MTX_PUSH;
            } else if (param == "G_MTX_NOPUSH") {
                paramInt = G_MTX_NOPUSH;
            } else if (param == "G_MTX_LOAD") {
                paramInt = G_MTX_LOAD;
            } else if (param == "G_MTX_MUL") {
                paramInt = G_MTX_MUL;
            } else if (param == "G_MTX_MODELVIEW") {
                paramInt = G_MTX_MODELVIEW;
            } else if (param == "G_MTX_PROJECTION") {
                paramInt = G_MTX_PROJECTION;
            }

            if (fName[0] == '>' && fName[1] == '0' && fName[2] == 'x') {
                int offset = strtol(fName.c_str() + 1, NULL, 16);
                g = gsSPMatrix(offset | 1, paramInt);
            } else {
                g = { gsSPMatrix(0, paramInt) };

                g.words.w0 &= 0x00FFFFFF;
                g.words.w0 += (G_MTX_OTR2 << 24);
                g.words.w1 = (uintptr_t)malloc(fName.size() + 1);
                strcpy((char*)g.words.w1, fName.data());
            }
        } else if (childName == "SetCycleType") {
            uint32_t param = 0;

            if (child->Attribute("G_CYC_1CYCLE", 0)) {
                param |= G_CYC_1CYCLE;
            }

            if (child->Attribute("G_CYC_2CYCLE", 0)) {
                param |= G_CYC_2CYCLE;
            }

            if (child->Attribute("G_CYC_COPY", 0)) {
                param |= G_CYC_COPY;
            }

            if (child->Attribute("G_CYC_FILL", 0)) {
                param |= G_CYC_FILL;
            }

            g = gsDPSetCycleType(param);
        } else if (childName == "PipelineMode") {
            uint32_t param = 0;

            if (child->Attribute("G_PM_1PRIMITIVE", 0)) {
                param |= G_PM_1PRIMITIVE;
            }

            if (child->Attribute("G_PM_NPRIMITIVE", 0)) {
                param |= G_PM_NPRIMITIVE;
            }

            g = gsDPPipelineMode(param);
        } else if (childName == "TileSync") {
            g = gsDPTileSync();
        } else if (childName == "LoadTile") {
            uint32_t t = child->IntAttribute("T");
            uint32_t uls = child->IntAttribute("Uls");
            uint32_t ult = child->IntAttribute("Ult");
            uint32_t lrs = child->IntAttribute("Lrs");
            uint32_t lrt = child->IntAttribute("Lrt");

            g = gsDPLoadTile(t, uls, ult, lrs, lrt);
        } else if (childName == "SetTextureLUT") {
            std::string mode = child->Attribute("Mode");
            uint32_t modeVal = 0;

            if (mode == "G_TT_NONE") {
                modeVal = G_TT_NONE;
            } else if (mode == "G_TT_RGBA16") {
                modeVal = G_TT_RGBA16;
            } else if (mode == "G_TT_IA16") {
                modeVal = G_TT_IA16;
            }

            g = gsDPSetTextureLUT(modeVal);
        } else if (childName == "LoadTLUTCmd") {
            uint32_t tile = child->IntAttribute("Tile");
            uint32_t count = child->IntAttribute("Count");

            g = gsDPLoadTLUTCmd(tile, count);
        } else if (childName == "SetCombineLERP") {
            const char* a0 = child->Attribute("A0", 0);
            const char* b0 = child->Attribute("B0", 0);
            const char* c0 = child->Attribute("C0", 0);
            const char* d0 = child->Attribute("D0", 0);

            const char* aa0 = child->Attribute("Aa0", 0);
            const char* ab0 = child->Attribute("Ab0", 0);
            const char* ac0 = child->Attribute("Ac0", 0);
            const char* ad0 = child->Attribute("Ad0", 0);

            const char* a1 = child->Attribute("A1", 0);
            const char* b1 = child->Attribute("B1", 0);
            const char* c1 = child->Attribute("C1", 0);
            const char* d1 = child->Attribute("D1", 0);

            const char* aa1 = child->Attribute("Aa1", 0);
            const char* ab1 = child->Attribute("Ab1", 0);
            const char* ac1 = child->Attribute("Ac1", 0);
            const char* ad1 = child->Attribute("Ad1", 0);

            g = gsDPSetCombineLERP_NoMacros(
                GetCombineLERPValue(a0), GetCombineLERPValue(b0), GetCombineLERPValue(c0), GetCombineLERPValue(d0),
                GetCombineLERPValue(aa0), GetCombineLERPValue(ab0), GetCombineLERPValue(ac0), GetCombineLERPValue(ad0),
                GetCombineLERPValue(a1), GetCombineLERPValue(b1), GetCombineLERPValue(c1), GetCombineLERPValue(d1),
                GetCombineLERPValue(aa1), GetCombineLERPValue(ab1), GetCombineLERPValue(ac1), GetCombineLERPValue(ad1));
        } else if (childName == "LoadSync") {
            g = gsDPLoadSync();
        } else if (childName == "LoadBlock") {
            uint32_t tile = child->IntAttribute("Tile");
            uint32_t uls = child->IntAttribute("Uls");
            uint32_t ult = child->IntAttribute("Ult");
            uint32_t lrs = child->IntAttribute("Lrs");
            uint32_t dxt = child->IntAttribute("Dxt");

            g = gsDPLoadBlock(tile, uls, ult, lrs, dxt);
        } else if (childName == "Triangle1") {
            int v00 = child->IntAttribute("V00");
            int v01 = child->IntAttribute("V01");
            int v02 = child->IntAttribute("V02");

            g = gsSP1TriangleOTR(v00, v01, v02, child->IntAttribute("Flag0"));
            g.words.w0 &= 0xFF000000;
            g.words.w0 |= v00;
            g.words.w1 |= v01 << 16;
            g.words.w1 |= v02 << 0;
        } else if (childName == "Triangles2") {
            g = gsSP2Triangles(child->IntAttribute("V00"), child->IntAttribute("V01"), child->IntAttribute("V02"),
                               child->IntAttribute("Flag0"), child->IntAttribute("V10"), child->IntAttribute("V11"),
                               child->IntAttribute("V12"), child->IntAttribute("Flag1"));
        } else if (childName == "LoadVertices") {
            std::string fName = child->Attribute("Path");
            // fName = ">" + fName;

            char* filePath = (char*)malloc(fName.size() + 1);
            strcpy(filePath, fName.data());

            g = GsSpVertexOtR2P1(filePath);

            dl->Instructions.push_back(g);

            g = GsSpVertexOtR2P2(child->IntAttribute("Count"), child->IntAttribute("VertexBufferIndex"),
                                 child->IntAttribute("VertexOffset"));
        } else if (childName == "SetTextureImage") {
            std::string fName = child->Attribute("Path");
            // fName = ">" + fName;
            std::string fmt = child->Attribute("Format");
            uint32_t fmtVal = G_IM_FMT_RGBA;

            if (fmt == "G_IM_FMT_I") {
                fmtVal = G_IM_FMT_I;
            } else if (fmt == "G_IM_FMT_IA") {
                fmtVal = G_IM_FMT_IA;
            } else if (fmt == "G_IM_FMT_CI") {
                fmtVal = G_IM_FMT_CI;
            } else if (fmt == "G_IM_FMT_YUV") {
                fmtVal = G_IM_FMT_YUV;
            } else if (fmt == "G_IM_FMT_RGBA") {
                fmtVal = G_IM_FMT_RGBA;
            }

            std::string siz = child->Attribute("Size");
            uint32_t sizVal = G_IM_SIZ_32b;

            if (siz == "G_IM_SIZ_8b_LOAD_BLOCK") {
                sizVal = G_IM_SIZ_8b_LOAD_BLOCK;
            } else if (siz == "G_IM_SIZ_4b") {
                sizVal = G_IM_SIZ_4b;
            } else if (siz == "G_IM_SIZ_8b") {
                sizVal = G_IM_SIZ_8b;
            } else if (siz == "G_IM_SIZ_16b" || siz == "G_IM_SIZ_16b_LOAD_BLOCK") {
                sizVal = G_IM_SIZ_16b;
            } else if (siz == "G_IM_SIZ_32b") {
                sizVal = G_IM_SIZ_32b;
            } else if (siz == "G_IM_SIZ_DD") {
                sizVal = G_IM_SIZ_DD;
            } else {
                int bp = 0;
            }

            uint32_t width = child->IntAttribute("Width");

            if (fName[0] == '>' && fName[1] == '0' && (fName[2] == 'x' || fName[2] == 'X')) {
                uint32_t seg = std::stoul(fName.substr(1), nullptr, 16);
                g = { gsDPSetTextureImage(fmtVal, sizVal, width + 1, seg | 1) };
            } else {
                g = { gsDPSetTextureImage(fmtVal, sizVal, width + 1, 0) };
                g.words.w0 &= 0x00FFFFFF;
                g.words.w0 += (G_SETTIMG_OTR_FILEPATH << 24);
                g.words.w1 = (uintptr_t)malloc(fName.size() + 1);
                strcpy((char*)g.words.w1, fName.data());
            }

            dl->Instructions.push_back(g);

            g = gsDPPipeSync();
        } else if (childName == "SetTile") {
            uint32_t line = child->IntAttribute("Line");
            uint32_t tmem = child->IntAttribute("TMem");
            uint32_t tile = child->IntAttribute("Tile");
            uint32_t palette = child->IntAttribute("Palette");
            std::string cms0 = child->Attribute("Cms0");
            std::string cms1 = child->Attribute("Cms1");
            std::string cmt0 = child->Attribute("Cmt0");
            std::string cmt1 = child->Attribute("Cmt1");
            uint32_t maskS = child->IntAttribute("MaskS");
            uint32_t maskT = child->IntAttribute("MaskT");
            uint32_t shiftS = child->IntAttribute("ShiftS");
            uint32_t shiftT = child->IntAttribute("ShiftT");

            std::string fmt = child->Attribute("Format");
            uint32_t fmtVal = G_IM_FMT_RGBA;

            if (fmt == "G_IM_FMT_I") {
                fmtVal = G_IM_FMT_I;
            } else if (fmt == "G_IM_FMT_IA") {
                fmtVal = G_IM_FMT_IA;
            } else if (fmt == "G_IM_FMT_CI") {
                fmtVal = G_IM_FMT_CI;
            } else if (fmt == "G_IM_FMT_YUV") {
                fmtVal = G_IM_FMT_YUV;
            } else if (fmt == "G_IM_FMT_RGBA") {
                fmtVal = G_IM_FMT_RGBA;
            }

            std::string siz = child->Attribute("Size");
            uint32_t sizVal = G_IM_SIZ_32b;

            if (siz == "G_IM_SIZ_8b_LOAD_BLOCK") {
                sizVal = G_IM_SIZ_8b_LOAD_BLOCK;
            } else if (siz == "G_IM_SIZ_4b") {
                sizVal = G_IM_SIZ_4b;
            } else if (siz == "G_IM_SIZ_8b") {
                sizVal = G_IM_SIZ_8b;
            } else if (siz == "G_IM_SIZ_16b" || siz == "G_IM_SIZ_16b_LOAD_BLOCK") {
                sizVal = G_IM_SIZ_16b;
            } else if (siz == "G_IM_SIZ_32b") {
                sizVal = G_IM_SIZ_32b;
            } else if (siz == "G_IM_SIZ_DD") {
                sizVal = G_IM_SIZ_DD;
            } else {
                int bp = 0;
            }

            uint32_t cms0Val = 0;
            uint32_t cms1Val = 0;
            uint32_t cmt0Val = 0;
            uint32_t cmt1Val = 0;

            if (cms0 == "G_TX_MIRROR") {
                cms0Val = G_TX_MIRROR;
            }

            if (cms0 == "G_TX_CLAMP") {
                cms0Val = G_TX_CLAMP;
            }

            if (cms1 == "G_TX_MIRROR") {
                cms1Val = G_TX_MIRROR;
            }

            if (cms1 == "G_TX_CLAMP") {
                cms1Val = G_TX_CLAMP;
            }

            if (cmt0 == "G_TX_MIRROR") {
                cmt0Val = G_TX_MIRROR;
            }

            if (cmt0 == "G_TX_CLAMP") {
                cmt0Val = G_TX_CLAMP;
            }

            if (cmt1 == "G_TX_MIRROR") {
                cmt1Val = G_TX_MIRROR;
            }

            if (cmt1 == "G_TX_CLAMP") {
                cmt1Val = G_TX_CLAMP;
            }

            g = gsDPSetTile(fmtVal, sizVal, line, tmem, tile, palette, cmt0Val | cmt1Val, maskT, shiftT,
                            cms0Val | cms1Val, maskS, shiftS);
        } else if (childName == "SetTileSize") {
            uint32_t t = child->IntAttribute("T");
            uint32_t uls = child->IntAttribute("Uls");
            uint32_t ult = child->IntAttribute("Ult");
            uint32_t lrs = child->IntAttribute("Lrs");
            uint32_t lrt = child->IntAttribute("Lrt");

            g = gsDPSetTileSize(t, uls, ult, lrs, lrt);
        } else if (childName == "SetOtherMode") {
            std::string cmdStr = child->Attribute("Cmd");
            int sft = child->IntAttribute("Sft");
            int length = child->IntAttribute("Length");
            uint32_t data = 0;
            uint32_t cmdVal = 0;

            if (cmdStr == "G_SETOTHERMODE_H") {
                cmdVal = G_SETOTHERMODE_H;
            } else if (cmdStr == "G_SETOTHERMODE_L") {
                cmdVal = G_SETOTHERMODE_L;
            }

            // OTRTODO: There are so many more of these we haven't added in yet...
            if (child->Attribute("G_AD_PATTERN", 0)) {
                data |= G_AD_PATTERN;
            }

            if (child->Attribute("G_AD_NOTPATTERN", 0)) {
                data |= G_AD_NOTPATTERN;
            }

            if (child->Attribute("G_AD_DISABLE", 0)) {
                data |= G_AD_DISABLE;
            }

            if (child->Attribute("G_AD_NOISE", 0)) {
                data |= G_AD_NOISE;
            }

            if (child->Attribute("G_CD_MAGICSQ", 0)) {
                data |= G_CD_MAGICSQ;
            }

            if (child->Attribute("G_CD_BAYER", 0)) {
                data |= G_CD_BAYER;
            }

            if (child->Attribute("G_CD_NOISE", 0)) {
                data |= G_CD_NOISE;
            }

            if (child->Attribute("G_CK_NONE", 0)) {
                data |= G_CK_NONE;
            }

            if (child->Attribute("G_CK_KEY", 0)) {
                data |= G_CK_KEY;
            }

            if (child->Attribute("G_TC_CONV", 0)) {
                data |= G_TC_CONV;
            }

            if (child->Attribute("G_TC_FILTCONV", 0)) {
                data |= G_TC_FILTCONV;
            }

            if (child->Attribute("G_TC_FILT", 0)) {
                data |= G_TC_FILT;
            }

            if (child->Attribute("G_TF_POINT", 0)) {
                data |= G_TF_POINT;
            }

            if (child->Attribute("G_TF_AVERAGE", 0)) {
                data |= G_TF_AVERAGE;
            }

            if (child->Attribute("G_TF_BILERP", 0)) {
                data |= G_TF_BILERP;
            }

            if (child->Attribute("G_TL_TILE", 0)) {
                data |= G_TL_TILE;
            }

            if (child->Attribute("G_TL_LOD", 0)) {
                data |= G_TL_LOD;
            }

            if (child->Attribute("G_TD_CLAMP", 0)) {
                data |= G_TD_CLAMP;
            }

            if (child->Attribute("G_TD_SHARPEN", 0)) {
                data |= G_TD_SHARPEN;
            }

            if (child->Attribute("G_TD_DETAIL", 0)) {
                data |= G_TD_DETAIL;
            }

            if (child->Attribute("G_TP_NONE", 0)) {
                data |= G_TP_NONE;
            }

            if (child->Attribute("G_TP_PERSP", 0)) {
                data |= G_TP_PERSP;
            }

            if (child->Attribute("G_CYC_1CYCLE", 0)) {
                data |= G_CYC_1CYCLE;
            }

            if (child->Attribute("G_CYC_COPY", 0)) {
                data |= G_CYC_COPY;
            }

            if (child->Attribute("G_CYC_FILL", 0)) {
                data |= G_CYC_FILL;
            }

            if (child->Attribute("G_CYC_2CYCLE", 0)) {
                data |= G_CYC_2CYCLE;
            }

            if (child->Attribute("G_PM_1PRIMITIVE", 0)) {
                data |= G_PM_1PRIMITIVE;
            }

            if (child->Attribute("G_PM_NPRIMITIVE", 0)) {
                data |= G_PM_NPRIMITIVE;
            }

            if (child->Attribute("G_RM_FOG_SHADE_A", 0)) {
                data |= G_RM_FOG_SHADE_A;
            }

            if (child->Attribute("G_RM_FOG_PRIM_A", 0)) {
                data |= G_RM_FOG_PRIM_A;
            }

            if (child->Attribute("G_RM_PASS", 0)) {
                data |= G_RM_PASS;
            }

            if (child->Attribute("G_ZS_PIXEL", 0)) {
                data |= G_ZS_PIXEL;
            }

            if (child->Attribute("G_ZS_PRIM", 0)) {
                data |= G_ZS_PRIM;
            }

            if (child->Attribute("G_RM_AA_ZB_OPA_SURF", 0)) {
                data |= G_RM_AA_ZB_OPA_SURF;
            }

            if (child->Attribute("G_RM_AA_ZB_OPA_SURF2", 0)) {
                data |= G_RM_AA_ZB_OPA_SURF2;
            }

            if (child->Attribute("G_RM_AA_ZB_OPA_SURF2", 0)) {
                data |= G_RM_AA_ZB_OPA_SURF2;
            }

            if (child->Attribute("G_RM_AA_ZB_XLU_SURF", 0)) {
                data |= G_RM_AA_ZB_XLU_SURF;
            }

            if (child->Attribute("G_RM_AA_ZB_XLU_SURF2", 0)) {
                data |= G_RM_AA_ZB_XLU_SURF2;
            }

            if (child->Attribute("G_RM_AA_ZB_OPA_DECAL", 0)) {
                data |= G_RM_AA_ZB_OPA_DECAL;
            }

            if (child->Attribute("G_RM_AA_ZB_OPA_DECAL2", 0)) {
                data |= G_RM_AA_ZB_OPA_DECAL2;
            }

            if (child->Attribute("G_RM_AA_ZB_XLU_DECAL", 0)) {
                data |= G_RM_AA_ZB_XLU_DECAL;
            }

            if (child->Attribute("G_RM_AA_ZB_XLU_DECAL2", 0)) {
                data |= G_RM_AA_ZB_XLU_DECAL2;
            }

            if (child->Attribute("G_RM_AA_ZB_OPA_INTER", 0)) {
                data |= G_RM_AA_ZB_OPA_INTER;
            }

            if (child->Attribute("G_RM_AA_ZB_OPA_INTER2", 0)) {
                data |= G_RM_AA_ZB_OPA_INTER2;
            }

            if (child->Attribute("G_RM_AA_ZB_XLU_INTER", 0)) {
                data |= G_RM_AA_ZB_XLU_INTER;
            }

            if (child->Attribute("G_RM_AA_ZB_XLU_INTER2", 0)) {
                data |= G_RM_AA_ZB_XLU_INTER2;
            }

            if (child->Attribute("G_RM_AA_ZB_XLU_LINE", 0)) {
                data |= G_RM_AA_ZB_XLU_LINE;
            }

            if (child->Attribute("G_RM_AA_ZB_XLU_LINE2", 0)) {
                data |= G_RM_AA_ZB_XLU_LINE2;
            }

            if (child->Attribute("G_RM_AA_ZB_DEC_LINE", 0)) {
                data |= G_RM_AA_ZB_DEC_LINE;
            }

            if (child->Attribute("G_RM_AA_ZB_DEC_LINE2", 0)) {
                data |= G_RM_AA_ZB_DEC_LINE2;
            }

            if (child->Attribute("G_RM_AA_ZB_TEX_EDGE", 0)) {
                data |= G_RM_AA_ZB_TEX_EDGE;
            }

            if (child->Attribute("G_RM_AA_ZB_TEX_EDGE2", 0)) {
                data |= G_RM_AA_ZB_TEX_EDGE2;
            }

            if (child->Attribute("G_RM_AA_ZB_TEX_INTER", 0)) {
                data |= G_RM_AA_ZB_TEX_INTER;
            }

            if (child->Attribute("G_RM_AA_ZB_TEX_INTER2", 0)) {
                data |= G_RM_AA_ZB_TEX_INTER2;
            }

            if (child->Attribute("G_RM_AA_ZB_SUB_SURF", 0)) {
                data |= G_RM_AA_ZB_SUB_SURF;
            }

            if (child->Attribute("G_RM_AA_ZB_SUB_SURF2", 0)) {
                data |= G_RM_AA_ZB_SUB_SURF2;
            }

            if (child->Attribute("G_RM_AA_ZB_PCL_SURF", 0)) {
                data |= G_RM_AA_ZB_PCL_SURF;
            }

            if (child->Attribute("G_RM_AA_ZB_PCL_SURF2", 0)) {
                data |= G_RM_AA_ZB_PCL_SURF2;
            }

            if (child->Attribute("G_RM_AA_ZB_OPA_TERR", 0)) {
                data |= G_RM_AA_ZB_OPA_TERR;
            }

            if (child->Attribute("G_RM_AA_ZB_OPA_TERR2", 0)) {
                data |= G_RM_AA_ZB_OPA_TERR2;
            }

            if (child->Attribute("G_RM_AA_ZB_TEX_TERR", 0)) {
                data |= G_RM_AA_ZB_TEX_TERR;
            }

            if (child->Attribute("G_RM_AA_ZB_TEX_TERR2", 0)) {
                data |= G_RM_AA_ZB_TEX_TERR2;
            }

            if (child->Attribute("G_RM_AA_ZB_SUB_TERR", 0)) {
                data |= G_RM_AA_ZB_SUB_TERR;
            }

            if (child->Attribute("G_RM_AA_ZB_SUB_TERR2", 0)) {
                data |= G_RM_AA_ZB_SUB_TERR2;
            }

            g = gsSPSetOtherMode(cmdVal, sft, length, data);
        } else if (childName == "LoadTextureBlock") {
            uint32_t fmt = child->IntAttribute("Format");
            uint32_t siz = child->IntAttribute("Size");
            uint32_t width = child->IntAttribute("Width");
            uint32_t height = child->IntAttribute("Height");
            uint32_t maskS = child->IntAttribute("MaskS");
            uint32_t maskT = child->IntAttribute("MaskT");
            uint32_t shiftS = child->IntAttribute("ShiftS");
            uint32_t shiftT = child->IntAttribute("ShiftT");
            uint32_t cms = 0;
            uint32_t cmt = 0;

            if (child->Attribute("CMS_TXMirror", 0)) {
                cms |= G_TX_MIRROR;
            }

            if (child->Attribute("CMS_TXNoMirror", 0)) {
                cms |= G_TX_NOMIRROR;
            }

            if (child->Attribute("CMS_TXWrap", 0)) {
                cms |= G_TX_WRAP;
            }

            if (child->Attribute("CMS_TXClamp", 0)) {
                cms |= G_TX_CLAMP;
            }

            if (child->Attribute("CMT_TXMirror", 0)) {
                cmt |= G_TX_MIRROR;
            }

            if (child->Attribute("CMT_TXNoMirror", 0)) {
                cmt |= G_TX_NOMIRROR;
            }

            if (child->Attribute("CMT_TXWrap", 0)) {
                cmt |= G_TX_WRAP;
            }

            if (child->Attribute("CMT_TXClamp", 0)) {
                cmt |= G_TX_CLAMP;
            }

            std::string fName = child->Attribute("Path");
            // fName = ">" + fName;

            Gfx g2[7];

            if (siz == 0) {
                Gfx g3[7] = { gsDPLoadTextureBlock(0, fmt, G_IM_SIZ_4b, width, height, 0, cms, cmt, maskS, maskT,
                                                   shiftS, shiftT) };
                memcpy(g2, g3, 7 * sizeof(Gfx));
            } else if (siz == 1) {
                Gfx g3[7] = { gsDPLoadTextureBlock(0, fmt, G_IM_SIZ_8b, width, height, 0, cms, cmt, maskS, maskT,
                                                   shiftS, shiftT) };
                memcpy(g2, g3, 7 * sizeof(Gfx));
            } else if (siz == 2) {
                Gfx g3[7] = { gsDPLoadTextureBlock(0, fmt, G_IM_SIZ_16b, width, height, 0, cms, cmt, maskS, maskT,
                                                   shiftS, shiftT) };
                memcpy(g2, g3, 7 * sizeof(Gfx));
            } else if (siz == 3) {
                Gfx g3[7] = { gsDPLoadTextureBlock(0, fmt, G_IM_SIZ_32b, width, height, 0, cms, cmt, maskS, maskT,
                                                   shiftS, shiftT) };
                memcpy(g2, g3, 7 * sizeof(Gfx));
            }

            g = { gsDPSetTextureImage(fmt, siz, width + 1, 0) };
            g.words.w0 &= 0x00FFFFFF;
            g.words.w0 += (G_SETTIMG_OTR_FILEPATH << 24);
            g.words.w1 = (uintptr_t)malloc(fName.size() + 1);
            strcpy((char*)g.words.w1, fName.data());

            dl->Instructions.push_back(g);

            for (int j = 1; j < 7; j++) {
                dl->Instructions.push_back(g2[j]);
            }

            g = gsDPPipeSync();
        } else if (childName == "EndDisplayList") {
            g = gsSPEndDisplayList();
        } else if (childName == "CullDisplayList") {
            uint32_t start = child->IntAttribute("Start");
            uint32_t end = child->IntAttribute("End");

            g = gsSPCullDisplayList(start, end);
        } else if (childName == "ClipRatio") {
            uint32_t ratio = child->IntAttribute("Start");
            Gfx g2[4];

            switch (ratio) {
                case 1: {
                    Gfx g3[4] = { gsSPClipRatio(FRUSTRATIO_1) };
                    memcpy(g2, g3, sizeof(Gfx) * 4);
                } break;
                case 2: {
                    Gfx g3[4] = { gsSPClipRatio(FRUSTRATIO_2) };
                    memcpy(g2, g3, sizeof(Gfx) * 4);
                } break;
                case 3: {
                    Gfx g3[4] = { gsSPClipRatio(FRUSTRATIO_3) };
                    memcpy(g2, g3, sizeof(Gfx) * 4);
                } break;
                case 4: {
                    Gfx g3[4] = { gsSPClipRatio(FRUSTRATIO_4) };
                    memcpy(g2, g3, sizeof(Gfx) * 4);
                } break;
                case 5: {
                    Gfx g3[4] = { gsSPClipRatio(FRUSTRATIO_5) };
                    memcpy(g2, g3, sizeof(Gfx) * 4);
                } break;
                case 6: {
                    Gfx g3[4] = { gsSPClipRatio(FRUSTRATIO_6) };
                    memcpy(g2, g3, sizeof(Gfx) * 4);
                } break;
            }

            for (int j = 0; j < 4; j++) {
                dl->Instructions.push_back(g2[j]);
            }

        } else if (childName == "JumpToDisplayList") {
            std::string dlPath = (char*)child->Attribute("Path");
            if (dlPath[0] == '>' && dlPath[1] == '0' && (dlPath[2] == 'x' || dlPath[2] == 'X')) {
                uint32_t seg = std::stoul(dlPath.substr(1), nullptr, 16);
                g = { gsSPBranchListOTRHash(seg | 1) };
            } else {
                char* dlPath2 = (char*)malloc(strlen(dlPath.c_str()) + 1);
                strcpy(dlPath2, dlPath.c_str());

                g = gsSPBranchListOTRFilePath(dlPath2);
            }
        } else if (childName == "CallDisplayList") {
            std::string dlPath = (char*)child->Attribute("Path");
            if (dlPath[0] == '>' && dlPath[1] == '0' && (dlPath[2] == 'x' || dlPath[2] == 'X')) {
                uint32_t seg = std::stoul(dlPath.substr(1), nullptr, 16);
                g = { gsSPDisplayList(seg | 1) };
            } else {
                char* dlPath2 = (char*)malloc(strlen(dlPath.c_str()) + 1);
                strcpy(dlPath2, dlPath.c_str());

                g = gsSPDisplayListOTRFilePath(dlPath2);
            }
        } else if (childName == "ClearGeometryMode" || childName == "SetGeometryMode") {
            uint64_t clearData = 0;

            if (child->Attribute("G_SHADE", 0)) {
                clearData |= G_SHADE;
            }

            if (child->Attribute("G_LIGHTING", 0)) {
                clearData |= G_LIGHTING;
            }

            if (child->Attribute("G_SHADING_SMOOTH", 0)) {
                clearData |= G_SHADING_SMOOTH;
            }

            if (child->Attribute("G_ZBUFFER", 0)) {
                clearData |= G_ZBUFFER;
            }

            if (child->Attribute("G_TEXTURE_GEN", 0)) {
                clearData |= G_TEXTURE_GEN;
            }

            if (child->Attribute("G_TEXTURE_GEN_LINEAR", 0)) {
                clearData |= G_TEXTURE_GEN_LINEAR;
            }

            if (child->Attribute("G_CULL_BACK", 0)) {
                clearData |= G_CULL_BACK;
            }

            if (child->Attribute("G_CULL_FRONT", 0)) {
                clearData |= G_CULL_FRONT;
            }

            if (child->Attribute("G_CULL_BOTH", 0)) {
                clearData |= G_CULL_BOTH;
            }

            if (child->Attribute("G_FOG", 0)) {
                clearData |= G_FOG;
            }

            if (child->Attribute("G_CLIPPING", 0)) {
                clearData |= G_CLIPPING;
            }

            if (childName == "ClearGeometryMode") {
                g = gsSPClearGeometryMode(clearData);
            } else {
                g = gsSPSetGeometryMode(clearData);
            }
        } else if (childName == "LightColor") {
            int n = child->IntAttribute("N");
            uint32_t col = child->IntAttribute("Col");

            Gfx g2[2];

            switch (n) {
                case 1: {
                    Gfx g3[2] = { gsSPLightColor(LIGHT_1, col) };
                    memcpy(g2, g3, sizeof(Gfx) * 2);
                } break;
                case 2: {
                    Gfx g3[2] = { gsSPLightColor(LIGHT_2, col) };
                    memcpy(g2, g3, sizeof(Gfx) * 2);
                } break;
                case 3: {
                    Gfx g3[2] = { gsSPLightColor(LIGHT_3, col) };
                    memcpy(g2, g3, sizeof(Gfx) * 2);
                } break;
                case 4: {
                    Gfx g3[2] = { gsSPLightColor(LIGHT_4, col) };
                    memcpy(g2, g3, sizeof(Gfx) * 2);
                } break;
                case 5: {
                    Gfx g3[2] = { gsSPLightColor(LIGHT_5, col) };
                    memcpy(g2, g3, sizeof(Gfx) * 2);
                } break;
                case 6: {
                    Gfx g3[2] = { gsSPLightColor(LIGHT_6, col) };
                    memcpy(g2, g3, sizeof(Gfx) * 2);
                } break;
                case 7: {
                    Gfx g3[2] = { gsSPLightColor(LIGHT_7, col) };
                    memcpy(g2, g3, sizeof(Gfx) * 2);
                } break;
                case 8: {
                    Gfx g3[2] = { gsSPLightColor(LIGHT_8, col) };
                    memcpy(g2, g3, sizeof(Gfx) * 2);
                } break;
            }

            for (int j = 0; j < 2; j++) {
                dl->Instructions.push_back(g2[j]);
            }

        } else if (childName == "SetRenderMode") {
            std::string rawMode1 = child->Attribute("Mode1");
            std::string rawMode2 = child->Attribute("Mode2");
            g = gsDPSetRenderMode(renderModes[rawMode1], renderModes[rawMode2]);
        } else {
            printf("DisplayListXML: Unknown node %s\n", childName.c_str());
            g = gsDPPipeSync();
        }

        dl->Instructions.push_back(g);

        child = child->NextSiblingElement();
    }
}

uint32_t DisplayListFactoryV0::GetCombineLERPValue(std::string valStr) {
    std::string strings[] = { "G_CCMUX_COMBINED",
                              "G_CCMUX_TEXEL0",
                              "G_CCMUX_TEXEL1",
                              "G_CCMUX_PRIMITIVE",
                              "G_CCMUX_SHADE",
                              "G_CCMUX_ENVIRONMENT",
                              "G_CCMUX_1",
                              "G_CCMUX_NOISE",
                              "G_CCMUX_0",
                              "G_CCMUX_CENTER",
                              "G_CCMUX_K4",
                              "G_CCMUX_SCALE",
                              "G_CCMUX_COMBINED_ALPHA",
                              "G_CCMUX_TEXEL0_ALPHA",
                              "G_CCMUX_TEXEL1_ALPHA",
                              "G_CCMUX_PRIMITIVE_ALPHA",
                              "G_CCMUX_SHADE_ALPHA",
                              "G_CCMUX_ENV_ALPHA",
                              "G_CCMUX_LOD_FRACTION",
                              "G_CCMUX_PRIM_LOD_FRAC",
                              "G_CCMUX_K5",
                              "G_ACMUX_COMBINED",
                              "G_ACMUX_TEXEL0",
                              "G_ACMUX_TEXEL1",
                              "G_ACMUX_PRIMITIVE",
                              "G_ACMUX_SHADE",
                              "G_ACMUX_ENVIRONMENT",
                              "G_ACMUX_1",
                              "G_ACMUX_0",
                              "G_ACMUX_LOD_FRACTION",
                              "G_ACMUX_PRIM_LOD_FRAC" };
    uint32_t values[] = { G_CCMUX_COMBINED,
                          G_CCMUX_TEXEL0,
                          G_CCMUX_TEXEL1,
                          G_CCMUX_PRIMITIVE,
                          G_CCMUX_SHADE,
                          G_CCMUX_ENVIRONMENT,
                          G_CCMUX_1,
                          G_CCMUX_NOISE,
                          G_CCMUX_0,
                          G_CCMUX_CENTER,
                          G_CCMUX_K4,
                          G_CCMUX_SCALE,
                          G_CCMUX_COMBINED_ALPHA,
                          G_CCMUX_TEXEL0_ALPHA,
                          G_CCMUX_TEXEL1_ALPHA,
                          G_CCMUX_PRIMITIVE_ALPHA,
                          G_CCMUX_SHADE_ALPHA,
                          G_CCMUX_ENV_ALPHA,
                          G_CCMUX_LOD_FRACTION,
                          G_CCMUX_PRIM_LOD_FRAC,
                          G_CCMUX_K5,
                          G_ACMUX_COMBINED,
                          G_ACMUX_TEXEL0,
                          G_ACMUX_TEXEL1,
                          G_ACMUX_PRIMITIVE,
                          G_ACMUX_SHADE,
                          G_ACMUX_ENVIRONMENT,
                          G_ACMUX_1,
                          G_ACMUX_0,
                          G_ACMUX_LOD_FRACTION,
                          G_ACMUX_PRIM_LOD_FRAC };

    for (int i = 0; i < ARRAY_COUNT(values); i++) {
        if (valStr == strings[i]) {
            return values[i];
        }
    }

    return G_CCMUX_1;
}

} // namespace LUS
