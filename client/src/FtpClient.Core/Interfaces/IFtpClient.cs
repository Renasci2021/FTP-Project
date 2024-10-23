using FtpClient.Core.Models;

namespace FtpClient.Core.Interfaces;

public interface IFtpClient
{
    FtpResponse Connect();
    void Disconnect();

    FtpResponse HandleCommand(string command, string argument);
}
