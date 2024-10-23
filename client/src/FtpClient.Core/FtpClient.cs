using System.Net.Sockets;
using FtpClient.Core.Interfaces;
using FtpClient.Core.Models;

namespace FtpClient.Core;

public partial class FtpClient(string host, int port) : IFtpClient
{
    private readonly string _host = host;
    private readonly int _port = port;

    private TcpClient? _controlClient;
    private NetworkStream? _controlStream;

    private TcpClient? _dataClient;
    private NetworkStream? _dataStream;

    private StreamReader? _reader;
    private StreamWriter? _writer;

    public FtpResponse Connect()
    {
        _controlClient = new TcpClient(_host, _port);
        _controlStream = _controlClient.GetStream();

        _reader = new StreamReader(_controlStream);
        _writer = new StreamWriter(_controlStream) { AutoFlush = true };

        // Read the welcome message
        return ReadResponse();
    }

    public void Disconnect()
    {
        _reader?.Close();
        _writer?.Close();
        _controlClient?.Close();
        _controlStream?.Close();
        _dataClient?.Close();
        _dataStream?.Close();
    }

    public FtpResponse HandleCommand(string command, string argument)
    {
        return command switch
        {
            "USER" => HandleBasicCommand(command, argument),
            "PASS" => HandleBasicCommand(command, argument),
            "PORT" => HandlePortCommand(argument),
            "PASV" => HandlePasvCommand(argument),
            "SYST" => HandleBasicCommand(command, argument),
            "TYPE" => HandleBasicCommand(command, argument),
            "QUIT" => HandleQuitCommand(),
            "PWD" => HandleBasicCommand(command, argument),
            "MKD" => HandleBasicCommand(command, argument),
            "RMD" => HandleBasicCommand(command, argument),
            "CWD" => HandleBasicCommand(command, argument),

            // TODO: handle other commands
            _ => new FtpResponse(0, "Unknown command", false),
        };
    }


    private void SendCommand(string command, string argument)
    {
        _writer!.Write($"{command} {argument}\r\n");
    }

    private FtpResponse ReadResponse()
    {
        string? response = _reader!.ReadLine();

        if (response == null)
        {
            return new FtpResponse(0, "No response from server", false);
        }

        int code = int.Parse(response[..3]);

        // TODO: handle multiline responses

        return new FtpResponse(code, response, true);
    }
}
