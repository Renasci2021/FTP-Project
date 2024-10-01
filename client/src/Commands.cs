using System.Net;
using System.Net.Sockets;
using System.Reflection.Metadata;
using System.Text;

namespace FtpClient;

public class Commands
{
    public static void Execute(string command, FtpClient client)
    {
        string[] parts = command.Split(' ');
        string cmd = parts[0].ToUpper();

        switch (cmd)
        {
            case "PORT":
                HandlePort(client);
                break;

            default:
                client.ProcessCommand($"{cmd} {string.Join(" ", parts[1..])}");
                break;
        }
    }

    private static void HandlePort(FtpClient client)
    {
        string localIp = "127,0,0,1";
        int port = 12345;

        int port1 = port / 256;
        int port2 = port % 256;
        string portCommand = $"PORT {localIp},{port1},{port2}";

        client.ProcessCommand(portCommand);

        ListenForDataConnection(port);
    }

    private static void ListenForDataConnection(int port)
    {
        Task.Run(() =>
        {
            TcpListener listener = new TcpListener(IPAddress.Any, port);
            listener.Start();

            Console.WriteLine("Listening for data connection on port {0}", port);

            TcpClient? dataClient = listener.AcceptTcpClient();

            Console.WriteLine("Data connection established");

            NetworkStream? dataStream = dataClient.GetStream();

            byte[] buffer = new byte[1024];
            int bytesRead = dataStream.Read(buffer, 0, buffer.Length);
            string response = Encoding.ASCII.GetString(buffer, 0, bytesRead);
            Console.WriteLine(response);

            dataStream.Close();
            dataClient.Close();
            listener.Stop();
        });
    }
}