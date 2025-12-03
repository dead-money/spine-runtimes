#ifndef URP_LIT_INPUT_INCLUDED
#define URP_LIT_INPUT_INCLUDED

#include "Packages/com.unity.render-pipelines.core/ShaderLibrary/Common.hlsl"

////////////////////////////////////////
// Defines
//
#undef LIGHTMAP_ON

CBUFFER_START(UnityPerMaterial)
    float4 _MainTex_ST;
    half _Cutoff;
    half4 _Color;
    half4 _Black;
    // Dead Money
    // Slot Count
    half _SlotCount;
    // Dead Money
CBUFFER_END

sampler2D _MainTex;
// Dead Money
// Support for slot color lookup table.
sampler2D _SlotColorLUT;
// Dead Money

#endif // URP_LIT_INPUT_INCLUDED
