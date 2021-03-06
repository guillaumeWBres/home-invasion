<?php

	// retrieve params from jquery 
	$title = $_POST['title'];
	$start = $_POST['start'];
	$end = $_POST['end'];

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

	// insert into database
	$sql = "INSERT INTO events (title, start, end) VALUES (:title, :start, :end);";
	$query = $connection->prepare($sql);
	$query->execute(
		array(
			':title'=>$title,
			':start'=>$start,
			':end'=>$end
		)
	);
?>
