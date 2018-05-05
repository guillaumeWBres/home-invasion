<?php

	// retrieve params from jquery 
	$prev = $_POST['previous'];
	$new = $_POST['new'];

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

	$connection->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
	$connection->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE,PDO::FETCH_OBJ);

	// modify in database
	$sql = "UPDATE events set title = \"".$new."\" where title = \"".$prev."\";";
	$query = $connection->prepare($sql);
	$query->execute();
?>
