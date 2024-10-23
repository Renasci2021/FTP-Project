namespace FtpClient.Core.Models;

public record FtpResponse(int Code, string Message, bool IsSuccess, string? Data = null);
