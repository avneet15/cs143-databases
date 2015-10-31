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
        <li class='active'><a href='http://localhost:1438/~cs143/S1.php'>Home</a></li>
		<li><a href='http://localhost:1438/~cs143/I1.php'>Add Actor/Director </a></li>
		<li><a href='http://localhost:1438/~cs143/I2.php'>Add Movie Information</a></li>
		<li><a href='http://localhost:1438/~cs143/I3.php'>Add Comments to Movies</a></li>
        <li><a href='http://localhost:1438/~cs143/I4.php'>Add Actor to Movie </a></li>
        <li><a href='http://localhost:1438/~cs143/I5.php'>Add Director to Movie</a></li>
    </ul>
</div>

<p>


<form method ="GET" action = "<?php echo htmlspecialchars($_SERVER['PHP_SELF']);?>">
	<h3>Search for an actor or movie:</h3><br/>
	<input type = "text" name ="query" style="font-size:10pt;width: 200px;height: 22px;" placeholder = "Actor or Movie Name"><br/><br/>
	<input type="submit" value = "Search">

</form>
</p>
<?php
function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}

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
	$conn->select_db("TEST");
	$keyword = test_input($_GET['query']);
	$parts = explode(" ",$keyword);
    $num_of_params=sizeof($parts);
    if($num_of_params == 1) {
        $sql1 = "select concat(first,' ',last) as Actors from Actor where first like '%" . $parts[0] . "%' OR last LIKE '%" . $parts[0] . "%' order by first";
    }
    else if($num_of_params){
        $sql1 = "select concat(first,' ',last) as Actors from Actor where (first like '%" . $parts[0] . "%' AND last LIKE '" . $parts[1] . "%' or first like '%" . $parts[1] . "%' AND last LIKE '" . $parts[0] . "%') order by first";
    }
    $sql2 = "select concat(title,' ',year) as MOVIES from Movie where title like '%".$keyword."%' order by title";
	$result_actor = $conn->query($sql1);
    $result_movie = $conn->query($sql2);
    /*echo $sql1." ".$sql2;*/
    if ($result_actor->num_rows > 0 or $result_movie->num_rows > 0) {
        if ($result_actor->num_rows > 0) {
            echo "<table><tr>";
            $field_info = $result_actor->fetch_fields();
            foreach ($field_info as $val) {
                echo "<th>" . $val->name . "</th>";
            }
            echo "</tr>";
            while ($row = $result_actor->fetch_array()) {
                echo "<tr>";
                //echo "<td>" . $row['ACTOR'] .$row['DOB']. "</td>";
                echo '<td><a href="B1.php?actor='. urlencode($row['Actors']).'">'.$row['Actors'].'</a></td>';
                echo "</tr>";
            }
        }
        if ($result_movie->num_rows > 0) {
            echo "<table><tr>";
            $field_info = $result_movie->fetch_fields();
            foreach ($field_info as $val) {
                echo "<th>" . $val->name . "</th>";
            }
            echo "</tr>";
            while ($row = $result_movie->fetch_array()) {
                echo "<tr>";
                for ($x = 0; $x < count($field_info); $x++) {
                    echo '<td><a href="/~cs143/B1.php?actor=Pierce+Brosnan">'.$row[$x].'</a></td>';
                    /*echo "<a><td>" . $row[$x] . "</td>";*/
                }
                echo "</tr>";
            }
        }
    } else {
		//echo mysqli_error($conn)."<br>";
		echo "No Result found!";
	}
//echo "Connected successfully<br/>";

//echo "<br>".$sql;
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
</script>
</body>
</html>
