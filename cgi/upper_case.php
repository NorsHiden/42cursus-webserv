<?php
    $input = fopen('php://stdin', 'r');
    echo "Status: 200 OK\r\n";
    echo "Content-Type: text/plain\r\n\r\n";
    while ($line = fgets($input)) {
        echo strtoupper($line);
    }
?>