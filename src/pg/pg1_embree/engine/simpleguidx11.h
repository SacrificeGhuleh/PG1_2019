#pragma once

#include <array>

#include <glm/vec4.hpp>

#include <engine/simpleguidx11.h>
#include <utils/structs.h>


class SimpleGuiDX11 {
public:
  SimpleGuiDX11(int width, int height);
  
  virtual ~SimpleGuiDX11();
  
  int MainLoop();
  
  static float producerTime;
protected:
  
  int Init();
  
  int Cleanup();
  
  void CreateRenderTarget();
  
  void CleanupRenderTarget();
  
  HRESULT CreateDeviceD3D(HWND hWnd);
  
  void CleanupDeviceD3D();
  
  HRESULT CreateTexture();
  
  LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
  
  static LRESULT CALLBACK s_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  
  virtual int Ui();
  
  virtual glm::vec4 get_pixel(int x, int y, float t = 0.0f);
  
  void Producer();
  
  int width() const;
  
  int height() const;
  
  bool vsync_{true};
protected:
  int width_{640};
  int height_{480};
  float *tex_data_{nullptr}; // DXGI_FORMAT_R32G32B32A32_FLOAT
  std::mutex tex_data_lock_;
  
  static float gamma_;

private:
  WNDCLASSEX wc_;
  
  HWND hwnd_;
  ID3D11Device *g_pd3dDevice{nullptr};
  ID3D11DeviceContext *g_pd3dDeviceContext{nullptr};
  IDXGISwapChain *g_pSwapChain{nullptr};
  
  ID3D11RenderTargetView *g_mainRenderTargetView{nullptr};
  ID3D11Texture2D *tex_id_{nullptr};
  ID3D11ShaderResourceView *tex_view_{nullptr};
  
  std::atomic<bool> finish_request_{false};
};
