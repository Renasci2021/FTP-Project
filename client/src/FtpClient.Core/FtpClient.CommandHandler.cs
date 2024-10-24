using System.Net.Sockets;
using System.Text.RegularExpressions;
using FtpClient.Core.Models;

namespace FtpClient.Core;

public partial class FtpClient
{
    private void HandleBasicCommand(string command, string argument)
    {
        SendCommand(command, argument);
        ReadResponse();
    }

    private void HandlePortCommand(string argument)
    {
        var parts = argument.Split(',');

        if (parts.Length != 6)
        {
            ErrorOccurred?.Invoke(this, new ArgumentException("Invalid PORT argument"));
            return;
        }

        if (_dataConnectionMode != DataConnectionMode.None)
        {
            CloseDataConnection();
            LogMessageReceived?.Invoke(this, "Previous data connection closed");
        }

        try
        {
            var ip = string.Join('.', parts[0..4]);
            var port = int.Parse(parts[4]) * 256 + int.Parse(parts[5]);

            ListenDataConnection(ip, port);
            _dataConnectionMode = DataConnectionMode.Active;
            LogMessageReceived?.Invoke(this, $"Data connection listening at {ip}:{port}");

            SendCommand("PORT", argument);
            ReadResponse();
        }
        catch (ArgumentException ex)
        {
            ErrorOccurred?.Invoke(this, new Exception("Failed to parse PORT argument", ex));
        }
        catch (SocketException ex)
        {
            ErrorOccurred?.Invoke(this, new Exception("Failed to open data connection", ex));
        }
    }

    private void HandlePasvCommand(string argument)
    {
        if (_dataConnectionMode != DataConnectionMode.None)
        {
            CloseDataConnection();
            LogMessageReceived?.Invoke(this, "Previous data connection closed");
        }

        SendCommand("PASV", argument);
        var response = ReadResponse();

        if (response?.Code != 227)
        {
            return;
        }

        try
        {
            var match = PasvRegex().Match(response.Messages[0]);
            var ip = string.Join('.', match.Groups.Cast<Group>().Skip(1).Take(4).Select(g => g.Value));
            var port = int.Parse(match.Groups[5].Value) * 256 + int.Parse(match.Groups[6].Value);

            OpenDataConnection(ip, port);
            _dataConnectionMode = DataConnectionMode.Passive;
            LogMessageReceived?.Invoke(this, $"Data connection opened at {ip}:{port}");
        }
        catch (ArgumentException ex)
        {
            ErrorOccurred?.Invoke(this, new Exception("Failed to parse PASV response", ex));
        }
        catch (SocketException ex)
        {
            ErrorOccurred?.Invoke(this, new Exception("Failed to open data connection", ex));
        }
    }

    private async Task HandleRetrCommand(string argument)
    {
        if (!EstablishDataConnection()) return;

        SendCommand("RETR", argument);
        var response = ReadResponse();

        if (response?.Code != 125 && response?.Code != 150) return;

        LogMessageReceived?.Invoke(this, "Downloading file...");

        using var memoryStream = new MemoryStream();
        byte[] buffer = new byte[8192];
        int bytesRead;

        while ((bytesRead = await _dataStream!.ReadAsync(buffer)) > 0)
        {
            memoryStream.Write(buffer, 0, bytesRead);
        }

        var endResponse = await WaitForTransferEnd();

        if (endResponse.Code == 226)
        {
            File.WriteAllBytes(argument, memoryStream.ToArray());
            LogMessageReceived?.Invoke(this, "File downloaded");
        }

        CloseDataConnection();
    }

    private async Task HandleStorCommand(string argument)
    {
        if (!EstablishDataConnection()) return;

        if (!File.Exists(argument))
        {
            ErrorOccurred?.Invoke(this, new Exception("File not found"));
            return;
        }

        SendCommand("STOR", argument);
        var response = ReadResponse();

        if (response?.Code != 125 && response?.Code != 150) return;

        byte[] bytes = File.ReadAllBytes(argument);

        LogMessageReceived?.Invoke(this, "Uploading file...");
        await _dataStream!.WriteAsync(bytes);
        await _dataStream.FlushAsync();
        _dataStream.Close();
        LogMessageReceived?.Invoke(this, "File uploaded");

        await WaitForTransferEnd();

        CloseDataConnection();
    }

    private async Task HandleListCommand(string argument)
    {
        if (!EstablishDataConnection()) return;

        SendCommand("LIST", argument);
        var response = ReadResponse();

        if (response!.Code != 125 && response.Code != 150) return;

        var reader = new StreamReader(_dataStream!);
        var task = reader.ReadToEndAsync();

        var endResponse = await WaitForTransferEnd();

        if (endResponse.Code == 226)
        {
            DataReceived?.Invoke(this, task.Result);
        }

        CloseDataConnection();
    }

    private void HandleQuitCommand()
    {
        SendCommand("QUIT", "");
        ReadResponse();
        Disconnect();
    }

    private bool EstablishDataConnection()
    {
        if (_dataConnectionMode == DataConnectionMode.Active)
        {
            if (_dataListener!.Pending())
            {
                _dataClient = _dataListener.AcceptTcpClient();
                _dataStream = _dataClient.GetStream();
                LogMessageReceived?.Invoke(this, "Data connection established");
                return true;
            }
            else
            {
                ErrorOccurred?.Invoke(this, new Exception("Data connection not established"));
                _dataConnectionMode = DataConnectionMode.None;
                return false;
            }
        }
        else if (_dataConnectionMode == DataConnectionMode.Passive)
        {
            return true;
        }
        else
        {
            ErrorOccurred?.Invoke(this, new Exception("Data connection mode not set"));
            return false;
        }
    }

    private void CloseDataConnection()
    {
        _dataStream?.Close();
        _dataClient?.Close();
        _dataListener?.Stop();

        _dataConnectionMode = DataConnectionMode.None;
    }

    private async Task<FtpResponse> WaitForTransferEnd()
    {
        FtpResponse endResponse;

        while (true)
        {
            endResponse = ReadResponse()!;

            if (endResponse.Code == 226) break;
            if (endResponse.Code >= 400) break;

            await Task.Delay(100);
        }

        return endResponse;
    }

    private void ListenDataConnection(string ip, int port)
    {
        _dataListener?.Stop();
        _dataListener = new TcpListener(System.Net.IPAddress.Parse(ip), port);
        _dataListener.Start();
    }

    private void OpenDataConnection(string ip, int port)
    {
        _dataClient = new TcpClient(ip, port);
        _dataStream = _dataClient.GetStream();
    }

    [GeneratedRegex(@"(\d+),(\d+),(\d+),(\d+),(\d+),(\d+)")]
    private static partial Regex PasvRegex();
}
