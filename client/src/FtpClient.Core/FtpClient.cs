using System.Net.Sockets;
using System.Reflection.Metadata.Ecma335;
using FtpClient.Core.Interfaces;
using FtpClient.Core.Models;

namespace FtpClient.Core;

public partial class FtpClient(string host, int port) : IFtpClient
{
    private enum DataConnectionMode
    {
        None,
        Active,
        Passive
    }

    private readonly string _host = host;
    private readonly int _port = port;

    private TcpClient? _controlClient;
    private NetworkStream? _controlStream;
    private StreamReader? _reader;
    private StreamWriter? _writer;

    private TcpListener? _dataListener;
    private TcpClient? _dataClient;
    private NetworkStream? _dataStream;

    private DataConnectionMode _dataConnectionMode = DataConnectionMode.None;

    public event EventHandler<FtpResponse?>? ResponseReceived;
    public event EventHandler<string>? DataReceived;

    public event EventHandler<string>? LogMessageReceived;
    public event EventHandler<Exception>? ErrorOccurred;

    public bool Connect()
    {
        try
        {
            _controlClient = new TcpClient(_host, _port);
            _controlStream = _controlClient.GetStream();

            _reader = new StreamReader(_controlStream);
            _writer = new StreamWriter(_controlStream) { AutoFlush = true };

            // Read welcome messages
            ReadResponse();
            return true;
        }
        catch (Exception ex)
        {
            ErrorOccurred?.Invoke(this, ex);
            return false;
        }
    }

    public void Disconnect()
    {
        _reader?.Close();
        _writer?.Close();
        _controlClient?.Close();
        _controlStream?.Close();
        _dataClient?.Close();
        _dataStream?.Close();
        _dataListener?.Stop();
    }

    public async Task HandleCommand(string command, string argument)
    {
        switch (command)
        {
            case "USER":
            case "PASS":
            case "SYST":
            case "TYPE":
            case "PWD":
            case "MKD":
            case "RMD":
            case "CWD":
                HandleBasicCommand(command, argument);
                break;

            case "PORT":
                HandlePortCommand(argument);
                break;
            case "PASV":
                HandlePasvCommand(argument);
                break;

            case "RETR":
                await HandleRetrCommand(argument);
                break;
            case "STOR":
                await HandleStorCommand(argument);
                break;
            case "LIST":
                await HandleListCommand(argument);
                break;

            case "QUIT":
                HandleQuitCommand();
                break;

            default:
                ErrorOccurred?.Invoke(this, new Exception($"Unknown command: {command}"));
                break;
        }
    }


    private void SendCommand(string command, string argument)
    {
        _writer!.Write($"{command} {argument}\r\n");
    }

    private FtpResponse? ReadResponse()
    {
        string? response = _reader!.ReadLine();
        if (response == null) return null;

        int code = int.Parse(response[..3]);
        List<string> messages = [];

        while (true)
        {
            messages.Add(response);

            if (int.TryParse(response[..3], out _) && response[3] == ' ') break;

            response = _reader.ReadLine()!;
        }

        var ftpResponse = new FtpResponse(code, messages);
        ResponseReceived?.Invoke(this, ftpResponse);
        return ftpResponse;
    }
}
