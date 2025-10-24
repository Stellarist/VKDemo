# VKEngine

A toy engine in Vulkan.

┌─────────────────────────────────────────┐
│          Application Layer              │
│         (main.cpp, game logic)          │
└──────────────────┬──────────────────────┘
                   │
┌──────────────────▼──────────────────────┐
│           Scene Graph Layer             │
│    (scene/, Node, SubMesh, Material)    │
└──────────────────┬──────────────────────┘
                   │
┌──────────────────▼──────────────────────┐
│      Render Layer - High Level          │
│       (render/Renderer.hpp/cpp)         │
└──────────────────┬──────────────────────┘
                   │
        ┌──────────┴──────────┐
        │                     │
┌───────▼────────┐   ┌────────▼────────┐
│  RHI Layer     │   │ Graphics Layer  │
│ (render/rhi/)  │   │(render/graphics)│
│                │   │                 │
│ • GpuMesh      │   │ • Context       │
│ • GpuMaterial  │   │ • Buffer        │
│ • GpuTexture   │   │ • Image         │
└────────┬───────┘   └─────────┬───────┘
         │                     │
         └──────────┬──────────┘
                    │
         ┌──────────▼──────────┐
         │    Vulkan API       │
         └─────────────────────┘
