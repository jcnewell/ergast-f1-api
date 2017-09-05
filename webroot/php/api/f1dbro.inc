<?php
$user="root";
$password="f1";
$database="ergastdb";
$connection = mysql_connect("ergastdb", $user, $password);
mysql_set_charset("utf8", $connection);
@mysql_select_db($database, $connection) or die("Unable to select database");
?>