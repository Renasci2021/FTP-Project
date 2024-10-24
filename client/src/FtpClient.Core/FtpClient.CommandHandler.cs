using System.Net.Sockets;
using System.Text.RegularExpressions;

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

        try
        {
            var ip = string.Join('.', parts[0..4]);
            var port = int.Parse(parts[4]) * 256 + int.Parse(parts[5]);

            ListenDataConnection(ip, port);

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
        SendCommand("PASV", argument);
        var response = ReadResponse();

        if (response?.Code != 227)
        {
            return;
        }

        try
        {
            var match = PasvRegex().Match(response.Message);
            var ip = string.Join('.', match.Groups.Cast<Group>().Skip(1).Take(4).Select(g => g.Value));
            var port = int.Parse(match.Groups[5].Value) * 256 + int.Parse(match.Groups[6].Value);
            OpenDataConnection(ip, port);
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

    private async Task HandleListCommand(string argument)
    {
        await Task.Delay(1000);
        ErrorOccurred?.Invoke(this, new NotImplementedException());
    }

    private void HandleQuitCommand()
    {
        SendCommand("QUIT", "");
        ReadResponse();
        Disconnect();
    }

    private void ListenDataConnection(string ip, int port)
    {
        _dataListener?.Stop();
        _dataListener = new TcpListener(System.Net.IPAddress.Parse(ip), port);
        _dataListener.Start();
    }

    private void OpenDataConnection(string ip, int port)
    {
        _dataClient?.Close();
        _dataStream?.Close();

        _dataClient = new TcpClient(ip, port);
        _dataStream = _dataClient.GetStream();

        LogMessageReceived?.Invoke(this, $"Data connection opened to {ip}:{port}");
    }

    [GeneratedRegex(@"(\d+),(\d+),(\d+),(\d+),(\d+),(\d+)")]
    private static partial Regex PasvRegex();
}
