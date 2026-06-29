Write-Host "=========================================" -ForegroundColor Cyan
Write-Host "      Instagram Interactive Client         " -ForegroundColor Cyan
Write-Host "=========================================" -ForegroundColor Cyan

Write-Host ""
Write-Host "Commands:"
Write-Host "POST <InstaId> <post>"
Write-Host "FOLLOW <followerId> <followeeId>"
Write-Host "UNFOLLOW <followerId> <followeeId>"
Write-Host "FEED <InstaId>"
Write-Host "exit"
Write-Host ""

while ($true) {

    $cmd = Read-Host "InstaCLI> "

    if ($cmd -eq "exit") {
        Write-Host "Goodbye!"
        break
    }

    if ([string]::IsNullOrWhiteSpace($cmd)) {
        continue
    }

    try {

        $socket = New-Object System.Net.Sockets.TcpClient("127.0.0.1", 8080)

        $stream = $socket.GetStream()

        $writer = New-Object System.IO.StreamWriter($stream)
        $writer.AutoFlush = $true

        $reader = New-Object System.IO.StreamReader($stream)

        $writer.WriteLine($cmd)

        $response = $reader.ReadLine()

        Write-Host "Server: $response" -ForegroundColor Green

        $socket.Close()
    }
    catch {

        Write-Host "Could not connect to server." -ForegroundColor Red
    }
}