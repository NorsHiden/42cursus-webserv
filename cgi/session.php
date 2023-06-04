<?php
// Split the HTTP cookie header into individual cookies
$cookies = explode(';', $_SERVER['HTTP_COOKIE']);
$PATH_INFO = "/Users/nelidris/Desktop/webserv/cgi/session_data";
// Loop through the cookies and set them
foreach ($cookies as $cookie) {
    $parts = explode('=', $cookie);
    $name = trim($parts[0]);
    $value = isset($parts[1]) ? trim($parts[1]) : '';
    $_COOKIE[$name] = $value;
}
// Start the session
session_start();

// Check if the session ID exists in the request
if (isset($_COOKIE['PHPSESSID'])) {
    // Retrieve the session ID from the client's request
    $sessionId = $_COOKIE['PHPSESSID'];

    // Load the session data from the file
	if (!file_exists("{$PATH_INFO}/$sessionId"))
		fopen("{$PATH_INFO}/$sessionId", 'w');
   	$sessionData = file_get_contents("{$PATH_INFO}/$sessionId");

    // Unserialize the session data into an associative array
    $sessionVars = unserialize($sessionData);
} else {
    // Generate a new session ID
    $sessionId = uniqid();

    // Create an empty array to store session variables
    $sessionVars = array();
}

// Access and manipulate session variables
$sessionVars['number'] = isset($sessionVars['number']) ? $sessionVars['number'] + 1 : 1;

// Serialize the session variables
$sessionData = serialize($sessionVars);

// Save the session data to the file
file_put_contents("{$PATH_INFO}/$sessionId", $sessionData);


// Output the session data
echo "Status: 200 OK\r\n";
echo "Content-Type: text/html\r\n";
echo "Set-Cookie: PHPSESSID=" . $sessionId . "\r\n\r\n";
echo "Session ID: $sessionId<br>";
echo "Number: {$sessionVars['number']}<br>";
?>