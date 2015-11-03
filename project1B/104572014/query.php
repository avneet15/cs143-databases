<html>
<head/>
<body>
	<p>
		<form method ="GET" action = "<?php echo $_SERVER['PHP_SELF'];?>">
	<h3>Please type a valid SQL statement below:</h3><br/>
	<textarea name ="query" cols="100" rows="10"/><?php echo $_GET['query'] ?></textarea><br/><br/>
	<input type="submit" value = "Submit">

</form>
</p>
<?php

if(isset($_GET['query'])) {

	$servername = "localhost";
	$username = "cs143";
	//$password = "password";

// Create connection
$conn = new mysqli($servername, $username);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} else {
	$conn->select_db("CS143");
	$sql = $_GET['query'];
	$result = $conn->query($sql);

	if ($result->num_rows > 0) {
		echo "<h2>Results from MySQL are:</h2>";
		echo "<table border = 1><tr>";
		$field_info = $result->fetch_fields();
		foreach ($field_info as $val) {
            echo "<th>".$val->name."</th>";
		}
		echo "</tr>";
		while($row = $result->fetch_array()) {
			echo "<tr>";
			for ($x = 0; $x < count($field_info); $x++) {
				if($row[$x] == "")echo "<td>N/A</td>";
				else echo "<td>".$row[$x]."</td>";
			}
			echo "</tr>";
		}
	}
	else {
		echo mysqli_error($conn)."<br>";
		echo "No Result found!";
	}
//echo "Connected successfully<br/>";

//echo "<br>".$sql;
}
}

?>
</body>
</html>
