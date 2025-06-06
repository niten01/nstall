# nstall

A utility for creating minimalistic GUI/CLI installers.

## Usage

You can download the latest Nstall release from the [Releases](#) page (binaries for Windows and Linux GUI modes are available).
The Nstall GUI mode supports Windows and Linux (X11/XWayland).
The CLI mode can be used on any platform.

If you want to use Nstall on a different platform, refer to the **Building from Source** section.

* Using GUI mode is straightforward—just launch `nstall-constructor`, and you're good to go.
* CLI mode is activated when any command-line arguments are passed to `nstall-constructor` (or if it was built in CLI-only mode).
  Use `-h` to see available options.

## Building from Source

Nstall uses the CMake build system, so the build process is standard.
All dependencies are fetched automatically. However, on Linux, you must have the X11 development libraries installed (`libx11-dev` and `libxft-dev` on Ubuntu). These are often pre-installed on many systems.

To build Nstall:

1. Navigate to the repository root and configure the CMake project:

   ```bash
   cmake -S . -B build
   ```

2. Then build the project:

   ```bash
   cmake --build build
   ```

To build in CLI-only mode (which also disables the X11 dependency), specify `NSTALL_CLI_ONLY=On` during configuration:

```bash
cmake -S . -B build -DNSTALL_CLI_ONLY=On
```
