<?php
if($driverStandings || $constructorStandings) error(400, "Bad Request: Race queries do not support standings qualifiers.");

$query = "SELECT SQL_CALC_FOUND_ROWS ra.year, ra.round, ra.name, ra.date, ra.time, ra.url, ra.fp1_date, ra.fp1_time, ra.fp2_date, ra.fp2_time, ra.fp3_date, ra.fp3_time, ra.quali_date, ra.quali_time, ra.sprint_date, ra.sprint_time, c.circuitRef, c.name, c.location, c.country, c.lat, c.lng, c.alt, c.url FROM races ra, circuits c";

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
$result = mysqli_query($mysqli, $query) or die('I cannot select from the database because: ' . mysqli_error());
if($result) {
  $query = "SELECT FOUND_ROWS()";
  $found_rows_result = mysqli_query($mysqli, $query) or die('I cannot select from the database because: ' . mysqli_error());
  $row = mysqli_fetch_row($found_rows_result);
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
    while($row = @mysqli_fetch_array($result)) {
      $season = $row[0];
      $round = $row[1];
      $raceName = $row[2];
      $date = $row[3];
      $start = $row[4];
      $raceUrl = $row[5];
      $fp1Date = $row[6];
      $fp1Time = $row[7];
      $fp2Date = $row[8];
      $fp2Time = $row[9];
      $fp3Date = $row[10];
      $fp3Time = $row[11];
      $qualiDate = $row[12];
      $qualiTime = $row[13];
      $sprintDate = $row[14];
      $sprintTime = $row[15];
      $circuitId = $row[16];
      $circuitName = $row[17];
      $locality = $row[18];
      $country= $row[19];
      $lat = $row[20];
      $long = $row[21];
      $alt = $row[22];
      $circuitUrl = $row[23];
      
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
      
      if($fp1Date) {
        echo "\t\t\t<FirstPractice>\n";
        echo "\t\t\t\t<Date>$fp1Date</Date>\n";
        if($fp1Time) {
          $fp1Time .= "Z";
          echo "\t\t\t\t<Time>$fp1Time</Time>\n";
        }
        echo "\t\t\t</FirstPractice>\n";
      }
      
      if($sprintDate) {
        if($qualiDate) {
          echo "\t\t\t<Qualifying>\n";
          echo "\t\t\t\t<Date>$qualiDate</Date>\n";
          if($qualiTime) {
            $qualiTime .= "Z";
            echo "\t\t\t\t<Time>$qualiTime</Time>\n";
          }
          echo "\t\t\t</Qualifying>\n";
        }
      }
      
      if($fp2Date) {
        echo "\t\t\t<SecondPractice>\n";
        echo "\t\t\t\t<Date>$fp2Date</Date>\n";
        if($fp2Time) {
          $fp2Time .= "Z";
          echo "\t\t\t\t<Time>$fp2Time</Time>\n";
        }
        echo "\t\t\t</SecondPractice>\n";
      }
      
      if($fp3Date) {
        echo "\t\t\t<ThirdPractice>\n";
        echo "\t\t\t\t<Date>$fp3Date</Date>\n";
        if($fp3Time) {
          $fp3Time .= "Z";
          echo "\t\t\t\t<Time>$fp3Time</Time>\n";
        }
        echo "\t\t\t</ThirdPractice>\n";
      }
      
      if($sprintDate) {
        echo "\t\t\t<Sprint>\n";
        echo "\t\t\t\t<Date>$sprintDate</Date>\n";
        if($sprintTime) {
          $sprintTime .= "Z";
          echo "\t\t\t\t<Time>$sprintTime</Time>\n";
        }
        echo "\t\t\t</Sprint>\n";
      
      } else {
        if($qualiDate) {
          echo "\t\t\t<Qualifying>\n";
          echo "\t\t\t\t<Date>$qualiDate</Date>\n";
          if($qualiTime) {
            $qualiTime .= "Z";
            echo "\t\t\t\t<Time>$qualiTime</Time>\n";
          }
          echo "\t\t\t</Qualifying>\n";
        }
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
    
    $numRows = mysqli_num_rows($result);
    echo "\"Races\":[";
    if($numRows > 0) {
      for($i=1; $i<=$numRows; $i++) {
        $row = @mysqli_fetch_array($result);
        $season = $row[0];
        $round = $row[1];
        $raceName = $row[2];
        $date = $row[3];
        $start = $row[4];
        $raceUrl = escape($row[5]);
        $fp1Date = $row[6];
        $fp1Time = $row[7];
        $fp2Date = $row[8];
        $fp2Time = $row[9];
        $fp3Date = $row[10];
        $fp3Time = $row[11];
        $qualiDate = $row[12];
        $qualiTime = $row[13];
        $sprintDate = $row[14];
        $sprintTime = $row[15];
        $circuitId = $row[16];
        $circuitName = $row[17];
        $locality = $row[18];
        $country= $row[19];
        $lat = $row[20];
        $long = $row[21];
        $alt = $row[22];
        $circuitUrl = escape($row[23]);
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
        
        if($fp1Date) {
          echo ",\"FirstPractice\":{";
            echo "\"date\":\"$fp1Date\"";
            if($fp1Time) {
              $fp1Time .= "Z";
              echo ",\"time\":\"$fp1Time\"";
            }
          echo "}";
        }
        
        if($sprintDate) {
          if($qualiDate) {
            echo ",\"Qualifying\":{";
            echo "\"date\":\"$qualiDate\"";
            if($qualiTime) {
               $qualiTime .= "Z";
               echo ",\"time\":\"$qualiTime\"";
            }
            echo "}";
          }
        }
        
        if($fp2Date) {
          echo ",\"SecondPractice\":{";
          echo "\"date\":\"$fp2Date\"";
          if($fp2Time) {
             $fp2Time .= "Z";
             echo ",\"time\":\"$fp2Time\"";
          }
          echo "}";
        }
        
        if($fp3Date) {
          echo ",\"ThirdPractice\":{";
          echo "\"date\":\"$fp3Date\"";
          if($fp3Time) {
             $fp3Time .= "Z";
             echo ",\"time\":\"$fp3Time\"";
          }
          echo "}";
        }
        
        if($sprintDate) {
          echo ",\"Sprint\":{";
          echo "\"date\":\"$sprintDate\"";
          if($sprintTime) {
             $sprintTime .= "Z";
             echo ",\"time\":\"$sprintTime\"";
          }
          echo "}";
        
        } else {
          if($qualiDate) {
            echo ",\"Qualifying\":{";
            echo "\"date\":\"$qualiDate\"";
            if($qualiTime) {
               $qualiTime .= "Z";
               echo ",\"time\":\"$qualiTime\"";
            }
            echo "}";
          }
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