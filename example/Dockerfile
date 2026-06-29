FROM alpine:latest AS build
RUN apk add --no-cache g++ make python3
WORKDIR /app
COPY . .
RUN make MODE=release

FROM alpine:latest
RUN apk add --no-cache libstdc++ php83-cgi && ln -s /usr/bin/php-cgi83 /usr/bin/php-cgi
WORKDIR /app
COPY --from=build /app/webserv ./webserv
COPY --from=build /app/target/release/toml98/bin/toml98.so ./target/release/toml98/bin/toml98.so
COPY --from=build /app/target/release/mon-cgi/bin/mon-cgi.so ./target/release/mon-cgi/bin/mon-cgi.so
COPY --from=build /app/target/release/mon-http/bin/mon-http.so ./target/release/mon-http/bin/mon-http.so
COPY --from=build /app/target/release/mon-net/bin/mon-net.so ./target/release/mon-net/bin/mon-net.so
COPY --from=build /app/target/release/mon-router/bin/mon-router.so ./target/release/mon-router/bin/mon-router.so
EXPOSE 1998
CMD ["./webserv"]
