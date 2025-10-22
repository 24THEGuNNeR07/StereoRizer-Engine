# StereoRizer Engine

A research-oriented 3D engine focused on exploring various stereo rendering techniques and optimizations based on academic papers. The primary goal is to investigate and implement methods to reduce GPU workload when producing stereo image pairs in OpenGL.

## Project Overview

StereoRizer Engine is designed to be a testbed for stereo rendering research, allowing easy implementation and comparison of different rendering techniques. The engine supports basic 3D model loading and OpenXR integration, making it suitable for VR development and research.

## Getting Started

### Prerequisites

- Visual Studio 2022 or later
- Windows 10/11 with OpenGL support
- OpenXR runtime (like SteamVR or Oculus)

### External Dependencies (included)
- GLFW (Window management)
- GLEW (OpenGL extension loading)
- Assimp (3D model loading)
- GLM (Mathematics)
- OpenXR (VR support)

### Building and Running

1. Clone the repository:
```bash
git clone https://github.com/24THEGuNNeR07/StereoRizer-Engine.git
```

2. Open `StereoRizerEngine/StereoRizerEngine.sln` in Visual Studio

3. Make sure the build configuration is set to x64 (Debug or Release)

4. Build the solution (F5 or Ctrl+B)

5. Run the application (F5)

### Project Structure

```
StereoRizerEngine/
├── include/                # Header files
│   ├── core/              # Core engine components
│   ├── graphics/          # Graphics-related headers
│   └── xr/               # OpenXR integration
├── src/                   # Source files
│   ├── core/             # Core implementation
│   ├── graphics/         # Graphics implementation
│   └── xr/              # OpenXR implementation
├── resources/            # Engine resources
│   └── shaders/         # GLSL shaders
└── models/              # 3D model files
```

## Current Features

- Basic 3D engine infrastructure
- OpenGL-based rendering pipeline
- 3D model loading via Assimp
  - Basic mesh loading
  - Vertex position support
- Shader system
  - Vertex/Fragment shader support
  - Basic Phong lighting
  - Runtime shader reloading
- Camera system with perspective projection
- OpenXR integration
  - Basic VR support
  - Stereo rendering
  - Head tracking
- Transform system
  - Translation
  - Rotation
  - Scale

## Planned Features and Improvements

### In Progress
1. Model Loading Enhancements
   - Fix missing normals in model loading
   - Add proper normal generation
   - Implement material system
   - Add texture coordinate support

2. Stereo Rendering Optimization
   - Primary eye selection functionality
   - Configurable secondary eye resolution
   - View-dependent rendering optimizations

3. GUI Integration
   - ImGui integration for runtime configuration
   - Real-time parameter adjustment
   - Performance metrics display
   - Eye configuration controls

### Future Development
- Implementation of various stereo rendering techniques
- Performance comparison tools
- Advanced optimization techniques
- Enhanced shader effects
- Multi-platform VR support

## Contributing

Contributions are welcome! To contribute:
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- OpenXR Working Group for the OpenXR specification
- Academic research papers on stereo rendering techniques
