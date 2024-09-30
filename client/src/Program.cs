using System;

namespace FtpClient;

class Program
{
    static void Main(string[] args)
    {
        Console.WriteLine("Welcome to the FTP client!");

        var client = new FtpClient();
        client.Connect("127.0.0.1", 21);

        while (true)
        {
            Console.Write("ftp> ");
            string command = Console.ReadLine() ?? string.Empty;
            Commands.Execute(command, client);
        }
    }
}