<html xmlns="http://www.w3.org/1999/html">
<head>
    <meta charset='utf-8'>
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <script src="http://code.jquery.com/jquery-latest.min.js" type="text/javascript"></script>
    <script src="script.js"></script>
	
	<script src="chosen/chosen.jquery.js"></script>
	<link rel="stylesheet" href="chosen/chosen.css">
	<link rel="stylesheet" href="styles.css">
	<link rel="stylesheet" href="styles.css">



</head>
<body>
<div id='cssmenu'>
    <ul>
        <li class='active'><a href='http://localhost:1438/~cs143/S1.php'>Search Movie or Actor</a></li>
		<li><a href='http://localhost:1438/~cs143/I1.php'>Add Actor/Director </a></li>
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
$conn->select_db("CS143");

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}
?>


<div style="margin-left:20px;">
<p><h3>Select a Movie</h3></p>
		<form method="GET" action = "<?php echo htmlspecialchars($_SERVER['PHP_SELF']);?>">
			
			
			Movie: &nbsp;
			<select name="mid" class="chosen-select" required>
			<option value="" disabled selected>Select your option</option>
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
			
			<input type="submit" value="Show Movie Info">
		</form>
		
</p>
</div>

<?php
if(isset($_GET['mid'])) {
    
    $mid = test_input($_GET['mid']);
	
	//echo "--$mid--";
	
		
	$select_query = "select CONCAT(first, ' ' , last) as name, first, last from Actor A inner join MovieActor MA on A.id = MA.aid where MA.mid =".$mid;
    //$select_query = "select CONCAT(first, ' ' , last) as name, first, last from Actor where id  = 1";

	$result = $conn->query($select_query);
	if (!$result) {
	//echo $result;
	echo "Error in selecting from Movie table".mysqli_error($conn);
	}
	else {
		echo "<b>Actors in this Movie:</b><br/>";
		while ($row = mysqli_fetch_array($result)){
			echo "<a href='B1.php?actor=$row[1]%20$row[2]'>$row[0]</a><br/>";
		}
		
		$select_query = "select avg(rating) from Review where mid=$mid group by mid";
		//echo $select_query."<br/><br/>";
		$result = $conn->query($select_query);
		
		while ($row = mysqli_fetch_array($result)){
			echo "<br/>Average score of the movie based on user feedbacks: <b><i>$row[0]</i></b><br/>";
		}
		
		$select_query = "select comment,name,time,rating from Review where mid=$mid";
		//echo $select_query."<br/><br/>";
		$result = $conn->query($select_query);
		echo "<br/><b>User comments:</b><br/><ol>";
		while ($row = mysqli_fetch_array($result)){
			echo "On ".$row['time']." ".$row['name']." commented: <br/>";
			echo "<div class='dialogbox'><div class='body'><span class='tip tip-up'></span><div class='message'>".$row['comment'].
			"</div></div></div>";
			echo "<font color='red'>Rating given: ".$row['rating']."</font><br/><br/>";

		}
		
		echo "</ol><br/><a href='I3.php?mid=$mid'>Add your review now!</a><br/><br/>";
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