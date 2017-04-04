<?php
if($driverStandings || $constructorStandings) error(400, "Bad Request: Race queries do not support standings qualifiers.");

$query = "SELECT SQL_CALC_FOUND_ROWS ra.year, ra.round, ra.name, ra.date, ra.time, ra.url, c.circuitRef, c.name, c.location, c.country, c.lat, c.lng, c.alt, c.url FROM races ra, circuits c";

if($driver || $constructor || $grid || $results || $status || $fastest) $query .= ", results re";
if($driver) $query .= ", drivers";
if($constructor) $query .= ", constructors";
$query .= " WHERE ra.circuitId=c.circuitId";
if($year) $query .= " AND ra.year='$year'";
if($round) $query .= " AND ra.round='$round'";
if($circuit) $query .= " AND c.circuitRef='$circuit'";
if($driver || $constructor || $grid || $results || $status || $fastest) $query .= " AND ra.raceId=re.raceId";
if($constructor) $query .= " AND re.constructorId=constructors.constructorId AND constructors.constructorRef='$constructor'";
if($driver) $query .= " AND re.driverId=drivers.driverId AND drivers.driverRef='$driver'";
if($status) $query .= " AND re.statusId='$status'";
if($grid) $query .= " AND re.grid='$grid'";
if($fastest) $query .= " AND re.rank='$fastest'";
if($results) $query .= " AND re.positionText='$results'";
$query .= " ORDER BY ra.year, ra.round LIMIT $offset, $limit";

//echo "$query<br>\n<br>\n";
$result = mysql_query($query) or die('I cannot select from the database because: ' . mysql_error());
if($result) {
  $query = "SELECT FOUND_ROWS()";
  $found_rows_result = mysql_query($query) or die('I cannot select from the database because: ' . mysql_error());
  $row = mysql_fetch_row($found_rows_result);
  $total = $row[0];
  if(strcmp($format, "xml") == 0) {
    doXMLHeader($url, $series, $limit, $offset, $total);
    echo "\t<RaceTable";
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
      $season = $row[0];
      $round = $row[1];
      $raceName = $row[2];
      $date = $row[3];
      $start = $row[4];
      $raceUrl = $row[5];
      $circuitId = $row[6];
      $circuitName = $row[7];
      $locality = $row[8];
      $country= $row[9];
      $lat = $row[10];
      $long = $row[11];
      $alt = $row[12];
      $circuitUrl = $row[13];
      echo "\t\t<Race season=\"$season\" round=\"$round\"";
      if($raceUrl) echo " url=\"$raceUrl\"";
      echo ">\n";
      echo "\t\t\t<RaceName>$raceName</RaceName>\n";
      echo "\t\t\t<Circuit circuitId=\"$circuitId\" url=\"$circuitUrl\">\n";
      echo "\t\t\t\t<CircuitName>$circuitName</CircuitName>\n";
      echo "\t\t\t\t<Location lat=\"$lat\" long=\"$long\">\n";
      echo "\t\t\t\t\t<Locality>$locality</Locality>\n";
      echo "\t\t\t\t\t<Country>$country</Country>\n";
      echo "\t\t\t\t</Location>\n";
      echo "\t\t\t</Circuit>\n";
      echo "\t\t\t<Date>$date</Date>\n";
      if($start) {
        $start .= "Z";
        echo "\t\t\t<Time>$start</Time>\n";
      }
      echo "\t\t</Race>\n";
    }
    echo "\t</RaceTable>\n";
    echo "</MRData>\n";
  } elseif(strcmp($format, "json") == 0) {
    
    if($callback) {
      doJSONPHeader($url, $series, $limit, $offset, $total, $callback);
    } else {
      doJSONHeader($url, $series, $limit, $offset, $total);
    }
    echo "\"RaceTable\":{";
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
    echo "\"Races\":[";
    if($numRows > 0) {
      for($i=1; $i<=$numRows; $i++) {
        $row = @mysql_fetch_array($result);
        $season = $row[0];
        $round = $row[1];
        $raceName = $row[2];
        $date = $row[3];
        $start = $row[4];
        $raceUrl = escape($row[5]);
        $circuitId = $row[6];
        $circuitName = $row[7];
        $locality = $row[8];
        $country= $row[9];
        $lat = $row[10];
        $long = $row[11];
        $alt = $row[12];
        $circuitUrl = escape($row[13]);
        echo "{";     
        echo "\"season\":\"$season\",";
        echo "\"round\":\"$round\",";
        if($raceUrl) echo "\"url\":\"$raceUrl\",";
        echo "\"raceName\":\"$raceName\",";
        echo "\"Circuit\":{";
        echo "\"circuitId\":\"$circuitId\",";
        echo "\"url\":\"$circuitUrl\",";
        echo "\"circuitName\":\"$circuitName\",";
        echo "\"Location\":{";
        echo "\"lat\":\"$lat\",";
        echo "\"long\":\"$long\",";
        echo "\"locality\":\"$locality\",";
        echo "\"country\":\"$country\"";
        echo "}},";
        echo "\"date\":\"$date\"";
        if($start) {
          $start .= "Z";
           echo ",\"time\":\"$start\"";
        }
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
  error(404, "Race Not Found");
}

?>