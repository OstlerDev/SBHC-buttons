<?php

	// MySQL DB Config
	$servername = "localhost";
	$username   = "";
	$password   = "";
	$dbname     = "";

	// Button Variables
	// These are set using GET request. An example get request would look like this
	// ?name=photo_booth&value=3
	$sender_name = htmlspecialchars($_GET["name"]);
	$value = htmlspecialchars($_GET["value"]);

	// Create connection
	$conn = new mysqli($servername, $username, $password, $dbname);

	// Check connection
	if ($conn->connect_error) {
	    die("Connection failed: " . $conn->connect_error);
	} 

	// Create SQL to update table row to new value defined from the variables above
	$sql = "UPDATE status SET value='" . $value . "' WHERE name='" . $sender_name . "'";

	// Send the SQL to the server
	if ($conn->query($sql) === TRUE) {
		// SQL was successful
	    echo "ok";
	} else {
		// There was an error
	    echo "Error: " . $sql . "<br>" . $conn->error;
	}

	$sql = "SELECT display_name FROM status WHERE name='" . $sender_name . "'";
	$result = $conn->query($sql);

	if ($result->num_rows > 0) {
	    // output data of each row
	    while($row = $result->fetch_assoc()) {
	        echo $row["value"];
	        switch($value){
				case 0:
					slack("[Code Gray] " . $row["display_name"] . " is all normal now.", ":ghost:", "[Code Gray]");
					break;
				case 1:
					slack("[Code Green] " . $row["display_name"] . " has an issue, please respond soon. @tech", ":green_circle:", "[Code Green]");
					break;
				case 2:
					slack("[Code Yellow] " . $row["display_name"] . " has had a potentially dangerous group, please be advised. @security", ":yellow_circle:", "[Code Yellow]");
					break;
				case 3:
					slack("[Code Red] " . $row["display_name"] . " has had an emergency, please respond immediately. @channel", ":red_circle:", "[Code Red]");
					break;
				default:
					break;
			}
	    }
	}

	$conn->close();

	// (string) $message - message to be passed to Slack
	// (string) $room - room in which to write the message, too
	// (string) $icon - You can set up custom emoji icons to use with each message
	// Method from: https://gist.github.com/alexstone/9319715
	function slack($message, $icon = ":ghost:", $username = "SBHC Notifier", $room = "area_status") {
        $room = ($room) ? $room : "area_status";
        $data = "payload=" . json_encode(array(
                "channel"       =>  "#{$room}",
                "username"      =>  $username,
                "text"          =>  $message,
                "icon_emoji"    =>  $icon
            ));
	
		// You can get your webhook endpoint from your Slack settings
        $ch = curl_init("https://hooks.slack.com/services/"); // Fill this in.
        curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "POST");
        curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        $result = curl_exec($ch);
        curl_close($ch);
	
		// Laravel-specific log writing method
        // Log::info("Sent to Slack: " . $message, array('context' => 'Notifications'));
        return $result;
    }
?>