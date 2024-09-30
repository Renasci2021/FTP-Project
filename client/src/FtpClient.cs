using System.Net.Sockets;
using System.Text;

namespace FtpClient;

public class FtpClient
{
    private TcpClient? _client;
    private NetworkStream? _stream;

    public void Connect(string host, int port)
    {
        try
        {
            _client = new TcpClient(host, port);
            _stream = _client.GetStream();
            Console.WriteLine("Connected to {0}:{1}", host, port);

            // Read welcome message
            ReadResponse();
        }
        catch
        {
            Console.WriteLine("Failed to connect to {0}:{1}", host, port);
        }
    }

    public void Disconnect()
    {
        _stream?.Close();
        _client?.Close();
        Console.WriteLine("Disconnected from server");
    }

    public void ProcessCommand(string command)
    {
        if (string.IsNullOrEmpty(command))
        {
            return;
        }

        SendCommand(command);
        ReadResponse();
    }

    private void ReadResponse()
    {
        if (_stream == null)
        {
            return;
        }

        byte[] buffer = new byte[1024];
        int bytesRead = _stream.Read(buffer, 0, buffer.Length);
        string response = Encoding.ASCII.GetString(buffer, 0, bytesRead);
        Console.WriteLine(response);
    }

    private void SendCommand(string command)
    {
        if (_stream == null)
        {
            return;
        }

        byte[] buffer = Encoding.ASCII.GetBytes(command + "\r\n");
        _stream.Write(buffer, 0, buffer.Length);
    }
}