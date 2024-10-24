using FtpClient.Core.Models;

namespace FtpClient.CLI.Utilities;

internal static class FtpConsolePrinter
{
    public static void PrintResponse(FtpResponse? response)
    {
        if (response == null) return;

        var color = response.Code switch
        {
            >= 200 and < 300 => ConsoleColor.Green,
            >= 300 and < 400 => ConsoleColor.Cyan,
            >= 400 => ConsoleColor.Red,
            _ => ConsoleColor.White
        };

        foreach (var message in response.Messages.SkipLast(1))
        {
            Console.ForegroundColor = color;
            Console.Write(response.Code + "-");
            Console.ResetColor();
            Console.WriteLine(message);
        }

        Console.ForegroundColor = color;
        Console.Write(response.Code + " ");
        Console.ResetColor();
        Console.WriteLine(response.Messages.Last());
    }

    public static void PrintData(string data)
    {
        Console.ForegroundColor = ConsoleColor.Yellow;
        Console.WriteLine(data);
        Console.ResetColor();
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
