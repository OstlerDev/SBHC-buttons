<?php
// Set to plain text
header("Content-Type: text/plain");

// MySQL DB Config
$servername = "localhost";
$username   = "";
$password   = "";
$dbname     = "";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

// Create SQL to update table row to new value defined from the variables above
$sql = "SELECT * FROM status";
$result = $conn->query($sql);
$rows = array();

// Converting to JSON
if ($result->num_rows > 0) {
    // output data of each row
    while($row = $result->fetch_assoc()) {
        $rows[] = $row;
    }
} else {
    //echo "0 results";
}

// Print the JSON
print json_encode($rows);
$conn->close();
?>