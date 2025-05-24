# 🌐 Webserv
*A lightweight HTTP server written in C++*

## 🚀 Overview

**Webserv** is a custom-built HTTP server developed as part of the 42 curriculum. The goal is to implement core components of an HTTP/1.1-compliant server using **C++**, with support for essential HTTP methods, static file serving, CGI, and configuration parsing — all without using any external libraries beyond the C++98 standard.

## 🔧 Features

- 📡 HTTP/1.1 compliant server
- ✅ Support for **GET**, **POST**, and **DELETE**
- 🧾 Custom configuration file format (like NGINX)
- 🗂️ Multiple virtual servers and location blocks
- 🖼️ Static file serving
- ⚙️ CGI support (Python, PHP, etc.)
- 🔁 Redirections and error pages
- 💾 Chunked transfer encoding
- 📂 Directory listing (optional)
- ♻️ Non-blocking I/O with `select()`

## 🛠 Configuration Example

```conf
server {
    listen 8080;
    server_name localhost;

    root ./www;

    location / {
        index index.html;
    }

    location /upload {
        methods POST;
        upload_store ./uploads;
    }

    error_page 404 /errors_pages/html/404.html;
}
```

## 📜 Requirements Met

- ✔️ Multi-server and multi-location support
- ✔️ Full HTTP parsing and header support
- ✔️ Methods: GET, POST, DELETE
- ✔️ CGI execution support
- ✔️ Chunked body parsing
- ✔️ Custom config file and error pages
- ✔️ C++98 only, no external libs

## 📚 Learning Outcomes

This project teaches you about:
- Low-level network programming with sockets
- Parsing and handling HTTP traffic manually
- Creating a performant server architecture
- Handling asynchronous I/O and multi-client logic
- Writing clean, modular, and testable C++ code

## 🏁 Conclusion

Webserv is a rewarding challenge that pushes your understanding of networking, system calls, and server design. It bridges the gap between system-level programming and real-world software architecture — giving you a solid foundation to explore more advanced web technologies and servers.

