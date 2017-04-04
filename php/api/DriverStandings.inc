<?php
if(isset($constructor) || isset($constructorStandings) || isset($circuits) || isset($grid) || isset($fastest) || isset($results) || isset($status)) error(400, "Bad Request: The qualifiers specified are not supported.");

$query = "SELECT SQL_CALC_FOUND_ROWS DISTINCT d.driverId, d.driverRef, d.number, d.code, d.forename, d.surname, d.dob, d.nationality, d.url, ds.points, ds.position, ds.positionText, ds.wins, r.year, r.round";
$query .= " FROM drivers d, driverStandings ds, races r";
$query .= " WHERE ds.raceId=r.raceId AND ds.driverId=d.driverId";
if($driverStandings) $query .= " AND ds.positionText='$driverStandings'";
if($driver) $query .= " AND d.driverRef='$driver'";
if($year) {
  $query .= " AND r.year='$year'"; 
}
if($round) {
  $query .= " AND r.round='$round'";
} else {
  if($year) {
    $query .= " AND r.round=(SELECT MAX(round) FROM driverStandings ds, races r WHERE ds.raceId=r.raceId AND r.year='$year')";
  } else {
    $query .= " AND (r.year, r.round) IN (SELECT year, MAX(round) FROM races GROUP BY year)";
  }
}

$query .= " ORDER BY r.year, ds.position LIMIT $offset, $limit";
$result = mysql_query($query) or die('I cannot select from the database because: ' . mysql_error());

$query = "SELECT FOUND_ROWS()";
$found_rows_result = mysql_query($query) or die('I cannot select from the database because: ' . mysql_error());
$row = mysql_fetch_row($found_rows_result);
$total = $row[0];
$previousYear = NULL

