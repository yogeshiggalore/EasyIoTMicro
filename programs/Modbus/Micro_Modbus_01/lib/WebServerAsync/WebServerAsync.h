#ifndef _HEADER_WEBSERVERASYNC_H_
#define _HEADER_WEBSERVERASYNC_H_

String get_content_type(String filename); // convert the file extension to the MIME type

/* ESP Server functions*/
void setup_server(void);
bool server_handle_file_read(AsyncWebServerRequest *request); // send the right file to the client (if it exists)

// ESP url functions
void server_handle_root(AsyncWebServerRequest *request);
void server_update_web_page(AsyncWebServerRequest *request);

#endif