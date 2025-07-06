# TERMINAX

A minimal terminal emulator for the X Window System featuring:

- Line wrapping based on window width
- Infinite vertical scrollback with Up/Down arrow keys
- Dynamic window resizing with automatic rewrapping
- Runs a shell  via pseudoterminal (PTY)
- Basic keyboard input forwarding and output rendering

---

## Table of Contents

- [Features](#features)
- [Demo](#demo)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
- [Usage](#usage)
- [Configuration](#configuration)
- [Limitations](#limitations)
- [Contributing](#contributing)
- [License](#license)

---

## Features

- Simple and lightweight terminal emulator using Xlib
- Classic green on black color scheme
- Line wrapping and scrollback buffer
- Responsive to window resize events
- Scroll through terminal history with arrow keys
- Easily extendable for ANSI escape code parsing and mouse support

---

## Demo

![Terminal Emulator Screenshot](./screenshot.png)

---

## Getting Started

### Prerequisites

- `*nix` OS with X11
- X11 development libraries (`libx11-dev` or equivalent)
- C compiler (`gcc` recommended)
- `make` (optional)

### Installation

```
git clone https://github.com/gaidardzhiev/terminax
cd terminax
make
make strip
make install
```

---

## Usage

Run the terminal emulator: `./terminax`


- Use the **Up** and **Down** arrow keys to scroll through the terminal output.
- Resize the window to automatically rewrap lines.
- Keyboard input is sent directly to the shell running inside the terminal.
- Press keys as usual to interact with the shell.

---

## Configuration

- By default, the program launches `/bin/zsh`. To use a different shell (e.g., bash), modify the `execlp` call in the source code.
- The scrollback buffer size and font dimensions can be adjusted by changing the macros in the source.

---

## Limitations

- Does not fully support ANSI escape sequences or advanced terminal features.
- No mouse support or clipboard integration yet.
- UTF-8 and wide character support are not implemented.
- Intended as a learning project and starting point for a more complete terminal emulator.

---

## Contributing

Contributions, bug reports, and feature requests are welcome!

Please open issues or submit pull requests on GitHub.

---

## License

This project is licensed under the GPL3 License. See the [LICENSE](LICENSE) file for details.

