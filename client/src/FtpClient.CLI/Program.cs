using FtpClient.CLI.Core;

namespace FtpClient.CLI;

class Program
{
    static void Main(string[] args)
    {
        if (!TryParseArguments(args, out string host, out int port)) return;

        var executor = new FtpClientExecutor(new FtpClient.Core.FtpClient(host, port));
        executor.Execute();
    }

    private static bool TryParseArguments(string[] args, out string host, out int port)
    {
        host = "127.0.0.1";
        port = 10021;

        for (int i = 0; i < args.Length; i++)
        {
            if (args[i] == "-ip" && i + 1 < args.Length)
            {
                host = args[++i];
            }
            else if (args[i] == "-port" && i + 1 < args.Length)
            {
                if (!int.TryParse(args[++i], out port))
                {
                    Console.WriteLine("Invalid port number");
                    return false;
                }
            }
            else
            {
                Console.WriteLine("Usage: FtpClient.CLI -ip <host> -port <port>");
                return false;
            }
        }

        return true;
    }
}
