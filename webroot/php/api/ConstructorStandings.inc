<?php
if(isset($driver) || isset($driverStandings) || isset($circuits) || isset($grid) || isset($fastest) || isset($results) || isset($status)) error(400, "Bad Request: The qualifiers specified are not supported.");

$query = "SELECT SQL_CALC_FOUND_ROWS DISTINCT c.constructorRef, c.name, c.nationality, c.url, cs.points, cs.position, cs.positionText, cs.wins, r.year, r.round";

$query .= " FROM constructors c, constructorStandings cs, races r";
$query .= " WHERE cs.raceId=r.raceId AND cs.constructorId=c.constructorId";
if($constructorStandings) $query .= " AND cs.positionText='$constructorStandings'";
if($constructor) $query .= " AND c.constructorRef='$constructor'";

if($year) $query .= " AND r.year='$year'"; 
if($round) {
  $query .= " AND r.round='$round'";
} else {
  if($year) {
    $query .= " AND r.round=(SELECT MAX(round) FROM driverStandings ds, races r WHERE ds.raceId=r.raceId AND r.year='$year')";
  } else {
    $query .= " AND (r.year, r.round) IN (SELECT year, MAX(round) FROM races GROUP BY year)";
  }
}

$query .= " ORDER BY r.year, cs.position LIMIT $offset, $limit";
//echo "query=$query<br/>\n";
$result = mysql_query($query) or die('I cannot select from the database because: ' . mysql_error());

$query = "SELECT FOUND_ROWS()";
$found_rows_result = mysql_query($query) or die('I cannot select from the database because: ' . mysql_error());
$row = mysql_fetch_row($found_rows_result);
$total = $row[0];

if(strcmp($format, "xml") == 0) {
  doXMLHeader($url, $series, $limit, $offset, $total);
  echo "\t<StandingsTable";
  if(isset($year)) echo " season=\"$year\"";
  if(isset($round)) echo " round=\"$round\"";
  if(isset($constructor)) echo " constructorId=\"$constructor\"";
  if(isset($constructorStandings)) echo " constructorStandings=\"$constructorStandings\"";

  echo ">\n";  
  while($row = @mysql_fetch_array($result)) {
    $constructorRef = $row[0];
    $name = $row[1];
    $nationality = $row[2];
    $url = $row[3];
    $points = $row[4];
    $position = $row[5];
    $positionText = $row[6];
    $wins = $row[7];
    $year = $row[8];
    $round = $row[9];

    if($year != $previousYear) {
      if($previousYear) echo "\t\t</StandingsList>\n";
      $previousYear = $year;
      echo "\t\t<StandingsList";
      echo " season=\"$year\" round=\"$round\"";
      echo ">\n";
    }
    echo "\t\t\t<ConstructorStanding position=\"$position\" positionText=\"$positionText\" points=\"$points\" wins=\"$wins\">\n";
    echo "\t\t\t\t<Constructor constructorId=\"$constructorRef\" url=\"$url\">\n";
    echo "\t\t\t\t\t<Name>$name</Name>\n";
    echo "\t\t\t\t\t<Nationality>$nationality</Nationality>\n";
    echo "\t\t\t\t</Constructor>\n";
    echo "\t\t\t</ConstructorStanding>\n";
  }
  if($previousYear) echo "\t\t</StandingsList>\n";
  echo "\t</StandingsTable>";
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
  if(isset($constructor)) echo "\"constructorId\":\"$constructor\",";
  if(isset($constructorStandings)) echo "\"constructorStandings\":\"$constructorStandings\",";
  
  $numRows = mysql_num_rows($result);
  echo "\"StandingsLists\":[";
  if($numRows > 0) {
    while($row = @mysql_fetch_array($result)) {
      $constructorRef = $row[0];
      $name = $row[1];
      $nationality = $row[2];
      $url = $row[3];
      $points = $row[4];
      $position = $row[5];
      $positionText = $row[6];
      $wins = $row[7];
      $year = $row[8];
      $round = $row[9];

      if($year != $previousYear) {
        if($previousYear) {
          echo "}]},";
        }
        $previousYear = $year;
        echo "{";
        echo "\"season\":\"$year\",";
        echo "\"round\":\"$round\",";
        echo "\"ConstructorStandings\":[";
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
      echo "\"Constructor\":{";
      echo "\"constructorId\":\"$constructorRef\",";
      echo "\"url\":\"$url\",";
      echo "\"name\":\"$name\",";
      echo "\"nationality\":\"$nationality\"";
      echo "}";        
    }
    echo "}]}";
  }
  echo "]}}}"; 
  if($callback) {
    echo ")";
  }
}
?>