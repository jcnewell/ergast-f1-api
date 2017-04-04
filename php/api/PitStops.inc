<?php
if($driverStandings || $constructorStandings) error(400, "Bad Request: Pit stop queries do not support standings qualifiers.");
if(isset($circuits) || isset($grid) || isset($fastest) || isset($results) || isset($status)) error(400, "Bad Request: Pit stop queries do not support the specified qualifiers.");
if(!isset($year) || !isset($round)) error(400, "Bad Request: Pit stop queries require a season and round to be specified.");

$query = "SELECT SQL_CALC_FOUND_ROWS
  ra.year, ra.round, ra.name, ra.date, ra.time, ra.url, 
  ci.circuitRef, ci.name, ci.location, ci.country, ci.url, ci.lat, ci.lng, ci.alt,
  dr.driverRef,
  pi.stop, pi.lap, pi.time, pi.duration
  FROM pitStops pi, races ra, circuits ci, drivers dr";

$query .= " WHERE ra.circuitId=ci.circuitId AND pi.driverId=dr.driverId AND pi.raceId=ra.raceId AND ra.year='$year' AND ra.round='$round'";

if($driver) $query .= " AND dr.driverRef='$driver'";
if($pitstops) $query .= " AND pi.stop='$pitstops'";
if($laps) $query .= " AND pi.lap='$laps'";

$query .= " ORDER BY pi.time LIMIT $offset, $limit";

//echo "$query<br>\n<br>\n";
$result = mysql_query($query) or die('I cannot select from the database because: ' . mysql_error());

if($result) {
  $query = "SELECT FOUND_ROWS()";
  $found_rows_result = mysql_query($query) or die('I cannot select from the database because: ' . mysql_error());
  $row = mysql_fetch_row($found_rows_result);
  $total = $row[0];
  $doRaceHeader = true;
  
  if(strcmp($format, "xml") == 0) {
    doXMLHeader($url, $series, $limit, $offset, $total);
    
    $doRaceHeader = true;
    echo "\t<RaceTable";
    echo " season=\"$year\"";
    echo " round=\"$round\"";
    if(isset($driver)) echo " driverId=\"$driver\"";
    if(isset($pitstops)) echo " stop=\"$pitstops\"";
    if(isset($laps)) echo " lap=\"$laps\"";
    echo ">\n";
    
    while($row = @mysql_fetch_array($result)) {
      $driverRef = $row[14];
      $stop = $row[15];
      $lap = $row[16];
      $time = $row[17];
      $duration = $row[18];
      
      if($doRaceHeader) {
        $doRaceHeader = false;
        $year = $row[0];
        $round = $row[1];
        $raceName = $row[2];
        $date = $row[3];
        $start = $row[4];
        $raceUrl = $row[5];
        $circuitId = $row[6];
        $circuitName = $row[7];
        $locality = $row[8];
        $country = $row[9];
        $circuitUrl = $row[10];
        $lat = $row[11];
        $long = $row[12];
        echo "\t\t<Race season=\"$year\" round=\"$round\" url=\"$raceUrl\">\n";
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
        echo "\t\t\t<PitStopsList>\n";
      }
      echo "\t\t\t\t<PitStop driverId=\"$driverRef\" stop=\"$stop\" lap=\"$lap\" time=\"$time\" duration=\"$duration\"/>\n";
    }
    if($doRaceHeader == false) {
      echo "\t\t\t</PitStopsList>\n";
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
    echo "\"season\":\"$year\"";
    echo ",\"round\":\"$round\"";
    if(isset($driver)) echo ",\"driverId\":\"$driver\"";
    if(isset($pitstops)) echo ",\"stop\":\"$pitstops\"";
    if(isset($laps)) echo ",\"lap\":\"$laps\"";
    
    $numRows = mysql_num_rows($result);
    echo ",\"Races\":[";
    if($numRows > 0) {
      while($row = @mysql_fetch_array($result)) {
        $driverRef = $row[14];
        $stop = $row[15];
        $lap = $row[16];
        $time = $row[17];
        $duration = $row[18];
        
        if($doRaceHeader) {
          $doRaceHeader = false;
          $year = $row[0];
          $round = $row[1];
          $raceName = $row[2];
          $date = $row[3];
          $start = $row[4];
          $raceUrl = escape($row[5]);
          $circuitId = $row[6];
          $circuitName = $row[7];
          $locality = $row[8];
          $country = $row[9];
          $circuitUrl = $row[10];
          $lat = $row[11];
          $long = $row[12];
          echo "{";
          echo "\"season\":\"$year\",";
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
          echo "}";
          echo "},";
          echo "\"date\":\"$date\",";
          if($start) {
            $start .= "Z";
            echo "\"time\":\"$start\",";
          }
          echo "\"PitStops\":[";
        } else {
          echo "},";
        }
        echo "{";     
        echo "\"driverId\":\"$driverRef\",";
        echo "\"lap\":\"$lap\",";
        echo "\"stop\":\"$stop\",";
        echo "\"time\":\"$time\",";
        echo "\"duration\":\"$duration\"";
      }
      echo "}]}";
    }
    echo "]}}}";
    if($callback) {
      echo ")";
    }
  }
} else {
  error(404, "Not Found");
}

?>