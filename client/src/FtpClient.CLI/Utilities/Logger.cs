namespace FtpClient.CLI.Utilities;

internal class Logger(TextWriter? logWriter = null, TextWriter? errorWriter = null)
{
    private readonly TextWriter? _logWriter = logWriter;
    private readonly TextWriter? _errorWriter = errorWriter;

    public void LogInfo(string message)
    {
        _logWriter?.WriteLine(message);
    }

    public void LogError(string message)
    {
        _errorWriter?.WriteLine(message);
    }
}
