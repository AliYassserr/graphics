# graphics

Tower defense game (OpenGL 3.3, GLFW, glad, glm, FreeType, miniaudio).

Text is drawn with FreeType as in LearnOpenGL's "Text Rendering" tutorial: every glyph is rendered to a bitmap texture
and drawn on a quad. The font is `fonts/DejaVuSans.ttf` (Bitstream Vera licence, see `fonts/DejaVu-LICENSE.txt`).
Run the game from the project folder so `fonts/` is found. If no font loads, the game still runs and shows icons.

## Build

macOS:

    brew install glm glfw freetype
    clang++ -std=c++17 main.cpp src/glad.c -Iinclude -I$(brew --prefix)/include -I$(brew --prefix)/include/freetype2 -L$(brew --prefix)/lib -lglfw -lfreetype -framework OpenGL -framework Cocoa -framework IOKit -o game && ./game

Linux:

    sudo apt install libglfw3-dev libfreetype-dev
    g++ -std=c++17 main.cpp src/glad.c -Iinclude -IExternal/include -I/usr/include/freetype2 -lglfw -lfreetype -ldl -lpthread -o game && ./game

Windows (Visual Studio): install FreeType with vcpkg (`vcpkg install freetype:x64-windows`, then `vcpkg integrate install`),
open `Template.sln`, pick x64 and build. The project folder must be named `Template`.
