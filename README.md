# SQL-GUI - a SQLite database query tool

![screenshot](screenshot.png)

## Building

	% make

## Running

	% ./sql-gui [database] [sql]

You can use the included sample database like this:

	% ./sql-gui sql-murder-mystery.db "select * from person"

## Thanks

Made with the excellent [Dear ImGui](https://github.com/ocornut/imgui) (MIT License), and [SQLite](https://www.sqlite.org/) (Public Domain). The sample database is from the amazing [SQL Murder Mystery](https://github.com/NUKnightLab/sql-mysteries) (MIT License).

