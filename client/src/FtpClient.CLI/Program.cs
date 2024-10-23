using System.Net;
using FtpClient.Core;
using FtpClient.CLI.Core;
using FtpClient.CLI.Utilities;

namespace FtpClient.CLI;

class Program
{
    static void Main(string[] args)
    {
        if (!TryParseArguments(args, out string host, out int port, out bool debug)) return;
        Debug.IsEnabled = debug;

        var executor = new FtpClientExecutor(new FtpClient.Core.FtpClient(host, port));
        executor.Execute();
    }

    // TODO: 按实际需求修改
    private static bool TryParseArguments(string[] args, out string host, out int port, out bool debug)
    {
        host = "127.0.0.1";
        port = 10021;
        debug = true;

        for (int i = 0; i < args.Length; i++)
        {
            if (args[i] == "-ip" && i + 1 < args.Length)
            {
                host = args[i++];
            }
            else if (args[i] == "-port" && i + 1 < args.Length)
            {
                if (!int.TryParse(args[i++], out port))
                {
                    Console.WriteLine("Invalid port number");
                    return false;
                }
            }
            else if (args[i] == "-debug")
            {
                debug = true;
            }
            else
            {
                Console.WriteLine("Usage: FtpClient.CLI -ip <host> -port <port> [-debug]");
                return false;
            }
        }

        return true;
    }
}
