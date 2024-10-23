using FtpClient.CLI.Utilities;
using FtpClient.Core.Interfaces;
using FtpClient.Core.Models;

namespace FtpClient.CLI.Core;

internal class FtpClientExecutor(IFtpClient ftpClient)
{
    private readonly IFtpClient _ftpClient = ftpClient;

    public void Execute()
    {
        if (!TryEstablishConnection()) return;

        while (true)
        {
            ReadCommand(out var command, out var argument);
            if (string.IsNullOrWhiteSpace(command)) continue;

            // TODO: 参数校验
            var response = _ftpClient.HandleCommand(command, argument);
            PrintResponse(response);

            if (command == "QUIT")
            {
                Debug.LogInfo("Exiting FTP client");
                break;
            }
        }
    }

    private bool TryEstablishConnection()
    {
        Debug.LogInfo("Starting FTP client");

        var response = _ftpClient.Connect();
        if (!response.IsSuccess)
        {
            Debug.LogError(response.Message);
            return false;
        }

        Console.WriteLine(response.Message);
        return true;
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

    private static void PrintResponse(FtpResponse response)
    {
        if (response.IsSuccess)
        {
            if (response.Code != 0)
            {
                Console.ForegroundColor = ConsoleColor.Green;
                Console.Write(response.Code + " ");
                Console.ResetColor();
            }

            Console.WriteLine(response.Message);

            if (response.Data != null)
            {
                Console.ForegroundColor = ConsoleColor.Cyan;
                Console.WriteLine(response.Data);
                Console.ResetColor();
            }
        }
        else
        {
            Debug.LogError(response.Message);
        }
    }
}
