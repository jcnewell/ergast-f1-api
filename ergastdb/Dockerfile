FROM mariadb

RUN apt update && apt-get install -y curl
RUN curl -o /docker-entrypoint-initdb.d/f1db.sql.gz http://ergast.com/downloads/f1db.sql.gz
RUN gunzip /docker-entrypoint-initdb.d/f1db.sql.gz
