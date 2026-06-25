#ifndef MON_ROUTER_INCLUDE_MIMETYPES_HPP
#define MON_ROUTER_INCLUDE_MIMETYPES_HPP

#include <map>
#include <string>

namespace mon_router {

static inline std::string get_mime_type(const std::string& path) {
  static std::map<std::string, std::string> mime_types;
  if (mime_types.empty()) {
    mime_types[".html"] = "text/html";
    mime_types[".htm"] = "text/html";
    mime_types[".css"] = "text/css";
    mime_types[".js"] = "application/javascript";
    mime_types[".png"] = "image/png";
    mime_types[".jpg"] = "image/jpeg";
    mime_types[".jpeg"] = "image/jpeg";
    mime_types[".gif"] = "image/gif";
    mime_types[".ico"] = "image/x-icon";
    mime_types[".svg"] = "image/svg+xml";
    mime_types[".webp"] = "image/webp";
    mime_types[".avif"] = "image/avif";

    mime_types[".woff"] = "font/woff";
    mime_types[".woff2"] = "font/woff2";
    mime_types[".ttf"] = "font/ttf";
    mime_types[".otf"] = "font/otf";

    mime_types[".pdf"] = "application/pdf";
    mime_types[".xml"] = "application/xml";
    mime_types[".csv"] = "text/csv";
    mime_types[".md"] = "text/markdown";

    mime_types[".mp3"] = "audio/mpeg";
    mime_types[".mp4"] = "video/mp4";
    mime_types[".webm"] = "video/webm";
    mime_types[".ogg"] = "audio/ogg";
    mime_types[".wav"] = "audio/wav";

    mime_types[".zip"] = "application/zip";
    mime_types[".gz"] = "application/gzip";
    mime_types[".wasm"] = "application/wasm";
    mime_types[".map"] = "application/json";

    mime_types[".json"] = "application/json";
    mime_types[".txt"] = "text/plain";
  }

  size_t dot_idx = path.find_last_of('.');
  if (dot_idx == std::string::npos) {
    return "application/octet-stream";
  }

  std::string ext = path.substr(dot_idx);
  std::map<std::string, std::string>::const_iterator iter =
      mime_types.find(ext);
  if (iter != mime_types.end()) {
    return iter->second;
  }

  return "application/octet-stream";
}

}  // namespace mon_router

#endif