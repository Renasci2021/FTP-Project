namespace FtpClient;

public class Commands
{
    public static void Execute(string command, FtpClient client)
    {
        string[] parts = command.Split(' ');
        string cmd = parts[0].ToUpper();

        switch (cmd)
        {
            case "USER":
                client.ProcessCommand(command);
                break;
            case "PASS":
                client.ProcessCommand(command);
                break;
            case "QUIT":
                client.ProcessCommand(command);
                // client.Disconnect();
                // Environment.Exit(0);
                break;
            default:
                Console.WriteLine("Invalid command");
                break;
        }
    }
}