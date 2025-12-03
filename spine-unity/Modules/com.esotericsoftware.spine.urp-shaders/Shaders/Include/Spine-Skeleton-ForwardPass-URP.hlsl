#ifndef SKELETON_FORWARD_PASS_URP_INCLUDED
#define SKELETON_FORWARD_PASS_URP_INCLUDED

#include "Packages/com.unity.render-pipelines.universal/ShaderLibrary/Core.hlsl"
#include "Packages/com.unity.render-pipelines.core/ShaderLibrary/CommonMaterial.hlsl"
#include "Packages/com.esotericsoftware.spine.urp-shaders/Shaders/Include/SpineCoreShaders/Spine-Common.cginc"
#include "Packages/com.esotericsoftware.spine.urp-shaders/Shaders/Include/SpineCoreShaders/Spine-Skeleton-Tint-Common.cginc"

struct appdata {
	float3 pos : POSITION;
	half4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	
	// Dead Money
	// Support for two masks and slot data.
	float2 mask1uv : TEXCOORD1;
	float2 mask2uv : TEXCOORD2;
	float2 slotData : TEXCOORD3; // u: slot index, v: 0.0
	// Dead Money
	
	UNITY_VERTEX_INPUT_INSTANCE_ID
};

struct VertexOutput {
	half4 color : COLOR0;
	float2 uv0 : TEXCOORD0;
	
	// Dead Money
	// Support for two masks and slot data.
	float2 mask1uv : TEXCOORD1;
	float2 mask2uv : TEXCOORD2;
	float2 slotData : TEXCOORD3;
	// Dead Money
	
	float4 pos : SV_POSITION;
	UNITY_VERTEX_OUTPUT_STEREO
};

VertexOutput vert(appdata v) {
	VertexOutput o;
	UNITY_SETUP_INSTANCE_ID(v);
	UNITY_INITIALIZE_VERTEX_OUTPUT_STEREO(o);

	float3 positionWS = TransformObjectToWorld(v.pos);
	o.pos = TransformWorldToHClip(positionWS);
	o.uv0 = v.uv0;
	o.mask1uv = v.mask1uv;
	o.mask2uv = v.mask2uv;
	o.slotData = v.slotData;
	o.color = PMAGammaToTargetSpace(v.color);
#if defined (APPLY_MATERIAL_TINT_COLOR)
	o.color *= _Color;
#endif
	return o;
}

// Dead Money
// Mask Color Support
inline float4 srgbToLinear(float4 c)
{
	// A simple gamma ≈2.2 approximation.
	return float4(pow(c.rgb, 2.2), c.a);
}

inline float4 slotToLUTColor(float slotIndex, float colorIndex)
{
	float numSlots = _SlotCount;

	float W = numSlots * 4.0;
	float u = (slotIndex * 4 + colorIndex + 0.5) / (W);
	float2 uv = float2(u, 0.0);
	return tex2D(_SlotColorLUT, uv);
}

half4 frag(VertexOutput i) : SV_Target
{
	float slotIndex = i.slotData.x;

	float4 texColor = tex2D(_MainTex, i.uv0);
	float4 black = float4(0.0, 0.0, 0.0, texColor.a);
	float4 inkColor = float4(0.161, 0.114, 0.102, texColor.a);
	inkColor = srgbToLinear(inkColor);

	float4 mask1Color = tex2D(_MainTex, i.mask1uv);
	float4 mask2Color = tex2D(_MainTex, i.mask2uv);

	float4 lutColor1 = slotToLUTColor(slotIndex, 0);
	float4 lutColor2 = slotToLUTColor(slotIndex, 1);

	float4 colorA = lerp(black, lutColor1, mask1Color.r);
	float4 colorB = lerp(colorA, lutColor2, mask1Color.b);

	texColor = lerp(inkColor, colorB, mask1Color.r + mask1Color.b);

	#if defined(_ZWRITE)
	clip(texColor.a * i.color.a - _Cutoff);
	#endif
	
	#if defined(_STRAIGHT_ALPHA_INPUT)
	texColor.rgb *= texColor.a;
	#endif
	return (texColor * i.color);
}
// Dead Money

#endif
