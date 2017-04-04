<?php
if($driverStandings || $constructorStandings) error(400, "Bad Request: Status queries do not support standings qualifiers.");

$query = "SELECT DISTINCT SQL_CALC_FOUND_ROWS st.statusId, st.status, COUNT(*) FROM status st";

if($year || $round || $circuit) $query .= ", races ra";
$query .= ", results re";
if($driver) $query .= ", drivers dr";
if($constructor) $query .= ", constructors co";
if($circuit) $query .= ", circuits ci";
$query .= " WHERE TRUE";
if($status) $query .= " AND st.statusId='$status'";
$query .= " AND re.statusId=st.statusId";
if($year || $round || $circuit) $query .= " AND re.raceId=ra.raceId";
if($constructor) $query .= " AND re.constructorId=co.constructorId AND co.constructorRef='$constructor'";
if($driver) $query .= " AND re.driverId=dr.driverId AND dr.driverRef='$driver'";
if($circuit) $query .= " AND ra.circuitId=ci.circuitId AND ci.circuitRef='$circuit'";
if($grid) $query .= " AND re.grid='$grid'";
if($fastest) $query .= " AND re.rank='$fastest'";
if($results) $query .= " AND re.positionText='$results'";
if($year) $query .= " AND ra.year='$year'";
if($round) $query .= " AND ra.round='$round'";

$query .= " GROUP BY st.statusId ORDER BY st.statusId LIMIT $offset, $limit";

//echo "$query<br>\n<br>\n";
$result = mysql_query($query) or die('I cannot select from the database because: ' . mysql_error());
if($result) {
  $query = "SELECT FOUND_ROWS()";
  $found_rows_result = mysql_query($query) or die('I cannot select from the database because: ' . mysql_error());
  $row = mysql_fetch_row($found_rows_result);
  $total = $row[0];
  if(strcmp($format, "xml") == 0) {
    doXMLHeader($url, $series, $limit, $offset, $total);
    echo "\t<StatusTable";
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
      $statusId = $row[0];
      $status = $row[1];
      $count = $row[2];
      echo "\t\t<Status statusId=\"$statusId\" count=\"$count\">$status</Status>\n";
    }
    echo "\t</StatusTable>\n";
    echo "</MRData>\n";
  } elseif(strcmp($format, "json") == 0) {
    if($callback) {
      doJSONPHeader($url, $series, $limit, $offset, $total, $callback);
    } else {
      doJSONHeader($url, $series, $limit, $offset, $total);
    }
    echo "\"StatusTable\":{";
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
    echo "\"Status\":[";
    if($numRows > 0) {
      for($i=1; $i<=$numRows; $i++) {
        $row = @mysql_fetch_array($result);
        $statusId = $row[0];
        $status = $row[1];
        $count = $row[2];
        echo "{\"statusId\":\"$statusId\",\"count\":\"$count\",\"status\":\"$status\"}";
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