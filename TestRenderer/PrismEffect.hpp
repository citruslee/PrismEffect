#pragma once
#include "Renderer.hpp"
#include <d3d11.h>


class FPrismEffect
{
private:
	struct SPrismParams
	{
		float XResolution = 1080.0f;
		float YResolution = 1920.0f;
		float XTextureSize = 1.0f;
		float YTextureSize = 1.0f;
		float XOffset = 0.003f;
		float YOffset = 0.003f;
		float SmoothingDistance = 0.05f;
	};
public:
	explicit FPrismEffect(FRenderer& Renderer);

	~FPrismEffect();

	EErrorCode Initialize(const uint32_t Width, const uint32_t Height) noexcept;

	void OnGui() noexcept;

	void OnRender(const SRenderTarget* RenderTargets, const size_t Count) const noexcept;

	void ReloadTexture(const DXGI_FORMAT Format, SRenderTarget& RenderTarget) noexcept;

	void ReloadTexture(const char* FileName, const DXGI_FORMAT Format, SRenderTarget& RenderTarget) noexcept;

	const SRenderTarget& GetResult() const noexcept;

private:
	FRenderer& InternalRenderer;
	SShader PrismShader{};

	SPrismParams PrismParams{};
	SBuffer PrismConstantBuffer{};
	
	SRenderTarget FinalRenderTarget{};
	SRenderTarget InputTexture{};
};
