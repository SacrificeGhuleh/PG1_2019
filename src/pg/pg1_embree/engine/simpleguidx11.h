#pragma once

#include <array>

#include <engine/simpleguidx11.h>
#include <utils/structs.h>
#include <math/vector.h>

class SimpleGuiDX11 {
public:
  SimpleGuiDX11(int width, int height);
  
  ~SimpleGuiDX11();
  
  int MainLoop();
  
  static float producerTime;
protected:
  
  enum class MultiRay {
    PIXEL_1x1,
    PIXEL_2x2,
    PIXEL_2x4,
    PIXEL_4x4
  };
  
  MultiRay multiRay_;
  
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
  
  virtual Color4f get_pixel(int x, int y, float t = 0.0f);
  
  virtual std::array<Color4f, 4> get_pixel4(int x, int y, float t = 0.0f);
  
  virtual std::array<Color4f, 8> get_pixel8(int x, int y, float t = 0.0f);
  
  virtual std::array<Color4f, 16> get_pixel16(int x, int y, float t = 0.0f);
  
  void Producer();
  
  int width() const;
  
  int height() const;
  
  bool vsync_{true};
private:
  WNDCLASSEX wc_;
  
  HWND hwnd_;
  ID3D11Device *g_pd3dDevice{nullptr};
  ID3D11DeviceContext *g_pd3dDeviceContext{nullptr};
  IDXGISwapChain *g_pSwapChain{nullptr};
  
  ID3D11RenderTargetView *g_mainRenderTargetView{nullptr};
  ID3D11Texture2D *tex_id_{nullptr};
  ID3D11ShaderResourceView *tex_view_{nullptr};
  int width_{640};
  int height_{480};
  float *tex_data_{nullptr}; // DXGI_FORMAT_R32G32B32A32_FLOAT
  std::mutex tex_data_lock_;
  
  std::atomic<bool> finish_request_{false};
};
