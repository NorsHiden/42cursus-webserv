<?php
    echo "Status: 200 OK\r\n\r\n";
    if ($_SERVER["REQUEST_METHOD"] == "POST")
        echo "this is Post Method yaaay :D\n";
    else
        echo "You didn't request a Post method :(\n";
?>