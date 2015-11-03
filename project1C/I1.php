<html xmlns="http://www.w3.org/1999/html">
<head>
    <meta charset='utf-8'>
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="styles.css">
    <script src="http://code.jquery.com/jquery-latest.min.js" type="text/javascript"></script>
    <script src="script.js"></script>
	
	<script src="chosen/chosen.jquery.js"></script>
	<link rel="stylesheet" href="chosen/chosen.css">
	<link rel="stylesheet" href="styles.css">

</head>
<body>
<div id='cssmenu'>
    <ul>
        <li><a href='http://localhost:1438/~cs143/S1.php'>Search Movie or Actor</a></li>
		<li class='active'><a href='http://localhost:1438/~cs143/I1.php'>Add Actor/Director </a></li>
		<li><a href='http://localhost:1438/~cs143/I2.php'>Add Movie Information</a></li>
		<li><a href='http://localhost:1438/~cs143/I3.php'>Add Comments to Movies</a></li>
        <li><a href='http://localhost:1438/~cs143/I4.php'>Add Actor to Movie </a></li>
        <li><a href='http://localhost:1438/~cs143/I5.php'>Add Director to Movie</a></li>
    </ul>
</div>
<p>

<?php
function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}
$servername = "localhost";
$username = "cs143";
//$password = "password";

// Create connection
$conn = new mysqli($servername, $username);
$conn->select_db("TEST");

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}
?>


<div style="margin-left:20px;">
<p><h3>Add an actor or a director</h3></p>
		<form method="POST" action = "<?php echo htmlspecialchars($_SERVER['PHP_SELF']);?>">
			
			
			Actor or Director?
				<select name="person_type" class="chosen-select">
					<option value="Actor" selected>Actor</option>
					<option value="Director">Director</option>
				</select>
			<br/><br/>
			
			First name: <input type="text" name="first">
			<br/><br/>
			Last name: <input type="text" name="last">
			<br/><br/>
			
			Gender:
				<input type="radio" name="sex" value="male" checked>Male
				<input type="radio" name="sex" value="female">Female
			<br/><br/>
			
			Date of birth: &nbsp;&nbsp;<input type="text" name="dob" placeholder="YYYY-MM-DD">
			<br/><br/>
			
			Date of death: &nbsp;<input type="text" name="dod" placeholder="YYYY-MM-DD">
			<br/>
			
			<br>
			<br>
			<input type="submit" value="Submit">
		</form>
		
</p>
</div>

<?php
if(isset($_POST['person_type'])) {
    $person_type = test_input($_POST['person_type']);
    $first_name = str_replace("'", "\\'",test_input($_POST['first']));
	$last_name = str_replace("'", "\\'",test_input($_POST['last']));
    $sex = test_input($_POST['sex']);
	$dob = test_input($_POST['dob']);
    $dod = test_input($_POST['dod']);
	
	//echo "$person_type---$first_name---$last_name---$sex---$dob---$dod---";
	
	$max_person_id_query = "select id from MaxPersonID";
	$result = $conn->query($max_person_id_query);

	if (!$result) {
		echo "Error in selecting from MaxPersonID";
		exit;
	}
	else {
		$row = $result->fetch_assoc();
		$max_person_id = $row["id"] + 1;
		//echo $max_person_id;
	}
    if($person_type == "Actor") {
		$insert_query = "insert into Actor(id, last, first, sex, dob, dod) values ($max_person_id, '$last_name', '$first_name', '$sex', '$dob', '$dod')";
		//echo $insert_query."<br/><br/>";
		$result = $conn->query($insert_query);
		if (!$result) {
		echo "Error in inserting, please enter valid values!";
		}
		else {
		echo "Information added for ".strtoupper($first_name." ".$last_name);
		$update_query = "update MaxPersonID set id=".$max_person_id;
		$result = $conn->query($update_query);
			if (!$result) {
				echo "Error in updating, please check!";
			}
		}
		
	}
	
	else {
		
		$insert_query = "insert into Director(id, last, first, dob, dod) values ($max_person_id, '$last_name', '$first_name', '$dob', '$dod')";
		
		$result = $conn->query($insert_query);
		if (!$result) {
		echo "Error in inserting, please enter valid values!";
		}
		else {
		echo "Information added for ".strtoupper($first_name." ".$last_name);
		$update_query = "update MaxPersonID set id=".$max_person_id;
		$result = $conn->query($update_query);
			if (!$result) {
				echo "Error in updating, please check!";
			}
		}
		
	}
	
}
$conn->close();
?>
<script>
$(function(){
    $(".chosen-select").chosen({
	disable_search_threshold: 10,
    no_results_text: "Oops, nothing found!",
	});
});
</script>
</body>
</html>