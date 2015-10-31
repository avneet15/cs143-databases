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
</head>
<body>
<div id='cssmenu'>
    <ul>
        <li><a href='http://localhost:1438/~cs143/S1.php'>Home</a></li>
		<li><a href='http://localhost:1438/~cs143/I1.php'>Add Actor/Director </a></li>
		<li class='active'><a href='http://localhost:1438/~cs143/I2.php'>Add Movie Information</a></li>
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
<p><h3>Add a new movie</h3></p>
		<form method="POST" action = "<?php echo htmlspecialchars($_SERVER['PHP_SELF']);?>">
			
			
			Title: &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type="text" name="title">
			<br/><br/>
			
			Company: <input type="text" name="company">
			<br/><br/>
			
			Year: &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type="text" name="year" placeholder="YYYY">
			<br/><br/>
			
			Rating:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
			<select name="rating" class="chosen-select" style="width:50px;">
				<option value="5" selected>5</option>
				<option value="4">4</option>
				<option value="3">3</option>
				<option value="2">2</option>
				<option value="1">1</option>
			</select>
			<br/><br/>
			
			                
			<!--Genre: &nbsp;&nbsp;
			<input type="checkbox" name="Action" value="Action"> Action
			<input type="checkbox" name="Adventure" value="Adventure"> Adventure
			<input type="checkbox" name="Animation" value="Animation"> Animation
			<input type="checkbox" name="Comedy" value="Comedy"> Comedy
			<input type="checkbox" name="Crime" value="Crime"> Crime
			<input type="checkbox" name="Documentary" value="Documentary"> Documentary
			<input type="checkbox" name="Drama" value="Drama"> Drama
			<input type="checkbox" name="Family" value="Family"> Family
			<input type="checkbox" name="Fantasy" value="Fantasy"> Fantasy
			<br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
			<input type="checkbox" name="Musical" value="Musical"> Musical
			<input type="checkbox" name="Mystery" value="Mystery"> Mystery
			<input type="checkbox" name="Romance" value="Romance"> Romance
			<input type="checkbox" name="Sci-Fi" value="Sci-Fi"> Sci-Fi
			<input type="checkbox" name="Short" value="Short"> Short
			<input type="checkbox" name="Thriller" value="Thriller"> Thriller
			<input type="checkbox" name="War" value="War"> War
			<input type="checkbox" name="Western" value="Western"> Western
			-->
			
			<br/><br/>
			
			
			<input type="submit" value="Submit">
		</form>
		
</p>
</div>

<?php
if(isset($_POST['title'])) {
    $title = test_input($_POST['title']);
    $company = test_input($_POST['company']);
	$year = test_input($_POST['year']);
	$rating = test_input($_POST['rating']);
    
	//echo "$title---$company---$year---$rating--";
	
	$max_movie_id_query = "select id from MaxMovieID";
	$result = $conn->query($max_movie_id_query);

	if (!$result) {
		echo "Error in selecting from MaxPersonID";
		exit;
	}
	else {
		$row = $result->fetch_assoc();
		$max_movie_id = $row["id"] + 1;
		//echo $max_movie_id;
	}
		
		$insert_query = "insert into Movie(id, title, year, rating, company) values ($max_movie_id, '$title', '$year', $rating, '$company');";
		//echo $insert_query."<br/><br/>";
		$result = $conn->query($insert_query);
		if (!$result) {
		echo $result;
		echo "Error in inserting, please enter valid values!".mysqli_error($conn);
		}
		else {
		echo "New Movie information added.";
		$update_query = "update MaxMovieID set id=".$max_movie_id;
		$result = $conn->query($update_query);
			if (!$result) {
				echo "Error in updating, please check!";
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