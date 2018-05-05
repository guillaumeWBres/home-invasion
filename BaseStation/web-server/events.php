<?php 
	
	// retrieve calendar->jquery infos
	$start = $_POST['vStart']; 
	$end= $_POST['vEnd']; 

	// database description
	$host = "localhost";
	$username = "root";
	$password = "root";
	$database = "test";

	try {
		$connection = new PDO("mysql:host=$host;dbname=$database", $username, $password);
	} catch(Exception $e){
		exit('Failed to connect to database, error: ' .$e->getMessage());
	}

	$connection->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
	$connection->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

	// retrieve elements for this week
	$query = "SELECT * FROM events where start >= \"" .$start . "\" and end <= \"" .$end ."\";";
	$result = $connection->query($query) or die(print_r($connection->errorIno()));

	echo json_encode($result->fetchAll(PDO::FETCH_ASSOC));
	exit;
?>
