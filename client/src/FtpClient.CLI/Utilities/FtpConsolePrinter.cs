using FtpClient.Core.Models;

namespace FtpClient.CLI.Utilities;

internal static class FtpConsolePrinter
{
    public static void PrintResponse(FtpResponse? response)
    {
        if (response == null) return;

        Console.ForegroundColor = response.Code switch
        {
            >= 200 and < 300 => ConsoleColor.Green,
            >= 300 and < 400 => ConsoleColor.Cyan,
            >= 400 => ConsoleColor.Red,
            _ => ConsoleColor.White
        };
        Console.Write(response.Code + " ");
        Console.ResetColor();
        Console.WriteLine(response.Message);

        if (response.Data != null)
        {
            Console.WriteLine(response.Data);
        }
    }

    public static void PrintLogMessage(string message)
    {
        Console.Write("ftp: ");
        Console.WriteLine(message);
    }

    public static void PrintErrorMessage(Exception exception)
    {
        Console.ForegroundColor = ConsoleColor.Red;
        Console.Write("ftp: ");
        Console.ResetColor();
        Console.WriteLine(exception.Message);
    }
}