if(strcmp($format, "xml") == 0) {
  doXMLHeader($url, $series, $limit, $offset, $total);
  echo "\t<StandingsTable";
  if(isset($year)) echo " season=\"$year\"";
  if(isset($round)) echo " round=\"$round\"";
  if(isset($driver)) echo " driverId=\"$driver\"";
  if(isset($driverStandings)) echo " driverStandings=\"$driverStandings\"";

  echo ">\n";
  while($row = @mysql_fetch_array($result)) {
    $driverId = $row[0];
    $driverRef = $row[1];
    $permanentNumber = $row[2];
    $code = $row[3];
    $forename = $row[4];
    $surname = $row[5];
    $dob = $row[6];
    $nationality = $row[7];
    $url = $row[8];
    $points = $row[9];
    $position = $row[10];
    $positionText = $row[11];
    $wins = $row[12];
    $year = $row[13];
    $round = $row[14];
    
    if($year != $previousYear) {
      if($previousYear) echo "\t\t</StandingsList>\n";
      $previousYear = $year;    
      echo "\t\t<StandingsList";
      echo " season=\"$year\" round=\"$round\"";
      echo ">\n";
    }
    echo "\t\t\t<DriverStanding position=\"$position\" positionText=\"$positionText\" points=\"$points\" wins=\"$wins\">\n";
    if($code) {
      echo "\t\t\t\t<Driver driverId=\"$driverRef\" code=\"$code\" url=\"$url\">\n";      
    } else {
      echo "\t\t\t\t<Driver driverId=\"$driverRef\" url=\"$url\">\n";
    }
    if($permanentNumber) echo "\t\t\t\t\t<PermanentNumber>$permanentNumber</PermanentNumber>\n";
    echo "\t\t\t\t\t<GivenName>$forename</GivenName>\n";
    echo "\t\t\t\t\t<FamilyName>$surname</FamilyName>\n";
    echo "\t\t\t\t\t<DateOfBirth>$dob</DateOfBirth>\n";
    echo "\t\t\t\t\t<Nationality>$nationality</Nationality>\n";
    echo "\t\t\t\t</Driver>\n";
    
    $constructorResult = getConstructors($year, $round, $driverId);
    while($constructorRow = @mysql_fetch_array($constructorResult)) {
      $constructorRef = $constructorRow[0];
      $constr_name = $constructorRow[1];
      $constr_nationality = $constructorRow[2];
      $constr_url = $constructorRow[3];
      echo "\t\t\t\t<Constructor constructorId=\"$constructorRef\" url=\"$constr_url\">\n";
      echo "\t\t\t\t\t<Name>$constr_name</Name>\n";
      echo "\t\t\t\t\t<Nationality>$constr_nationality</Nationality>\n";
      echo "\t\t\t\t</Constructor>\n";
    }
    echo "\t\t\t</DriverStanding>\n";
  }
  if($previousYear) echo "\t\t</StandingsList>\n";
  echo "\t</StandingsTable>\n";
  echo "</MRData>\n";
} elseif(strcmp($format, "json") == 0) {
  if($callback) {
    doJSONPHeader($url, $series, $limit, $offset, $total, $callback);
  } else {
    doJSONHeader($url, $series, $limit, $offset, $total);
  }
  echo "\"StandingsTable\":{";
  if(isset($year)) echo "\"season\":\"$year\",";
  if(isset($round)) echo "\"round\":\"$round\",";
  if(isset($driver)) echo "\"driverId\":\"$driver\",";
  if(isset($driverStandings)) echo "\"driverStandings\":\"$driverStandings\",";
  
  $numRows = mysql_num_rows($result);
  echo "\"StandingsLists\":[";
  if($numRows > 0) {
    while($row = @mysql_fetch_array($result)) {
      $driverId = $row[0];
      $driverRef = $row[1];
      $permanentNumber = $row[2];
      $code = $row[3];
      $forename = $row[4];
      $surname = $row[5];
      $dob = $row[6];
      $nationality = $row[7];
      $url = escape($row[8]);
      $points = $row[9];
      $position = $row[10];
      $positionText = $row[11];
      $wins = $row[12];
      $year = $row[13];
      $round = $row[14];
        
      if($year != $previousYear) {
        if($previousYear) {
          echo "}]},";
        }
        $previousYear = $year;
        echo "{";
        echo "\"season\":\"$year\",";
        echo "\"round\":\"$round\",";
        echo "\"DriverStandings\":[";
      } else {
        if($previousYear) {
          echo "},";
        }
      }
      echo "{";     
      echo "\"position\":\"$position\",";
      echo "\"positionText\":\"$positionText\",";
      echo "\"points\":\"$points\",";
      echo "\"wins\":\"$wins\",";
      echo "\"Driver\":{";
      echo "\"driverId\":\"$driverRef\",";
      if($permanentNumber) echo "\"permanentNumber\":\"$permanentNumber\",";
      if($code) echo "\"code\":\"$code\",";
      echo "\"url\":\"$url\",";
      echo "\"givenName\":\"$forename\",";
      echo "\"familyName\":\"$surname\",";
      echo "\"dateOfBirth\":\"$dob\",";
      echo "\"nationality\":\"$nationality\"";
      echo "},";

      $constructorResult = getConstructors($year, $round, $driverId);
      $numRows = mysql_num_rows($constructorResult);
      echo "              \"Constructors\":[";
      if($numRows > 0) {
        for($i=1; $i<=$numRows; $i++) {
          $constructorRow = @mysql_fetch_array($constructorResult);
          $constructorRef = $constructorRow[0];
          $constr_name = $constructorRow[1];
          $constr_nationality = $constructorRow[2];
          $constr_url = escape($constructorRow[3]);
          echo "{";
          echo "\"constructorId\":\"$constructorRef\",";
          echo "\"url\":\"$constr_url\",";
          echo "\"name\":\"$constr_name\",";
          echo "\"nationality\":\"$constr_nationality\"";
          if($i == $numRows) {
            echo "}";
          } else {
            echo "},";
          }
        }
      }
      echo "]";
    }
    echo "}]}";
  }
  echo "]}}}";
  if($callback) {
    echo ")";
  }
}

function getConstructors($year, $round, $driverId) {
  $query = "SELECT DISTINCT c.constructorRef, c.name, c.nationality, c.url FROM constructors c, results re, races ra";
  $query .= " WHERE re.raceId=ra.raceId AND re.driverId='$driverId' AND c.constructorId=re.constructorId";
  $query .= " AND ra.year='$year'"; 
  $query .= " AND ra.round<='$round'";
  $result = mysql_query($query) or die('I cannot select from the database because: ' . mysql_error());
  return $result;
}
?>