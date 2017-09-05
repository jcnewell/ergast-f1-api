<?php
if(($driverStandings || $constructorStandings) && ($circuits || $grid || $results || $status)) error(400, "Bad Request: Cannot combine standings with circuit, grid, result or status qualifiers.");

$query = "SELECT SQL_CALC_FOUND_ROWS DISTINCT dr.driverRef, dr.number, dr.code, dr.forename, dr.surname, dr.dob, dr.nationality, dr.url FROM drivers dr";

if($year || $constructor || $status || $grid || $results || $circuit || $fastest) $query .= ", results re";
if($year || $circuit || $driverStandings || $constructorStandings) $query .= ", races ra";
if($driverStandings || $constructorStandings) $query .= ", driverStandings ds";
if($constructorStandings) $query .= ", constructorStandings cs";
if($circuit) $query .= ", circuits ci";
if($constructor) $query .= ", constructors co";
$query .= " WHERE TRUE";

if($driverStandings || $constructorStandings) {

  if($year || $constructor) $query .= " AND dr.driverId=re.driverId";
  if($year) $query .= " AND re.raceId=ra.raceId";
  if($constructor) $query .= " AND re.constructorId=co.constructorId AND co.constructorRef='$constructor'";
  if($driver) $query .= " AND dr.driverRef='$driver'";

  if($driverStandings) $query .= " AND ds.positionText='$driverStandings'";
  if($driverStandings || $constructorStandings) $query .= " AND ds.raceId=ra.raceId";
  if($driverStandings || $constructorStandings) $query .= " AND dr.driverId=ds.driverId";
  if($constructorStandings) $query .= " AND cs.raceId=ra.raceId AND cs.positionText='$constructorStandings'";
  if($constructor && $constructorStandings) $query .= " AND co.constructorId=cs.constructorId";

} else {
  if($year || $constructor || $status || $grid || $results || $circuit || $fastest) $query .= " AND dr.driverId=re.driverId";
  if($year || $circuit) $query .= " AND re.raceId=ra.raceId";
  if($circuit) $query .= " AND ra.circuitId=ci.circuitId AND ci.circuitRef='$circuit'";
  if($constructor) $query .= " AND re.constructorId=co.constructorId AND co.constructorRef='$constructor'";
  if($status) $query .= " AND re.statusId='$status'";
  if($grid) $query .= " AND re.grid='$grid'";
  if($fastest) $query .= " AND re.rank='$fastest'";
  if($results) $query .= " AND re.positionText='$results'";
  if($driver) $query .= " AND dr.driverRef='$driver'";
}

if($year) $query .= " AND ra.year='$year'";
if($round) {
  $query .= " AND ra.round='$round'";
} else {
  if($driverStandings || $constructorStandings) {
    if($year) {
      $query .= " AND ra.round=(SELECT MAX(round) FROM races WHERE races.year='$year')";
    } else {
      $query .= " AND (ra.year, ra.round) IN (SELECT year, MAX(round) FROM races GROUP BY year)";
    }
  }
}

$query .= " ORDER BY dr.surname LIMIT $offset, $limit";
//echo "$query<br>\n<br>\n";

$result = mysql_query($query) or die('I cannot select from the database because: ' . mysql_error());
if($result) {
  $query = "SELECT FOUND_ROWS()";
  $found_rows_result = mysql_query($query) or die('I cannot select from the database because: ' . mysql_error());
  $row = mysql_fetch_row($found_rows_result);
  $total = $row[0];
  if(strcmp($format, "xml") == 0) {
    doXMLHeader($url, $series, $limit, $offset, $total);
    echo "\t<DriverTable";
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
      $driverId = $row[0];
      $permanentNumber = $row[1];
      $code = $row[2];
      $forename = $row[3];
      $surname = $row[4];
      $dob = $row[5];
      $nationality = $row[6];
      $url = $row[7];
      if($code) {
        echo "\t\t<Driver driverId=\"$driverId\" code=\"$code\" url=\"$url\">\n";
      } else {
        echo "\t\t<Driver driverId=\"$driverId\" url=\"$url\">\n";
      }
      if($permanentNumber) echo "\t\t\t<PermanentNumber>$permanentNumber</PermanentNumber>\n";
      echo "\t\t\t<GivenName>$forename</GivenName>\n";
      echo "\t\t\t<FamilyName>$surname</FamilyName>\n";
      echo "\t\t\t<DateOfBirth>$dob</DateOfBirth>\n";
      echo "\t\t\t<Nationality>$nationality</Nationality>\n";
      echo "\t\t</Driver>\n";
    }
    echo "\t</DriverTable>\n";
    echo "</MRData>\n";
  } elseif(strcmp($format, "json") == 0) {
    if($callback) {
      doJSONPHeader($url, $series, $limit, $offset, $total, $callback);
    } else {
      doJSONHeader($url, $series, $limit, $offset, $total);
    }
    echo "\"DriverTable\":{";
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
    echo "\"Drivers\":[";
    if($numRows > 0) {
      for($i=1; $i<=$numRows; $i++) {
        $row = @mysql_fetch_array($result);
        $driverId = $row[0];
        $permanentNumber = $row[1];
        $code = $row[2];
        $forename = $row[3];
        $surname = $row[4];
        $dob = $row[5];
        $nationality = $row[6];
        $url = escape($row[7]);
        echo "{";
        echo "\"driverId\":\"$driverId\",";
        if($permanentNumber) echo "\"permanentNumber\":\"$permanentNumber\",";
        if($code) echo "\"code\":\"$code\",";
        echo "\"url\":\"$url\",";
        echo "\"givenName\":\"$forename\",";
        echo "\"familyName\":\"$surname\",";
        echo "\"dateOfBirth\":\"$dob\",";
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
  error(404, "Driver Not Found");
}

?>