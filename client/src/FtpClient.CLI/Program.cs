namespace FtpClient.CLI;

using System;

class Program
{
    static void Main(string[] args)
    {
        var massage = FtpClient.Core.Class1.GetMessage();
        Console.WriteLine(massage);
    }
}
