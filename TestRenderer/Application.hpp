#pragma once

#define NOMINMAX
#include <windows.h>
#include "Renderer.hpp"
#include "PrismEffect.hpp"

class FApplication
{
public:
	EErrorCode Setup(const HWND HWnd, const uint32_t Width, const uint32_t Height);

	void OnRender() noexcept;
	void OnUpdate(const float Time) noexcept;
	void OnGui() noexcept;
	
	void TearDown() noexcept;
	void Resize(const uint32_t Width, const uint32_t Height) const noexcept;
	EErrorCode Present() const noexcept;

	static double GetHighResolutionTime() noexcept;

private:
	SRenderTarget BackBuffer{};
	FRenderer Renderer{};
	FPrismEffect PrismEffect{ Renderer };
};
