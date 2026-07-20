# Configuration

Webserv uses a TOML configuration file to define servers, routes, handlers, CGI
scripts, and more. The file is passed as the first argument to the binary:

```
./webserv /etc/webserv/config.toml
```

## Config Flow

```
config.toml  -->  toml98::readTomlFile()  -->  toml98::Value (AST)
  -->  webserv::Config(root)                       (parse TOML AST into structs)
  -->  Config::implement()                         (register everything into Router + Listener)
  -->  Server::run()                               (event loop)
```

## Full Example

```toml
#:schema ../../docs/config.schema.json

[[server]]
port = 1998
max_body = 1048576

[[server.host]]
hostname = ""

[server.host.error]
404 = "/var/www/html/errors/404.html"

[[server.host.route]]
preffix = "/"
path = "/var/www/html/cgi"
index = "index.php"
methods = { GET = true, POST = true, DELETE = false }

[[server.host.route]]
preffix = "/upload"
path = "/srv/upload"
index = "index.noway"
methods = { GET = true, POST = true, DELETE = true }

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

[[server.host.redirect]]
preffix = "/old"
location = "/new"
```

## Section Reference

| Section | File | Description |
|---------|------|-------------|
| `[[server]]` | [server.md](config/server.md) | Server blocks: port, max_body |
| `[[server.host]]` | [host.md](config/host.md) | Hosts: hostname, error pages |
| `[[server.host.route]]` | [routes.md](config/routes.md) | Routes: preffix, path, index, methods, options |
| `[[server.host.redirect]]` | [redirects.md](config/redirects.md) | Internal URL rewrites |
| `[[server.host.api]]` | [api.md](config/api.md) | Built-in API handlers: echo, upload |
| `[[server.host.cgi]]` | [cgi.md](config/cgi.md) | CGI: glob patterns, binary paths |
| glob syntax | [glob-syntax.md](config/glob-syntax.md) | Glob pattern reference for CGI |

## Defaults & Validation

| Field | Required | Type | Default | Validated At |
|-------|----------|------|---------|-------------|
| `server` | yes | array | — | startup |
| `server[].port` | yes | integer | — | startup (unique) |
| `server[].max_body` | no | integer | `-1` (unlimited) | startup |
| `server[].host` | yes | array | — | startup |
| `host[].hostname` | yes | string | — | startup |
| `host[].error` | no | table | empty | startup (file exists, readable) |
| `host[].route` | yes | array | — | startup |
| `route.preffix` | yes | string | — | runtime |
| `route.path` | yes | string | — | startup (directory exists) |
| `route.index` | no | string | `"index.html"` | runtime |
| `route.methods.GET` | no | boolean | `true` | runtime |
| `route.methods.POST` | no | boolean | `false` | runtime |
| `route.methods.DELETE` | no | boolean | `false` | runtime |
| `route.options.dir_listing` | no | boolean | `false` | runtime |
| `host[].redirect` | no | array | empty | runtime |
| `host[].api` | yes | array | — | startup |
| `api.uri` | yes | string | — | startup |
| `api.external` | yes | boolean | — | startup |
| `api.func` | yes | string | — | startup |
| `api.upload.path` | cond. | string | — | startup |
| `api.upload.random_len` | cond. | integer | — | startup |
| `host[].cgi` | yes | array | — | startup |
| `cgi.glob` | yes | string | — | startup |
| `cgi.bin` | yes | string | — | startup (file, executable) |
