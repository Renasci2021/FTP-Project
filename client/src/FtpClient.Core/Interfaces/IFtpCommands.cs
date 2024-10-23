using FtpClient.Core.Models;

namespace FtpClient.Core.Interfaces;

internal interface IFtpCommands
{
    FtpResponse HandleUserCommand(string username);
    FtpResponse HandlePassCommand(string password);

    FtpResponse HandlePortCommand(string host, int port);
    FtpResponse HandlePasvCommand();

    FtpResponse HandleRetrCommand(string filename);
    FtpResponse HandleStorCommand(string filename);

    FtpResponse HandleSystCommand();
    FtpResponse HandleTypeCommand(string type);

    FtpResponse HandleQuitCommand();

    FtpResponse HandlePwdCommand();
    FtpResponse HandleMkdCommand(string path);
    FtpResponse HandleRmdCommand(string path);
    FtpResponse HandleCwdCommand(string path);
    FtpResponse HandleListCommand();
}
