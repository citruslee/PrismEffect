#include "PrismEffect.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include <tchar.h>

FPrismEffect::FPrismEffect(FRenderer& Renderer) : InternalRenderer(Renderer)
{

}

FPrismEffect::~FPrismEffect()
{
	InternalRenderer.DestroyShader(PrismShader);
	InternalRenderer.DestroyTexture(InputTexture);
	InternalRenderer.DestroyRenderTarget(FinalRenderTarget);
	InternalRenderer.DestroyBuffer(PrismConstantBuffer);
}

EErrorCode FPrismEffect::Initialize(const uint32_t Width, const uint32_t Height) noexcept
{
	InternalRenderer.CreateRenderTarget(Width, Height, DXGI_FORMAT_R8G8B8A8_UNORM, FinalRenderTarget);
	ReloadTexture("Assets/face.jpg", DXGI_FORMAT_R8G8B8A8_UNORM, InputTexture);
#define CHECK_RESULT() \
	if (Result != EErrorCode::OK)\
	{\
		return Result;\
	}
	auto Result = InternalRenderer.CreateVertexShader(L"FullScreenTriangleVS.hlsl", "main", nullptr, 0, PrismShader);
	CHECK_RESULT();
	
	Result = InternalRenderer.CreatePixelShader(L"PrismEffect.hlsl", "main", PrismShader);
	CHECK_RESULT();
	
	Result = InternalRenderer.CreateConstantBufferWithData(PrismParams, PrismConstantBuffer);
	CHECK_RESULT();
#undef CHECK_RESULT

	return EErrorCode::OK;
}

void FPrismEffect::OnGui() noexcept
{
	if (ImGui::Button("Load Image"))
	{
		ReloadTexture(DXGI_FORMAT_R8G8B8A8_UNORM, InputTexture);
	}

	ImGui::SliderFloat2("Offset", static_cast<float*>(&PrismParams.XOffset), -0.03f, 0.03f);
	ImGui::SliderFloat("Smoothing Distance", static_cast<float*>(&PrismParams.SmoothingDistance), 0.00f, 0.5f);
}

void FPrismEffect::OnRender(const SRenderTarget* RenderTargets, const size_t Count) const noexcept
{
	InternalRenderer.SetRenderTarget(FinalRenderTarget);
	InternalRenderer.ClearRenderTarget(FinalRenderTarget, DirectX::XMFLOAT4(0.0f, 0.2f, 0.4f, 1.0f));
	InternalRenderer.SetViewport(FinalRenderTarget.Width, FinalRenderTarget.Height);
	InternalRenderer.SetShader(PrismShader);
	InternalRenderer.SetConstantBuffer(PrismConstantBuffer, EShaderStage::PIXEL);
	InternalRenderer.UpdateSubresource(PrismConstantBuffer, &PrismParams, sizeof(SPrismParams));
	InternalRenderer.SetTexture(0, InputTexture);
	InternalRenderer.SetConstantBuffer({ nullptr, 0 }, EShaderStage::VERTEX);
	InternalRenderer.SetVertexBuffer(0, { nullptr, 0 }, 0);
	InternalRenderer.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	InternalRenderer.Draw(3, 0);

	InternalRenderer.UnbindRenderTargets();
}

void FPrismEffect::ReloadTexture(const DXGI_FORMAT Format, SRenderTarget& RenderTarget)  noexcept
{
	TCHAR File[MAX_PATH] = { 0 };

	OPENFILENAME OpenFileName{};
	OpenFileName.lStructSize = sizeof(OpenFileName);
	OpenFileName.hwndOwner = nullptr;
	OpenFileName.lpstrFile = File;
	OpenFileName.nMaxFile = sizeof(File);
	OpenFileName.lpstrFilter = _T("(*.*) All\0*.*\0(*.png) PNG\0*.png\0(*.jpg) JPG\0*.jpg\0(*.dds) DDS\0*.dds\0");
	OpenFileName.nFilterIndex = 1;
	OpenFileName.lpstrFileTitle = nullptr;
	OpenFileName.nMaxFileTitle = 0;
	OpenFileName.lpstrInitialDir = nullptr;
	OpenFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&OpenFileName) == TRUE)
	{
		ReloadTexture(OpenFileName.lpstrFile, Format, RenderTarget);
	}
}

void FPrismEffect::ReloadTexture(const char* FileName, const DXGI_FORMAT Format,
	SRenderTarget& RenderTarget) noexcept
{
	InternalRenderer.DestroyTexture(RenderTarget);
	const auto Result = InternalRenderer.CreateTextureFromFile(FileName, Format, RenderTarget);
	PrismParams.XTextureSize = RenderTarget.Width;
	PrismParams.YTextureSize = RenderTarget.Height;
}

const SRenderTarget& FPrismEffect::GetResult() const noexcept
{
	return FinalRenderTarget;
}
