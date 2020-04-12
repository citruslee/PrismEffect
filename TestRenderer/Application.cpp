#include "Application.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "Renderer.hpp"
#include <DirectXMath.h>
#include <chrono>

EErrorCode FApplication::Setup(const HWND HWnd, const uint32_t Width, const uint32_t Height)
{
	auto Result = Renderer.CreateDeviceAndSwapchainForHwnd(HWnd, Width, Height, BackBuffer);
	Result = PrismEffect.Initialize(Width, Height);
	Result = Renderer.InitializeImGui();

	return EErrorCode::OK;
}

void FApplication::OnRender() noexcept
{
	PrismEffect.OnRender(nullptr, 0);
	Renderer.SetRenderTarget(BackBuffer);
	Renderer.ClearRenderTarget(BackBuffer, DirectX::XMFLOAT4(0.0f, 0.2f, 0.4f, 1.0f));
}

void FApplication::OnUpdate(const float Time) noexcept
{
}

void FApplication::OnGui() noexcept
{

	ImGui::Begin("Render Window");
	{
		PrismEffect.OnGui();
		auto& Io = ImGui::GetIO();
		if(ImGui::IsWindowHovered())
		{
			Io.WantCaptureMouse = Io.WantCaptureKeyboard = true;
		}
		else
		{
			Io.WantCaptureMouse = Io.WantCaptureKeyboard = false;
		}
		const auto& RenderTarget = PrismEffect.GetResult();
		const auto AspectRatio = static_cast<float>(RenderTarget.Width) / static_cast<float>(RenderTarget.Height);
		const auto WindowSize = ImGui::GetWindowSize();
		auto CursorPosition = ImGui::GetCursorPos();
		auto RenderSize = WindowSize;

		RenderSize.x -= CursorPosition.x * 2;
		RenderSize.y -= CursorPosition.y * 2;

		if (RenderSize.x < RenderSize.y * AspectRatio)
		{
			RenderSize.y = RenderSize.x / AspectRatio;
			CursorPosition.y = (WindowSize.y - RenderSize.y) * 0.5f;
		}
		else
		{
			RenderSize.x = RenderSize.y * AspectRatio;
			CursorPosition.x = (WindowSize.x - RenderSize.x) * 0.5f;
		}
		ImGui::SetCursorPos(CursorPosition);
		ImGui::Image(RenderTarget.ShaderResourceView, RenderSize);
	}
	ImGui::End();
}

void FApplication::TearDown() noexcept
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Renderer.DestroyRenderTarget(BackBuffer);
}

void FApplication::Resize(const uint32_t Width, const uint32_t Height) const noexcept
{
	Renderer.ResizeBackBuffer(Width, Height, BackBuffer);
}

EErrorCode FApplication::Present() const noexcept
{
	return Renderer.Present(1, 0);
}

double FApplication::GetHighResolutionTime() noexcept
{
	static const std::chrono::high_resolution_clock::time_point Start = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point Now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> TimeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(Now - Start);
	return TimeSpan.count();
}
