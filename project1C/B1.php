<html>
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
        <li class='active'><a href='http://localhost:1438/~cs143/S1.php'>Search Movie or Actor</a></li>
        <li ><a href='http://localhost:1438/~cs143/I1.php'>Add Actor/Director </a></li>
        <li><a href='http://localhost:1438/~cs143/I2.php'>Add Movie Information</a></li>
        <li><a href='http://localhost:1438/~cs143/I3.php'>Add Comments to Movies</a></li>
        <li><a href='http://localhost:1438/~cs143/I4.php'>Add Actor to Movie </a></li>
        <li><a href='http://localhost:1438/~cs143/I5.php'>Add Director to Movie</a></li>
    </ul>
</div>
	<p>
		<form method ="GET" action = "<?php echo htmlspecialchars($_SERVER['PHP_SELF']);?>">

</form>
</p>
<?php
function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}

if(isset($_GET['actor'])) {

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
	$actor = test_input($_GET['actor']);
    echo "<h3 style='text-transform: uppercase';>$actor</h3>";
	$parts = explode(" ",$actor);
    $sql_info= "select id,sex as SEX,dob as Dob,IFNULL(dod,'Still Alive') as Dod from Actor where first='" . $parts[0] . "' AND last ='" . $parts[1] . "'";
    $aid = 0;
}

	$result_info = $conn->query($sql_info);
    if ($result_info->num_rows > 0) {
        echo "<table>";

        $field_info = $result_info->fetch_fields();
        while ($row = $result_info->fetch_array()) {
            $aid = $row['id'];
            for ($x = 1; $x < count($field_info); $x++) {
                echo "<tr><td>" . $field_info[$x]->name . " : " . $row[$x] . "</td></tr>";
            }
        }
        echo "</table>";
        $sql_role = "select title,role,mid from Movie M inner join MovieActor MA where MA.aid =".$aid." and MA.mid = M.id";
        $result_aux = $conn->query($sql_role);
        if ($result_aux->num_rows > 0) {
            echo "<h3><i>Filmography</i></h3>";
            echo "<table border = 1><tr><th>ROLE</th><th>MOVIE</th></tr>";
            while ($row = $result_aux->fetch_array()) {
                echo "<tr><td>" .$row['role']."</td><td><a href='B2.php?mid=".$row['mid']."'>".$row['title']."</a></td></tr>"; 
            }
        }

    } else {
		//echo mysqli_error($conn)."<br>";
		echo "No Result found!";
	}
//echo "Connected successfully<br/>";

echo "<br>".$sql;

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
