<?php
$user="<username>";
$password="<password>";
$database="<database>";
$connection = mysql_connect("localhost", $user, $password);
mysql_set_charset("utf8", $connection);
@mysql_select_db($database, $connection) or die("Unable to select database");
?>