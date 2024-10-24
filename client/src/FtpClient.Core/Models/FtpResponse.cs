namespace FtpClient.Core.Models;

public record FtpResponse(int Code, List<string> Messages);
