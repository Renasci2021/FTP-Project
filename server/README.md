# FTP Server

A simple FTP server implemented in C that supports basic FTP commands and anonymous login.

## Features

- Anonymous user login support
- Active mode (PORT) and passive mode (PASV) data transfer
- Basic FTP commands support:
  - USER/PASS - Authentication
  - PORT/PASV - Data connection
  - RETR/STOR - File transfer
  - PWD/CWD - Directory operations
  - MKD/RMD - Directory creation and deletion
  - LIST - Directory listing
  - SYST/TYPE - System information
  - QUIT/ABOR - Connection control

## Build & Run

### Build

```bash
cd src
make
```

### Run

```bash
./server [-port <port>] [-root <root>] [-debug]
```

Options:

- `-port <port>`: Server listening port, default to 21
- `-root <root>`: FTP root directory, default to `/tmp`
- `-debug`: Enable debug output
- `-help`: Show help information

## Project Structure

- `src/` - Source code directory
  - `main.c` - Program entry
  - `server.c` - Server core functionality
  - `commands.c` - FTP command handlers
  - `session.c` - Session management
  - `utils.c` - Utility functions
  - `arg_parser.c` - Argument parser
  - `include/` - Header files

## Development Notes

- Multi-threaded client connection handling
- Supports up to 100 concurrent clients
- Basic security checks and error handling implemented
