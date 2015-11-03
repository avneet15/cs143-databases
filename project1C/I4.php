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
        <li><a href='http://localhost:1438/~cs143/S1.php'>Search Movie or Actor</a></li>
        <li ><a href='http://localhost:1438/~cs143/I1.php'>Add Actor/Director </a></li>
        <li><a href='http://localhost:1438/~cs143/I2.php'>Add Movie Information</a></li>
        <li><a href='http://localhost:1438/~cs143/I3.php'>Add Comments to Movies</a></li>
        <li class='active'><a href='http://localhost:1438/~cs143/I4.php'>Add Actor to Movie </a></li>
        <li><a href='http://localhost:1438/~cs143/I5.php'>Add Director to Movie</a></li>
    </ul>
</div>
<p>
<form method ="GET" action = "<?php echo htmlspecialchars($_SERVER['PHP_SELF']);?>">
    <h3>Add Actor to Movie:</h3><br>
    <b>Actor: </b>
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

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} else {
    $conn->select_db("TEST");
    $query_actor = "select concat(first,' ',last) as name from Actor order by first";
    $result_actor = $conn->query($query_actor);
    echo '<select name="Actor" class="chosen-select">';
    while ($row = $result_actor->fetch_array()) {
        //echo $row['name'];
        echo '<option value="' . $row['name'] . '">' . $row['name'] . '</option>';
    }
    echo '</select></br>';
    echo "<br/><b>Movie: </b>";
    $query_movie = "select title from Movie order by title";
    $result_movie = $conn->query($query_movie);
    echo '<select name="Movie" class="chosen-select">';
    while ($row = $result_movie->fetch_array()) {
        //echo $row['name'];
        echo '<option value="' . $row['title'] . '">' . $row['title'] . '</option>';
    }
    echo '</select></br></br>';
    $actor = test_input($_GET['Actor']);
    //$query_aid = "select id from Actor where concat(first,' ',last)='".$actor."'";
    //$result_aid = $conn->query($query_aid);
    //echo $result_aid->num_rows;
}
?>
    <b>Role: </b><input type = "text" name ="role" style="font-size:10pt;width: 150px;height: 20px;" placeholder = "As Role"><br/><br/>
    <input type="submit" name = 'submit' value = "Add">
</form>
</p>
<?php
if(isset($_GET['submit'])) {
    $actor = test_input($_GET['Actor']);
    $movie = test_input($_GET['Movie']);
    $query_aid = "select id from Actor where concat(first,' ',last)='" . $actor . "'";
    $query_mid = "select id from Movie where title='" . $movie . "'";
    $result_aid = $conn->query($query_aid);
    $result_mid = $conn->query($query_mid);
    $role = test_input($_GET['role']);
    if ($result_aid->num_rows > 0 and $result_mid->num_rows > 0) {
        $aid_row = $result_aid->fetch_assoc();
        $aid = $aid_row['id'];
        $mid_row = $result_mid->fetch_assoc();
        $mid = $mid_row['id'];
        $insert_sql = "insert into MovieActor values(" . $mid . "," . $aid . ",'" . $role . "')";
        if ($conn->query($insert_sql) === TRUE) {
            echo "<b>New record created successfully for ".$actor." in ".$movie."</b>";
        } else {
            echo "<b>Record creation failed</b>";
            //echo "Error: " . $sql . "<br>" . $conn->error;
        }
    } else {
        echo "<b>Please select valid values</b>";
    }
}
$conn->close();
?>
<script>
    $(".chosen-select").chosen({
	disable_search_threshold: 10,
    no_results_text: "Oops, nothing found!"
	});
</script>
</body>
</html>


