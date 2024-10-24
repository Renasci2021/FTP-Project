﻿using System.Net.Sockets;
using FtpClient.Core.Interfaces;
using FtpClient.Core.Models;

namespace FtpClient.Core;

public partial class FtpClient(string host, int port) : IFtpClient
{
    private readonly string _host = host;
    private readonly int _port = port;

    private TcpClient? _controlClient;
    private NetworkStream? _controlStream;
    private StreamReader? _reader;
    private StreamWriter? _writer;

    private TcpListener? _dataListener;
    private TcpClient? _dataClient;
    private NetworkStream? _dataStream;

    public event EventHandler<FtpResponse?>? ResponseReceived;
    public event EventHandler<string>? LogMessageReceived;
    public event EventHandler<Exception>? ErrorOccurred;

    public bool Connect()
    {
        _controlClient = new TcpClient(_host, _port);
        _controlStream = _controlClient.GetStream();

        _reader = new StreamReader(_controlStream);
        _writer = new StreamWriter(_controlStream) { AutoFlush = true };

        // Read the welcome message
        var response = ReadResponse();
        return response?.Code == 220;
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
            case "QUIT":
                HandleQuitCommand();
                break;

            case "LIST":
                await HandleListCommand(argument);
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
        string response = _reader!.ReadLine() ?? throw new Exception("No response received");

        // TODO: 处理多行响应
        try
        {
            int code = int.Parse(response[..3]);
            string message = response[4..];

            var ftpResponse = new FtpResponse(code, message);
            ResponseReceived?.Invoke(this, ftpResponse);
            return ftpResponse;
        }
        catch (Exception ex)
        {
            ErrorOccurred?.Invoke(this, ex);
            return null;
        }
    }
}
