<?php
if(($driverStandings || $constructorStandings) && ($circuits || $grid || $results || $status)) error(400, "Bad Request: Cannot combine standings with circuit, grid, result or status qualifiers.");

$query = "SELECT SQL_CALC_FOUND_ROWS DISTINCT constructors.constructorRef, constructors.name, constructors.nationality, constructors.url FROM constructors";

if($year || $driver || $status || $grid || $results || $circuit || $fastest) $query .= ", results";
if($year || $circuit || $driverStandings || $constructorStandings) $query .= ", races";
if($driverStandings || ($constructorStandings && $driver)) $query .= ", driverStandings"; 
if($constructorStandings) $query .= ", constructorStandings"; 
if($circuit) $query .= ", circuits";
if($driver) $query .= ", drivers";
$query .= " WHERE TRUE";
if($year || $driver || $status || $grid || $results || $circuit || $fastest) $query .= " AND constructors.constructorId=results.constructorId";
if($year || $circuit) $query .= " AND results.raceId=races.raceId";
if($circuit) $query .= " AND races.circuitId=circuits.circuitId AND circuits.circuitRef='$circuit'";
if($driver) $query .= " AND results.driverId=drivers.driverId AND drivers.driverRef='$driver'";
if($status) $query .= " AND results.statusId='$status'";
if($grid) $query .= " AND results.grid='$grid'";
if($fastest) $query .= " AND results.rank='$fastest'";
if($results) $query .= " AND results.positionText='$results'";
if($constructor) $query .= " AND constructors.constructorRef='$constructor'";

if($driverStandings) $query .= " AND driverStandings.positionText='$driverStandings' AND driverStandings.constructorId=constructors.constructorId";
if($driverStandings || ($constructorStandings && $driver)) $query .= " AND driverStandings.raceId=races.raceId";
if(($driverStandings || $constructorStandings) && $driver) $query .= " AND drivers.driverId=driverStandings.driverId";

if($constructorStandings) $query .= " AND constructorStandings.positionText='$constructorStandings' AND constructorStandings.constructorId=constructors.constructorId AND constructorStandings.raceId=races.raceId";
if($constructorStandings && $driver) $query .= " AND driverStandings.constructorId=constructorStandings.constructorId";

if($year) $query .= " AND races.year='$year'";
if($round) {
  $query .= " AND races.round='$round'";
} else {
  if($driverStandings || $constructorStandings) {
    if($year) {
      $query .= " AND races.round=(SELECT MAX(round) FROM races WHERE races.year='$year')";
    } else {
      $query .= " AND (races.year, races.round) IN (SELECT year, MAX(round) FROM races GROUP BY year)";
    }
  }
}
$query .= " ORDER BY constructors.name LIMIT $offset, $limit";
//echo "$query<br>\n<br>\n";

$result = mysql_query($query) or die('I cannot select from the database because: ' . mysql_error());
if($result) {
  $query = "SELECT FOUND_ROWS()";
  $found_rows_result = mysql_query($query) or die('I cannot select from the database because: ' . mysql_error());
  $row = mysql_fetch_row($found_rows_result);
  $total = $row[0];
  if(strcmp($format, "xml") == 0) {
    doXMLHeader($url, $series, $limit, $offset, $total);
    echo "\t<ConstructorTable";
    if(isset($year)) echo " season=\"$year\"";
    if(isset($round)) echo " round=\"$round\"";
    if(isset($circuit)) echo " circuitId=\"$circuit\"";
    if(isset($constructor)) echo " constructorId=\"$constructor\"";
    if(isset($constructorStandings)) echo " constructorStandings=\"$constructorStandings\"";
    if(isset($driver)) echo " driverId=\"$driver\"";
    if(isset($driverStandings)) echo " driverStandings=\"$driverStandings\"";
    if(isset($grid)) echo " grid=\"$grid\"";
    if(isset($fastest)) echo " fastest=\"$fastest\"";
    if(isset($results)) echo " position=\"$results\"";
    if(isset($status)) echo " statusId=\"$status\"";
    echo ">\n";
    while($row = @mysql_fetch_array($result)) {
      $constructorId = $row[0];
      $name = $row[1];
      $nationality = $row[2];
      $url = $row[3];
      echo "\t\t<Constructor constructorId=\"$constructorId\" url=\"$url\">\n";
      echo "\t\t\t<Name>$name</Name>\n";
      echo "\t\t\t<Nationality>$nationality</Nationality>\n";
      echo "\t\t</Constructor>\n";
    }
    echo "\t</ConstructorTable>\n";
    echo "</MRData>\n";
  } elseif(strcmp($format, "json") == 0) {
    if($callback) {
      doJSONPHeader($url, $series, $limit, $offset, $total, $callback);
    } else {
      doJSONHeader($url, $series, $limit, $offset, $total);
    }
    echo "\"ConstructorTable\":{";
    if(isset($year)) echo "\"season\":\"$year\",";
    if(isset($round)) echo "\"round\":\"$round\",";
    if(isset($circuit)) echo "\"circuitId\":\"$circuit\",";
    if(isset($constructor)) echo "\"constructorId\":\"$constructor\",";
    if(isset($constructorStandings)) echo "\"constructorStandings\":\"$constructorStandings\",";
    if(isset($driver)) echo "\"driverId\":\"$driver\",";
    if(isset($driverStandings)) echo "\"driverStandings\":\"$driverStandings\",";
    if(isset($grid)) echo "\"grid\":\"$grid\",";
    if(isset($fastest)) echo "\"fastest\":\"$fastest\",";
    if(isset($results)) echo "\"position\":\"$results\",";
    if(isset($status)) echo "\"status\":\"$status\",";
    
    $numRows = mysql_num_rows($result);
    echo "\"Constructors\":[";
    if($numRows > 0) {
      for($i=1; $i<=$numRows; $i++) {
        $row = @mysql_fetch_array($result);
        $constructorRef = $row[0];
        $name = $row[1];
        $nationality = $row[2];
        $url = escape($row[3]);
        echo "{";
        echo "\"constructorId\":\"$constructorRef\",";
        echo "\"url\":\"$url\",";
        echo "\"name\":\"$name\",";
        echo "\"nationality\":\"$nationality\"";
        if($i == $numRows) {
          echo "}";
        } else {
          echo "},";
        }
      }
    }
    echo "]}}}";
    if($callback) {
      echo ")";
    }
  }
} else {
  error(404, "Constructor Not Found");
}

?>