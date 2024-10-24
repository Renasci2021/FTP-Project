using FtpClient.CLI.Utilities;
using FtpClient.Core.Interfaces;

namespace FtpClient.CLI.Core;

internal class FtpClientExecutor
{
    private readonly IFtpClient _ftpClient;

    public FtpClientExecutor(IFtpClient ftpClient)
    {
        _ftpClient = ftpClient;

        _ftpClient.ResponseReceived += (_, response) => FtpConsolePrinter.PrintResponse(response);
        _ftpClient.DataReceived += (_, data) => FtpConsolePrinter.PrintData(data);

        _ftpClient.LogMessageReceived += (_, message) => FtpConsolePrinter.PrintLogMessage(message);
        _ftpClient.ErrorOccurred += (_, exception) => FtpConsolePrinter.PrintErrorMessage(exception);
    }

    public void Execute()
    {
        if (!_ftpClient.Connect()) return;

        while (true)
        {
            ReadCommand(out var command, out var argument);
            if (string.IsNullOrWhiteSpace(command)) continue;

            _ftpClient.HandleCommand(command, argument).Wait();

            if (command == "QUIT") break;
        }
    }

    private static void ReadCommand(out string command, out string argument)
    {
        Console.Write("ftp> ");
        var input = Console.ReadLine();
        if (string.IsNullOrWhiteSpace(input))
        {
            command = string.Empty;
            argument = string.Empty;
            return;
        }

        var parts = input.Split(' ', 2);
        command = parts[0].ToUpper();
        argument = parts.Length > 1 ? parts[1] : string.Empty;
    }
}
