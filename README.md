# Simple Physics Engine

A small 2D physics sandbox written in C++ with [SFML](https://www.sfml-dev.org/). Click to spawn balls of random size and color, and watch them bounce off the walls and off each other in elastic collisions. A **quadtree** speeds up collision detection, and the program draws it live so you can see how it divides the screen.

## Features

- **Spawn balls with the mouse.** Hold the left mouse button to add one ball per frame at the cursor.
- **Random properties.** Each ball gets a radius of 3–10 px, a mass proportional to its radius, one of five colors (green, red, blue, yellow, magenta) and a random diagonal starting velocity (30–50 px/s on each axis).
- **Elastic ball-to-ball collisions.** Velocities are split into a normal and a tangential part. The 1D elastic collision formula is applied along the normal, and the tangential part is kept. Overlapping balls are pushed apart first so they don't stick together.
- **Wall bounces.** Balls reflect off the window edges with no energy loss.
- **Quadtree broad phase.** The quadtree is rebuilt every frame (node capacity 4) and queried for nearby balls, so the program avoids checking every pair, which would be O(n²).
- **Visualisation.** The quadtree cells are drawn as faint outlines while there are fewer than 1000 balls, and the ball count is shown in the top-right corner.

## Controls

| Input                 | Action                       |
| --------------------- | ---------------------------- |
| Hold left mouse button | Spawn balls at the cursor   |
| Close the window      | Quit                         |

## Project layout

```
Simple_Physics_Engine/
├── physics.sln              # Visual Studio solution
├── physics/
│   ├── main.cpp             # All of the engine and rendering code
│   ├── physics.vcxproj      # Visual Studio project
│   ├── sfml-*.dll, openal32.dll   # SFML runtime DLLs (release and -d debug)
│   └── *.ttf                # Fonts (arial.ttf is loaded at runtime)
└── External/SFML/           # Bundled SFML 2.6.1 headers and .lib files
```

## How it works

All of the code is in [`physics/main.cpp`](physics/main.cpp).

| Piece | What it does |
| --- | --- |
| `Point` | A ball: position, velocity, radius, mass and color. |
| `update(Point&)` | Moves the ball with a fixed time step of `1/60` s (explicit Euler) and reflects its velocity when it hits a wall. |
| `collision(Point&, Point&)` | Resolves overlap and applies the 2D elastic collision equations (see [this write-up](https://www.vobarian.com/collisions/2dcollisions2.pdf)). |
| `Rec` | An axis-aligned rectangle stored as center + half-width/half-height, with `contains` and `intersects`. |
| `Quadtree` | A point quadtree with `insert`, `subdivide`, `query` (range search) and `show` (draws its cells). |

Each frame of the main loop:

1. Spawn a ball if the left mouse button is held down.
2. Build a fresh quadtree covering the 800×800 window and insert every ball.
3. For each ball, query the quadtree for neighbours in a small box beside it, and resolve a collision with any neighbour whose distance is ≤ the sum of the two radii.
4. Move each ball (handling wall bounces) and draw it.
5. Draw the quadtree and the ball counter, then display the frame (capped at 60 FPS).

## Building and running

The project targets **Windows** and **Visual Studio 2022** (toolset v143). The code includes `<windows.h>`, and SFML 2.6.1 is bundled in `External/SFML`, so there is nothing else to install.

1. Open `physics.sln` in Visual Studio 2022.
2. Choose the **x86** (Win32) platform and either **Debug** or **Release**. Only the Win32 configurations are set up with the SFML include and library paths.
3. **Fix the library path.** In *Project → Properties → Linker → General → Additional Library Directories*, the path is hard-coded to `C:\Users\PC\source\repos\physics\External\SFML\lib`. Unless your checkout is in that exact folder, replace it with:
   ```
   $(SolutionDir)External\SFML\lib
   ```
4. Build and run (F5 or Ctrl+F5).

The program loads `arial.ttf` and the SFML DLLs from its working directory. Visual Studio uses the project folder (`physics/`) by default, and that folder already contains these files. If you run the built `.exe` from somewhere else, copy `arial.ttf` and the matching `sfml-*-2.dll` files (`-d` versions for Debug) and `openal32.dll` next to it.

## Known limitations

- Integration is explicit Euler with a fixed time step, so very fast or crowded balls can tunnel through or jitter.
- The neighbour search uses a fixed-size box to the right of each ball (about 20×40 px). This works for the current maximum radius of 10 px, but larger balls would need a larger box.
- The quadtree's child nodes are allocated with `new` and never freed. Because the tree is rebuilt every frame, memory use grows the longer the program runs.
- No gravity yet, despite the window title "Gravity Circle".
- Windows only for now.

## References

- 2D elastic collisions: <https://www.vobarian.com/collisions/2dcollisions2.pdf>
- SFML documentation: <https://www.sfml-dev.org/documentation/2.6.1/>
