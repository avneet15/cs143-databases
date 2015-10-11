<html>
<head/>
<body>
	<h1>Calculator</h1>
<form method="GET" action="<?php echo $_SERVER['PHP_SELF'];?>">
Enter expression: <input type="text" name="exp">
<input type="submit" value = "Calculate">
</form>
<?php

if(isset($_GET['exp'])) {

	$expr = $_GET['exp'];

	echo "<h2>Result</h2>";
	//Check for valid expression
	$pattern = '(^\s*([-+]?)(\d*)(\.\d)*(\d)*(?:\s*([-+*\/])\s*((?:\s[-+])?[+-]?(\d*)(\.\d)*(\d)*)\s*)*+$)';
	if (preg_match($pattern, $expr)) {
		$new_exp = "echo ".$expr.";";
		
		eval("\$v = ".$expr.";");
		
		if(!isset($v)) {
			echo "Invalid Expression";
		} else {

			if($v==null && ($v!==0 && $v!==0.0) ) { //for handling divide by zero
				echo "Invalid Expression";
			}

			else {
				echo $expr." = ".$v;
			}
			
		}
	} else {
		echo "Invalid Expression";
	}
} else {
	
}
?>
</body>
</html>