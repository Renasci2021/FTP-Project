namespace FtpClient.CLI.Utilities;

internal static class Debug
{
    public static bool IsEnabled { get; set; } = true;

    public static void LogInfo(string message)
    {
        if (!IsEnabled) return;

        Console.WriteLine(message);
    }

    public static void LogError(string message)
    {
        if (!IsEnabled) return;

        Console.ForegroundColor = ConsoleColor.Red;
        Console.Write("Error: ");
        Console.ResetColor();
        Console.WriteLine(message);
    }
}
