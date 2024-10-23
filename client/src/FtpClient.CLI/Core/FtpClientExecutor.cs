using FtpClient.CLI.Utilities;
using FtpClient.Core.Interfaces;
using FtpClient.Core.Models;

namespace FtpClient.CLI.Core;

internal class FtpClientExecutor(IFtpClient ftpClient, Logger logger)
{
    private readonly IFtpClient _ftpClient = ftpClient;
    private readonly Logger _logger = logger;

    public void Execute()
    {
        if (!TryEstablishConnection()) return;

        while (true)
        {
            ReadCommand(out var command, out var argument);
            if (string.IsNullOrWhiteSpace(command)) continue;

            // TODO: 参数校验
            var response = _ftpClient.HandleCommand(command, argument);
            HandleResponse(response);

            if (command == "QUIT")
            {
                _logger.LogInfo("Exiting FTP client");
                break;
            }
        }
    }

    private bool TryEstablishConnection()
    {
        _logger.LogInfo("Starting FTP client");

        var response = _ftpClient.Connect();
        if (!response.IsSuccess)
        {
            _logger.LogError(response.Message);
            return false;
        }

        Console.WriteLine(response.Message);
        return true;
    }

    private void Dispose()
    {
        _logger.LogInfo("Disconnecting FTP client");

        var response = _ftpClient.Disconnect();
        if (!response.IsSuccess)
        {
            _logger.LogError(response.Message);
        }

        Console.WriteLine(response.Message);
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

    private void HandleResponse(FtpResponse response)
    {
        if (response.IsSuccess)
        {
            Console.WriteLine(response.Message);

            if (response.Data != null)
            {
                Console.WriteLine(response.Data);
            }
        }
        else
        {
            _logger.LogError(response.Message);
        }
    }
}
