# ergast-f1-api
A PHP-based API using the [Ergast Formula One MySQL database](http://ergast.com/mrd/)

## Configuration
Obtain a MySQL database image from: [http://ergast.com/mrd/db/](http://ergast.com/mrd/db/)

Restore the database and set the username and password for a user with read-only privileges in [f1dbro.inc](https://github.com/jcnewell/ergast-f1-api/blob/master/php/api/f1dbro.inc)

Add a rewrite rule to the .htaccess file in the root directory of the server e.g.

<IfModule mod_rewrite.c>
  RewriteEngine On
  RewriteBase /
  RewriteRule ^api/(.*) /php/api/index.php [L]
</IfModule>

This example assumes the API code is installed in /php/api and the desired location of the API is /api
