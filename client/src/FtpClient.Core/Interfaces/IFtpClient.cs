using FtpClient.Core.Models;

namespace FtpClient.Core.Interfaces;

public interface IFtpClient
{
    bool Connect();
    void Disconnect();

    Task HandleCommand(string command, string argument);

    event EventHandler<FtpResponse?> ResponseReceived;
    event EventHandler<string> LogMessageReceived;
    event EventHandler<Exception> ErrorOccurred;
}
