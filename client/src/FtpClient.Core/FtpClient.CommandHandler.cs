using System.Net.Sockets;
using System.Text.RegularExpressions;
using FtpClient.Core.Models;

namespace FtpClient.Core;

public partial class FtpClient
{
    private FtpResponse HandleBasicCommand(string command, string argument)
    {
        SendCommand(command, argument);
        return ReadResponse();
    }

    // TODO: 测试两种数据连接方式
    private FtpResponse HandlePortCommand(string argument)
    {
        var parts = argument.Split(',');
        if (parts.Length != 6)
        {
            return new FtpResponse(0, "Invalid PORT command", false);
        }

        var ip = string.Join('.', parts[0..4]);
        var port = int.Parse(parts[4]) * 256 + int.Parse(parts[5]);

        SendCommand("PORT", argument);

        try
        {
            OpenDataConnection(ip, port);
            return ReadResponse();
        }
        catch (Exception ex)
        {
            return new FtpResponse(0, ex.Message, false);
        }
    }

    private FtpResponse HandlePasvCommand(string argument)
    {
        SendCommand("PASV", argument);
        var response = ReadResponse();

        if (!response.IsSuccess)
        {
            return response;
        }

        if (response.Code != 227)
        {
            return response;
        }

        try
        {
            var match = PasvRegex().Match(response.Message);
            var ip = string.Join('.', match.Groups.Cast<Group>().Skip(1).Take(4).Select(g => g.Value));
            var port = int.Parse(match.Groups[5].Value) * 256 + int.Parse(match.Groups[6].Value);
            OpenDataConnection(ip, port);
            return response;
        }
        catch (Exception ex)
        {
            return new FtpResponse(0, ex.Message, false);
        }
    }

    private FtpResponse HandleQuitCommand()
    {
        SendCommand("QUIT", "");
        var response = ReadResponse();
        Disconnect();
        return response;
    }

    private void OpenDataConnection(string ip, int port)
    {
        _dataClient?.Close();
        _dataStream?.Close();

        _dataClient = new TcpClient(ip, port);
        _dataStream = _dataClient.GetStream();
    }

    [GeneratedRegex(@"(\d+),(\d+),(\d+),(\d+),(\d+),(\d+)")]
    private static partial Regex PasvRegex();
}
