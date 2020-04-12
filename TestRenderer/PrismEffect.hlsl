SamplerState Sampler : register(s0);
Texture2D Texture:register(t0);

cbuffer Parameters : register(b0)
{
	float2 Resolution;
	float2 TextureSize;
	float2 Offset;
	float SmoothingDistance;
}

float AspectFix(const float2 ScreenResolution, const float2 TextureResolution)
{
	float Aspect = ScreenResolution.x / ScreenResolution.y;
	float TextureAspect = TextureResolution.x / TextureResolution.y;

	return Aspect / TextureAspect;
}

float4 SampleImageWithRedShift(float2 TexCoord)
{
	float R = Texture.Sample(Sampler, TexCoord + Offset).r;
	float3 GBA = Texture.Sample(Sampler, TexCoord).gba;

	return float4(R, GBA);
}

float2 TransformTexCoord(float2 TexCoord, float2 TransformOffset, float Scale)
{
	float2 Temp = TexCoord;

	Temp -= 0.5;
	Temp *= Scale;
	Temp += TransformOffset;
	Temp += 0.5;

	return Temp;
}

float2 LineNormal(float2 A, float2 B)
{
	float2 N = B - A;
	
	float2 Temp = N;
	N.x = Temp.y;
	N.y = -Temp.x;
	
	return normalize(N);
}

float4 main(float4 Position : SV_POSITION, float2 TexCoord : TEXCOORD0) : SV_TARGET0
{
	float2 OrigTexCoord = TexCoord;
	float AspectFixRatio = AspectFix(Resolution, TextureSize);
	TexCoord.x -= 0.5;
	TexCoord.x *= AspectFixRatio;
	TexCoord.x += 0.5;

	float2 TopLeftUV = TransformTexCoord(TexCoord, float2(0.25, 0.4), 1.6666666666);
	float2 TopRightUV = TransformTexCoord(TexCoord, float2(-0.25, 0.4), 1.6666666666);

	//The -float2(0.0, 0.2) is for "centering" the image more to the bottom. It is still sampled at 100%, I just offsetted
	//it down, so it looks better with my hairy head. Feel free to delete it, as I dunno if this was part of the assignment
	float4 BottomSample = SampleImageWithRedShift(TexCoord - float2(0.0, 0.20));
	float4 TopLeftSample = SampleImageWithRedShift(TopLeftUV);
	float4 TopRightSample = SampleImageWithRedShift(TopRightUV);

	float4 Rect = lerp(TopLeftSample, TopRightSample, smoothstep(0.5 - SmoothingDistance, 0.5 + SmoothingDistance, OrigTexCoord.x));
	
	//these can be precalculated, because they are constants
	float TriangleY = 0.3;
	float2 A1 = float2(0.0, TriangleY + 0.2);
	float2 B1 = float2(0.5, TriangleY);
	float2 A2 = float2(1.0, TriangleY + 0.2);

	//these too
	float2 N1 = LineNormal(A1, B1);
	float2 N2 = LineNormal(B1, A2);

	float LineDist1 = dot(OrigTexCoord, N1) - dot(A1, N1);
	float LineDist2 = dot(OrigTexCoord, N2) - dot(A2, N2);
	float T = smoothstep(SmoothingDistance, -SmoothingDistance, LineDist2) * smoothstep(SmoothingDistance, -SmoothingDistance, LineDist1);
	return lerp(Rect, BottomSample, T);
}