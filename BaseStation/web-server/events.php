<?php 
	
	// retrieve calendar->jquery infos
	$start = $_GET['start']; 
	$end = $_GET['end'];

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

	// liste les evenments dans la .db
	$query = "SELECT * FROM events where start > \"" .$start ."\" and end <= \"" .$end ."\""; 
//	$query = "SELECT * FROM events where end <= \"2018-05-03\"";
	echo $query;
	$result = $connection->query($query) or die(print_r($connection->errorIno()));

	echo json_encode($result->fetchAll(PDO::FETCH_ASSOC));
	exit;
?>
