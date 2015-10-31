<html xmlns="http://www.w3.org/1999/html">
<head>
    <meta charset='utf-8'>
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="styles.css">
    <script src="http://code.jquery.com/jquery-latest.min.js" type="text/javascript"></script>
    <script src="script.js"></script>
</head>
<body>
<div id='cssmenu'>
    <ul>
        <li ><a href='http://localhost:1438/~cs143/S1.php'>Home</a></li>
        <li ><a href='http://localhost:1438/~cs143/I1.php'>Add Actor/Director </a></li>
        <li><a href='http://localhost:1438/~cs143/I2.php'>Add Movie Information</a></li>
        <li><a href='http://localhost:1438/~cs143/I3.php'>Add Comments to Movies</a></li>
        <li ><a href='http://localhost:1438/~cs143/I4.php'>Add Actor to Movie </a></li>
        <li class='active'><a href='http://localhost:1438/~cs143/I5.php'>Add Director to Movie</a></li>
    </ul>
</div>
<p>
<form method ="GET" action = "<?php echo htmlspecialchars($_SERVER['PHP_SELF']);?>">
    <h3>Add Director for Movie:</h3><br>
    <b>Director: </b>
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
        $query_director = "select concat(first,' ',last) as name from Director order by first";
        $result_director = $conn->query($query_director);
        echo '<select name="Director">';
        while ($row = $result_director->fetch_array()) {
            //echo $row['name'];
            echo '<option value="' . $row['name'] . '">' . $row['name'] . '</option>';
        }
        echo '</select></br>';
        echo "<br/><b>Movie: </b>";
        $query_movie = "select title from Movie order by title";
        $result_movie = $conn->query($query_movie);
        echo '<select name="Movie">';
        while ($row = $result_movie->fetch_array()) {
            //echo $row['name'];
            echo '<option value="' . $row['title'] . '">' . $row['title'] . '</option>';
        }
        echo '</select></br></br>';
    }
    ?>
    <input type="submit" name = 'submit' value = "Add">
</form>
</p>
<?php
if(isset($_GET['submit'])) {
    $director = test_input($_GET['Director']);
    $movie = test_input($_GET['Movie']);
    $query_did = "select id from Director where concat(first,' ',last)='" . $director . "'";
    $query_mid = "select id from Movie where title='" . $movie . "'";
    $result_did = $conn->query($query_did);
    $result_mid = $conn->query($query_mid);
    if ($result_did->num_rows > 0 and $result_mid->num_rows > 0) {
        $did_row = $result_did->fetch_assoc();
        $did = $did_row['id'];
        $mid_row = $result_mid->fetch_assoc();
        $mid = $mid_row['id'];
        $insert_sql = "insert into MovieDirector values(" . $mid . "," . $did.")";
        if ($conn->query($insert_sql) === TRUE) {
            echo "<b>New entry created successfully for ".$director." for ".$movie."</b>";
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

</body>
</html>