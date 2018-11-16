<?php
 
// Create connection
$con=mysqli_connect("settleapp-109458.mysql.binero.se","**user**","**pw**","**db_name**");
 
// Check connection
if (mysqli_connect_errno())
{
  echo "Unable to find database: " . mysqli_connect_error();
}
 
// This SQL statement selects ALL from the table 'Locations'
$sql = "SELECT * FROM Locations";
 
// Check if there are results
if ($result = mysqli_query($con, $sql))
{
	// If so, then create a results array and a temporary one
	// to hold the data
	$resultArray = array();
	$tempArray = array();
 
	// Loop through each row in the result set
	while($row = $result->fetch_object())
	{
		// Add each row into our results array
		$tempArray = $row;
	    array_push($resultArray, $tempArray);
	}
 
	// Finally, encode the array to JSON and output the results
	echo json_encode($resultArray);
}
 
// Close connections
mysqli_close($con);
?>