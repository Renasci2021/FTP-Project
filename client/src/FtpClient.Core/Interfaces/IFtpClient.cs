using FtpClient.Core.Models;

namespace FtpClient.Core.Interfaces;

public interface IFtpClient
{
    FtpResponse Connect();
    FtpResponse Disconnect();

    FtpResponse HandleCommand(string command, string argument);
}
