using FtpClient.Core.Interfaces;
using FtpClient.Core.Models;

namespace FtpClient.Core;

public partial class AnonymousClient : IFtpCommands
{
    public FtpResponse HandleCwdCommand(string path)
    {
        throw new NotImplementedException();
    }

    public FtpResponse HandleListCommand()
    {
        throw new NotImplementedException();
    }

    public FtpResponse HandleMkdCommand(string path)
    {
        throw new NotImplementedException();
    }

    public FtpResponse HandlePassCommand(string password)
    {
        throw new NotImplementedException();
    }

    public FtpResponse HandlePasvCommand()
    {
        throw new NotImplementedException();
    }

    public FtpResponse HandlePortCommand(string host, int port)
    {
        throw new NotImplementedException();
    }

    public FtpResponse HandlePwdCommand()
    {
        throw new NotImplementedException();
    }

    public FtpResponse HandleQuitCommand()
    {
        throw new NotImplementedException();
    }

    public FtpResponse HandleRetrCommand(string filename)
    {
        throw new NotImplementedException();
    }

    public FtpResponse HandleRmdCommand(string path)
    {
        throw new NotImplementedException();
    }

    public FtpResponse HandleStorCommand(string filename)
    {
        throw new NotImplementedException();
    }

    public FtpResponse HandleSystCommand()
    {
        throw new NotImplementedException();
    }

    public FtpResponse HandleTypeCommand(string type)
    {
        throw new NotImplementedException();
    }

    public FtpResponse HandleUserCommand(string username)
    {
        throw new NotImplementedException();
    }
}
