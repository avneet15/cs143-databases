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
$conn->select_db("CS143");

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}
$query_rating = "select distinct rating as rating from Movie";
$result_rating = $conn->query($query_rating);
$query_genre = "select distinct genre as genre from MovieGenre";
$result_genre = $conn->query($query_genre);
?>


<div style="margin-left:20px;">
<p><h3>Add a new movie</h3></p>
		<form method="POST" action = "<?php echo htmlspecialchars($_SERVER['PHP_SELF']);?>">
			
			<table border =0>
			<tr><td>Title:</td><td><input type="text" name="title" required></td></tr>
			
			
			<tr><td>Company:</td><td><input type="text" name="company" required></td></tr>
			
			
			<tr><td>Year:</td><td><input type="number" name="year" value="2015" required></td></tr>
			
			
			<tr><td>MPAA Rating:</td><td>
			<select name="rating" class="chosen-select" required style="width:150px;">
			<?php
			while ($row = $result_rating->fetch_array()) {
				echo '<option value="' . $row['rating'] . '">' . $row['rating'] . '</option>';
    		}
    		?>
			</select>
		</td></tr>
		<tr><td>Genre:</td></tr>	
			<?php 

			while ($row = $result_genre->fetch_array()) {
				echo '<tr><td><input type="checkbox" name="gen[]" value="' . $row['genre'] . '">' . $row['genre'] . '</td></tr>';
    		}  
    		?>              
			<br/>
		</table>
		<br/>
			<input type="submit" value="Add Movie">
		</form>
		
</p>
</div>

<?php
if(isset($_POST['title'])) {
    $title = str_replace("'", "\\'",test_input($_POST['title']));
    $company = str_replace("'", "\\'",test_input($_POST['company']));
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
		
		$insert_query = "insert into Movie(id, title, year, rating, company) values ($max_movie_id, '$title', '$year', '$rating', '$company');";

		//echo $insert_query."<br/><br/>";
		$result = $conn->query($insert_query);
		if (!$result) {
		echo $result;
		echo "Error in inserting, please enter valid values!".mysqli_error($conn);
		}
		else {
		if(isset($_POST['gen'])) {
			$genre_arr[] = $_POST['genre'];
			foreach ($_POST['gen'] as $genre) {
				$genre_sql = "insert into MovieGenre values($max_movie_id,'$genre');";
				$result = $conn->query($genre_sql);
			}
		}
		echo "Information added for movie <b>".$title."</b>";
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
    $(".chosen-select1").chosen({
	disable_search_threshold: 10,
    no_results_text: "Oops, nothing found!",
	});
});
</script>
</body>
</html>