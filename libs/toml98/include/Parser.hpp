
#ifndef TOML98_INCLUDE_PARSER_HPP
#define TOML98_INCLUDE_PARSER_HPP

#include <string>
#include <vector>

/*
[
  {
    "key": ["title"],
    "inlined": false,
    "value": ValueString("TOML Example")
  },

  {
    "key": ["owner", "name"],
    "inlined": false,
    "value": ValueString("Tom Preston-Werner")
  },
  {
    "key": ["database", "enabled"],
    "inlined": false,
    "value": ValueBoolean(true)
  },

  {
    "key": ["database", "ports"],
    "inlined": true,
    "value": ValueList([
      ValueInt(8000),
      ValueInt(8001),
      ValueInt(8002)
    ])
  },
  {
    "key": ["database", "data"],
    "inlined": true,
    "value": ValueList([
      ValueList([
        ValueString("delta"),
        ValueString("phi")
      ]),
      ValueList([
        ValueFloat(72.0)
      ])
    ])
  },
  {
    "key": ["database", "temp_targets", "cpu"],
    "inlined": true,
    "value": ValueFloat(79.5)
  },
  {
    "key": ["database", "temp_targets", "case"],
    "inlined": true,
    "value": ValueFloat(72.0)
  }
]
*/

namespace toml98 {

class Parser {
 public:
  struct KeyPath {
    std::vector<std::string> path;
  };

  struct Statement {
    KeyPath path;
    bool inlined;
  };

 private:
  std::vector<Statement> _document;
};

}  // namespace toml98

#endif
