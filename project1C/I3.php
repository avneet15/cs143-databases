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
		<li><a href='http://localhost:1438/~cs143/I1.php'>Add Actor/Director </a></li>
		<li><a href='http://localhost:1438/~cs143/I2.php'>Add Movie Information</a></li>
		<li class='active'><a href='http://localhost:1438/~cs143/I3.php'>Add Comments to Movies</a></li>
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
$conn->select_db("CS143");

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}
?>


<div style="margin-left:20px;">
<p><h3>Add a new comment to a movie</h3></p>
		<form method="POST" action = "<?php echo htmlspecialchars($_SERVER['PHP_SELF']);?>">
			
			
			Name: &nbsp;&nbsp;<input type="text" name="name" required>
			<br/><br/>
			
			Movie: &nbsp;
			<select name="mid" class="chosen-select" required>
			<option value="">Select your option</option>
			<?php 
			$select_query = "select id, title from Movie";
			$result = $conn->query($select_query);
			if(isset($_GET['mid'])) $mid = $_GET['mid'];
			while ($row = mysqli_fetch_array($result)){
				if($row[0]==$mid) {
				echo "<option value=$row[0] selected>$row[1]</option>";
				}
				else {
				echo "<option value=$row[0]>$row[1]</option>";
				}
			}
			?>
			</select>
			<br/><br/>
			
			Rating:&nbsp;
			<select name="rating" class="chosen-select" style="width:50px;">
				<option value="5" selected>5</option>
				<option value="4">4</option>
				<option value="3">3</option>
				<option value="2">2</option>
				<option value="1">1</option>
			</select>
			<br/><br/>
			
			Comment:
			<br/><textarea rows="4" cols="50" name="comment" placeholder="max. 500 characters"></textarea>
			<br/><br/>
			<input type="submit" value="Submit">
		</form>
		
</p>
</div>

<?php
if(isset($_POST['name'])) {
    $name = str_replace("'", "\\'",test_input($_POST['name']));
    $mid = test_input($_POST['mid']);
	$comment = str_replace("'", "\\'",test_input($_POST['comment']));
	$rating = test_input($_POST['rating']);
    
	//echo "$name---$mid---$comment---$rating--";
	
		
	$insert_query = "insert into Review(mid, name, rating, comment) values ($mid, '$name', $rating, '$comment');";
	//echo $insert_query."<br/><br/>";
	$result = $conn->query($insert_query);
	if (!$result) {
	echo $result;
	echo "Error in inserting, please enter valid values!".mysqli_error($conn);
	}
	else {
	echo "New Review added.";
	}
	
}
$conn->close();
?>
<script>
    $(".chosen-select1").chosen({
	disable_search_threshold: 10,
    no_results_text: "Oops, nothing found!"
	});
</script>
</body>
</html>