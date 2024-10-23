using System.Net.Sockets;
using FtpClient.Core.Interfaces;
using FtpClient.Core.Models;

namespace FtpClient.Core;

public partial class AnonymousClient(string host, int port) : IFtpClient
{
    private readonly string _host = host;
    private readonly int _port = port;

    private TcpClient? _controlClient;
    private NetworkStream? _controlStream;

    private StreamReader? _reader;
    private StreamWriter? _writer;

    public FtpResponse Connect()
    {
        _controlClient = new TcpClient(_host, _port);
        _controlStream = _controlClient.GetStream();

        _reader = new StreamReader(_controlStream);
        _writer = new StreamWriter(_controlStream) { AutoFlush = true };

        // Read the welcome message
        (int code, string message) = ReadResponse();

        return new FtpResponse(code, message, code == 220);
    }

    public FtpResponse Disconnect()
    {
        _reader?.Close();
        _writer?.Close();
        _controlStream?.Close();
        _controlClient?.Close();

        return new FtpResponse(0, "Disconnected", true);
    }

    public FtpResponse HandleCommand(string command, string argument)
    {
        return command switch
        {
            "NULL" => new FtpResponse(0, "Null", true),
            "DATA" => new FtpResponse(0, "Data", true, "fds\nfdas"),
            _ => new FtpResponse(0, "Unknown command", false),
        };
    }

    private (int code, string message) ReadResponse()
    {
        string response = _reader!.ReadLine()
            ?? throw new IOException("Failed to read response");

        int code = int.Parse(response[..3]);

        // TODO: handle multiline responses

        return (code, response);
    }
}
