namespace FtpClient;

public class Commands
{
    public static void Execute(string command, FtpClient client)
    {
        string[] parts = command.Split(' ');
        string cmd = parts[0].ToUpper();
        command = $"{cmd} {string.Join(" ", parts[1..])}";
        client.ProcessCommand(command);
    }
}