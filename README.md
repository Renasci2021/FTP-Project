# FTP Implementation

A complete FTP server and client implementation project, including:

- FTP server implemented in C
- FTP client implemented in C# (.NET 8.0)

## Features

- Standard FTP protocol support
- Active and passive mode data transfer
- Basic file and directory operations
- Cross-platform support

## Project Structure

- `server/` - FTP Server
  - Implemented in C
  - Multi-client concurrent connections
  - See [server/README.md](server/README.md) for details

- `client/` - FTP Client
  - Implemented in C# (.NET 8.0)
  - Command-line interface
  - See [client/README.md](client/README.md) for details

## Quick Start

1. Build and start the server:
```bash
cd server/src
make
./server -port 10021 -root /path/to/root
```

2. Build and run the client:
```bash
cd client/src
dotnet run -- -ip 127.0.0.1 -port 10021
```

## License

MIT License - see [LICENSE](LICENSE)
