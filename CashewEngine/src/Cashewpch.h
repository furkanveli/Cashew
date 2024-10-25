#pragma once

#ifdef CASHEW_PLATFORM_WINDOWS
#include <Windows.h>
#endif

// standard library
#include <memory>
#include <iostream>
#include <sstream>
#include <optional>
#include <vector>
#include <string>
#include <sstream>
#include <source_location>
#include <variant>
#include <exception>
#include <queue>
#include <bitset>

// DirectX12
#include <d3d12.h>
#include "Engine/d3dx12.h"
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include "d3d12sdklayers.h"
#include "dxgidebug.h"
#include <DirectXMath.h>
#include "DirectXColors.h"
#include <wrl.h>

// project specific headers
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "Engine/Macros.h"
#include "Engine/CashewError.h"
#include "Engine/Log.h"
#include "Engine/CashewTimer.h"