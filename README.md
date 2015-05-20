# Arduino_PHP_MySQL_Webapp
Example of storing Arduino data to HTTP server with a typical LAMP stack.

There are three folders for keeping the code for the different sections.
Ardiuno: has all Arduino code, default for UNO, but adaptable for Mega.

http_server: has PHP code, namely Arduino_HTTP_POST.php and client_HTTP_GET.php.

http_client: contains code that is served up by the HTTP server to run in a web browser.

This simple example is based on an Arduino UNO and uses 6 analog inputs.
The internal ADC Vref is set to 1.1 Volts as opposed to more commonly used 5.0 Volts, but can be changed easliy enough in the program. It was set to 1.1 Volts as the original project used industrial temperature sensors that were 0 to 1.0 Volts.

The Arduino program uses timer interrupts to obtain accurate timings to perform its functions.
On startup the program connects to a NTP server to get current UTC time.

The Arduino connects to a HTTP server and uses the HTTP POST command to pass data to the server.
Data is formatted in the Arduino into basic json style before data is posted.

The HTTP server acts on the HTTP POST it gets from the Arduino and processes it to either simple text file or to MySQL database. Storing to text file for data that changes often and where no accumulated historical records are required provides a simple and effective method for next to real time data.

Data that needs accumulated history is placed into a MySQL database.

In the folder http-server, there is PHP code used by the HTTP client side, such as a web broweser running a web application.
