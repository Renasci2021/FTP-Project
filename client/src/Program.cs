using System;

namespace FtpClient;

class Program
{
    static void Main(string[] args)
    {
        Console.WriteLine("Welcome to the FTP client!");

        FtpClient client = new FtpClient();
        client.Connect("127.0.0.1", 21);

        while (true)
        {
            Console.Write("ftp> ");
            string command = Console.ReadLine();
            Commands.Execute(command, client);
        }

        client.Disconnect();
    }
}