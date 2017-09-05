<?php
if($driverStandings || $constructorStandings) error(400, "Bad Request: Circuit queries do not support standings qualifiers.");

$query = "SELECT SQL_CALC_FOUND_ROWS DISTINCT ci.circuitRef, ci.name, ci.location, ci.country, ci.lat, ci.lng, ci.url FROM circuits ci";
if($year || $driver || $constructor || $status || $grid || $fastest || $results) $query .= ", races ra";
if($driver || $constructor || $status || $grid || $fastest || $results) $query .= ", results re";
if($driver) $query .= ", drivers dr";
if($constructor) $query .= ", constructors co";
$query .= " WHERE TRUE";
if($year || $driver || $constructor || $status || $grid || $fastest || $results) $query .= " AND ra.circuitId=ci.circuitId";
if($circuit) $query .= " AND ci.circuitRef='$circuit'";
if($driver || $constructor || $status || $grid || $fastest || $result) $query .= " AND re.raceId=ra.raceId";
if($constructor) $query .= " AND re.constructorId=co.constructorId AND co.constructorRef='$constructor'";
if($driver) $query .= " AND re.driverId=dr.driverId AND dr.driverRef='$driver'";
if($status) $query .= " AND re.statusId='$status'";
if($grid) $query .= " AND re.grid='$grid'";
if($fastest) $query .= " AND re.rank='$fastest'";
if($results) $query .= " AND re.positionText='$results'";
if($year) $query .= " AND ra.year='$year'";
if($round) $query .= " AND ra.round='$round'";
$query .= " ORDER BY ci.circuitRef LIMIT $offset, $limit";
//echo "$query<br>\n<br>\n";

$result = mysql_query($query) or die('I cannot select from the database because: ' . mysql_error());
if($result) {
  $query = "SELECT FOUND_ROWS()";
  $found_rows_result = mysql_query($query) or die('I cannot select from the database because: ' . mysql_error());
  $row = mysql_fetch_row($found_rows_result);
  $total = $row[0];
  if(strcmp($format, "xml") == 0) {
    doXMLHeader($url, $series, $limit, $offset, $total);
    echo "\t<CircuitTable";
    if(isset($year)) echo " season=\"$year\"";
    if(isset($round)) echo " round=\"$round\"";
    if(isset($circuit)) echo " circuitId=\"$circuit\"";
    if(isset($constructor)) echo " constructorId=\"$constructor\"";
    if(isset($driver)) echo " driverId=\"$driver\"";
    if(isset($grid)) echo " grid=\"$grid\"";
    if(isset($fastest)) echo " fastest=\"$fastest\"";
    if(isset($results)) echo " position=\"$results\"";
    if(isset($status)) echo " statusId=\"$status\"";
    echo ">\n";
    while($row = @mysql_fetch_array($result)) {
      $circuitId = $row[0];
      $circuitName = $row[1];
      $locality = $row[2];
      $country= $row[3];
      $lat = $row[4];
      $long = $row[5];
      $url = $row[6];
      echo "\t\t<Circuit circuitId=\"$circuitId\" url=\"$url\">\n";
      echo "\t\t\t<CircuitName>$circuitName</CircuitName>\n";
      echo "\t\t\t<Location lat=\"$lat\" long=\"$long\">\n";
      echo "\t\t\t\t<Locality>$locality</Locality>\n";
      echo "\t\t\t\t<Country>$country</Country>\n";
      echo "\t\t\t</Location>\n";
      echo "\t\t</Circuit>\n";
    }
    echo "\t</CircuitTable>\n";
    echo "</MRData>\n";
  } elseif(strcmp($format, "json") == 0) {
    if($callback) {
      doJSONPHeader($url, $series, $limit, $offset, $total, $callback);
    } else {
      doJSONHeader($url, $series, $limit, $offset, $total);
    }
    echo "\"CircuitTable\":{";
    if(isset($year)) echo "\"season\":\"$year\",";
    if(isset($round)) echo "\"round\":\"$round\",";
    if(isset($circuit)) echo "\"circuitId\":\"$circuit\",";
    if(isset($constructor)) echo "\"constructorId\":\"$constructor\",";
    if(isset($driver)) echo "\"driverId\":\"$driver\",";
    if(isset($grid)) echo "\"grid\":\"$grid\",";
    if(isset($fastest)) echo "\"fastest\":\"$fastest\",";
    if(isset($results)) echo "\"position\":\"$results\",";
    if(isset($status)) echo "\"status\":\"$status\",";
    
    $numRows = mysql_num_rows($result);
    echo "\"Circuits\":[";
    if($numRows > 0) {
      for($i=1; $i<=$numRows; $i++) {
        $row = @mysql_fetch_array($result);
        $circuitId = $row[0];
        $circuitName = $row[1];
        $locality = $row[2];
        $country = $row[3];
        $lat = $row[4];
        $long = $row[5];
        $url = escape($row[6]);
        echo "{";
        echo "\"circuitId\":\"$circuitId\",";
        echo "\"url\":\"$url\",";
        echo "\"circuitName\":\"$circuitName\",";
        echo "\"Location\":{";
        echo "\"lat\":\"$lat\",";
        echo "\"long\":\"$long\",";
        echo "\"locality\":\"$locality\",";
        echo "\"country\":\"$country\"";
        echo "}";
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
  error(404, "Circuit Not Found");
}

?>