# Teddy Engine

<p align="center">
  <img src="https://via.placeholder.com/400x200.png?text=Teddy+Engine" alt="Teddy Engine Logo">
</p>

<p align="center">
    <a href="https://github.com/your-username/Teddy/blob/main/LICENSE"><img src="https://img.shields.io/badge/License-MIT-blue.svg" alt="License"></a>
    <a href="#"><img src="https://img.shields.io/badge/Platform-Windows-informational.svg" alt="Platform"></a>
    <a href="#"><img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg" alt="C++17"></a>
</p>

Teddy is a 2D game engine currently in development. It provides a foundation for creating games with C# scripting capabilities and a dedicated editor application.

## Features

*   **2D Rendering:** A flexible 2D renderer with support for sprites, textures, and custom shaders.
*   **C# Scripting:** Write game logic in C# using the integrated Mono runtime. The scripting API provides access to engine features like input, scene management, and entity manipulation.
*   **Entity-Component System (ECS):** A modern and data-oriented architecture for managing game objects, powered by the high-performance EnTT library.
*   **Physics:** 2D physics simulation using the popular Box2D library.
*   **Editor:** A dedicated editor application (`TeddEditor`) for scene creation, object manipulation, and script management. The editor is built with ImGui and provides a user-friendly interface for game development.
*   **💻 Cross-Platform (in progress):** While currently targeting Windows, the use of cross-platform libraries like GLFW suggests that support for other platforms is planned.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

*   **IDE:** Visual Studio 2019 or newer is recommended.
*   **Premake:** Download Premake5 from [https://premake.github.io/](https://premake.github.io/) and place it in the `vendor/premake` directory or in a directory included in your system's PATH.
*   **Vulkan SDK:** The project has some Vulkan dependencies. Download and install the Vulkan SDK from [https://www.lunarg.com/vulkan-sdk/](https://www.lunarg.com/vulkan-sdk/). After installation, make sure the `VULKAN_SDK` environment variable is set to the SDK's installation path.

### Cloning

Clone the repository to your local machine:

```sh
git clone https://github.com/your-username/Teddy.git
cd Teddy
```

### Building

1.  **Generate Project Files:** Run the `Win-GenProjects.bat` script in the `scripts` directory. This will use Premake to generate the Visual Studio solution and project files.
2.  **Open the Solution:** Open the generated `Teddy.sln` file in Visual Studio.
3.  **Build the Projects:** Build the solution in Visual Studio. The default startup project is `TeddEditor`.



## Roadmap

Here are some of the features and improvements planned for the future:

*   [ ] **Animation System:** A system for creating and managing 2D animations.
*   [ ] **Audio System:** Integration of an audio library for sound effects and music.
*   [ ] **UI System:** A dedicated UI system for creating in-game user interfaces.
*   [ ] **Linux and macOS Support:** Expanding the engine to be fully cross-platform.
*   [ ] **Vulkan Renderer:** Completing the implementation of the Vulkan rendering backend.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
