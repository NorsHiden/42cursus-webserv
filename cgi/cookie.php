<?php
// Parse the HTTP cookie header
// Split the HTTP cookie header into individual cookies
$cookies = explode(';', $_SERVER['HTTP_COOKIE']);
// Loop through the cookies and set them
foreach ($cookies as $cookie) {
    $parts = explode('=', $cookie);
    $name = trim($parts[0]);
    $value = isset($parts[1]) ? trim($parts[1]) : '';
    $_COOKIE[$name] = $value;
}

if (isset($_COOKIE['testing_cookies'])) {
    echo "Status: 200 OK\r\n";
    echo "Content-Type: text/html\r\n";
    $cookieValue = is_numeric($_COOKIE['testing_cookies']) ? intval($_COOKIE['testing_cookies']) + 1 : 0;
    echo "Set-Cookie: testing_cookies=" . $cookieValue . "\r\n\r\n";
    echo "testing_cookies = " . $cookieValue;
} else {
    echo "Status: 404 Not Found\r\n";
    echo "Content-Type: text/html\r\n";
    echo "Set-Cookie: testing_cookies=0\r\n\r\n";
    echo "No cookie found.";
}

?>