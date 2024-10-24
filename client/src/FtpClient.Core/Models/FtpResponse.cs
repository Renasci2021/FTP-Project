namespace FtpClient.Core.Models;

public record FtpResponse(int Code, string Message, string? Data = null);
