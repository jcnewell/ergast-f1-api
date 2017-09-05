<?php
if(($driverStandings || $constructorStandings) && ($circuits || $grid || $fastest || $results || $status)) error(400, "Bad Request: Cannot combine standings with circuit, grid, result or status qualifiers.");
$query = "SELECT SQL_CALC_FOUND_ROWS DISTINCT s.year, s.url FROM seasons s";

if($driver) $query .= ", drivers dr";
if($constructor) $query .= ", constructors co";
if($driverStandings || $constructorStandings) {
  $query .= ", races ra";
  if($driverStandings || $driver) $query .= ", driverStandings ds";
  if($constructorStandings || $constructor) $query .= ", constructorStandings cs";
} else {
  if($year || $circuit || $driver || $constructor || $status || $results || $grid || $fastest) $query .= ", races ra";
  if($driver || $constructor || $status || $results || $grid || $fastest) $query .= ", results re";
  if($circuit) $query .= ", circuits ci";
}
$query .= " WHERE TRUE";

if($driverStandings || $constructorStandings) {
  $query .= " AND s.year=ra.year";
  if($constructorStandings || $constructor) $query .= " AND cs.raceId=ra.raceId";
  if($constructor) $query .= " AND cs.constructorId=co.constructorId AND co.constructorRef='$constructor'";
  if($constructorStandings) $query .= " AND cs.positionText='$constructorStandings'";
  if($driverStandings || $driver) $query .= " AND ds.raceId=ra.raceId";
  if($driver) $query .= " AND ds.driverId=dr.driverId AND dr.driverRef='$driver'";
  if($driverStandings) $query .= " AND ds.positionText='$driverStandings'";
  if($year) $query .= " AND s.year='$year'";
  if($round) {
    $query .= " AND ra.round='$round'";
  } else {
    if($year) {
      $query .= " AND ra.round=(SELECT MAX(round) FROM races WHERE races.year='$year')";
    } else {
      $query .= " AND (ra.year, ra.round) IN (SELECT year, MAX(round) FROM races GROUP BY year)";
    }
  }

} else {
  if($year || $circuit || $driver || $constructor || $status || $results || $grid || $fastest) $query .= " AND s.year=ra.year";
  if($circuit) $query .= " AND ra.circuitId=ci.circuitId AND ci.circuitRef='$circuit'";
  if($driver || $constructor || $status || $results || $grid || $fastest ) $query .= " AND ra.raceId=re.raceId";
  if($constructor) $query .= " AND re.constructorId=co.constructorId AND co.constructorRef='$constructor'";
  if($driver) $query .= " AND re.driverId=dr.driverId AND dr.driverRef='$driver'";
  if($status) $query .= " AND re.statusId='$status'";
  if($grid) $query .= " AND re.grid='$grid'";
  if($fastest) $query .= " AND re.rank='$fastest'";
  if($results) $query .= " AND re.positionText='$results'";
  if($year) $query .= " AND s.year='$year'";
  if($round) {
    $query .= " AND ra.round='$round'";
  }
}

$query .= " ORDER BY s.year LIMIT $offset, $limit";
//echo "$query<br>\n<br>\n";

$result = mysql_query($query) or die('I cannot select from the database because: ' . mysql_error());
if($result) {
  $query = "SELECT FOUND_ROWS()";
  $found_rows_result = mysql_query($query) or die('I cannot select from the database because: ' . mysql_error());
  $row = mysql_fetch_row($found_rows_result);
  $total = $row[0];
  if(strcmp($format, "xml") == 0) {
    doXMLHeader($url, $series, $limit, $offset, $total);
    echo "\t<SeasonTable";
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
      $season = $row['year'];
      $url = $row['url'];
      echo "\t\t<Season url=\"$url\">$season</Season>\n";
    }
    echo "\t</SeasonTable>\n";
    echo "</MRData>\n";
  } elseif(strcmp($format, "json") == 0) {
    if($callback) {
      doJSONPHeader($url, $series, $limit, $offset, $total, $callback);
    } else {
      doJSONHeader($url, $series, $limit, $offset, $total);
    }
    echo "\"SeasonTable\":{";
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
    echo "\"Seasons\":[";
    if($numRows > 0) {
      for($i=1; $i<=$numRows; $i++) {
        $row = @mysql_fetch_array($result);
        $season = $row['year'];
        $url = escape($row['url']);
        echo "{\"season\":\"$season\",\"url\":\"$url\"}";
        if($i != $numRows) {
          echo ",";
        }
      }
    }
    echo "]}}}";
    if($callback) {
      echo ")";
    }
  }
}

?>
