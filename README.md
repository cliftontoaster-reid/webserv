_This project has been created as part of the 42 curriculum by jfranc, lfiorell_

# Webserv

## Description

Webserv is an HTTP server written in C++ 98, developed as part of the 42 school
curriculum. It handles client requests using the HTTP protocol, serving static
files, executing CGI scripts, and supporting file uploads. The server is
non-blocking, using epoll for I/O multiplexing, and is configured via TOML
configuration files.

## Instructions

### Prerequisites

- C++ compiler (clang++ or g++)
- make
- Linux (macOS users may need additional `fcntl` flags)

### Build

```bash
make               # debug mode (default)
make MODE=release
```

### Configure

Create a `config.toml` file wherever. Here is an example from
[the docker example](./example/config/config.toml):

```toml
[[server]]
port = 1998

[[server.host]]
hostname = ""

[server.host.error]
404 = "/var/www/html/errors/404.html"

[[server.host.route]]
preffix = "/"
path = "/var/www/html/cgi"
index = "index.php"

[[server.host.route]]
preffix = "/upload"
path = "/srv/upload"
index = "index.noway"

[[server.host.route]]
preffix = "/imgs"
path = "/usr"

[[server.host.api]]
uri = "/api/upload"
external = false
func = "upload"
upload = { path = "/srv/upload", random_len = 4 }

[[server.host.cgi]]
glob = "/*.php"
bin = "/usr/bin/php-cgi"
```

### Run

```bash
./webserv <config-file>
```

Example configuration is provided in `example/config/config.toml`.

### Docker

```bash
cd example
docker compose up --build
```

### Tests

```bash
make test           # project tests
make test-all       # all tests including libraries
```

## Resources

- [RFC 1945 - HTTP/1.0](https://www.rfc-editor.org/rfc/rfc1945)
- [Developer Mozilla Documentation - HTTP response status codes](https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Status)
- [CGI Specification](https://docs.python.org/3/library/cgi.html)
- [TOML specification](https://toml.io/en/v1.1.0)
