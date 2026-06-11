import sys


def process_files(output_file, files_str, vars_str):
    files = files_str.split()
    variables = vars_str.split()

    if len(files) != len(variables):
        print("Error: The number of files and variables must match.")
        sys.exit(1)

    with open(output_file, "w") as out:
        out.write("#ifndef EMBEDDED_DATA_H\n#define EMBEDDED_DATA_H\n\n")
        out.write("#include <vector>\n\n")

        for filepath, varname in zip(files, variables):
            with open(filepath, "r") as f:
                content = f.read()

            # Escape for C++ string literals
            content = (
                content.replace("\\", "\\\\")
                .replace('"', '\\"')
                .replace("\n", '\\n"\n"')
            )

            # 1. Define the raw data array
            raw_var = varname + "_raw"
            out.write("static const char " + raw_var + '[] = \n"' + content + '";\n')

            # 2. Use the range constructor: std::vector<char> v(start, end)
            # sizeof(array) - 1 excludes the null terminator
            out.write(
                "const std::vector<char> "
                + varname
                + "("
                + raw_var
                + ", "
                + raw_var
                + " + sizeof("
                + raw_var
                + ") - 1);\n\n"
            )

        out.write("#endif // EMBEDDED_DATA_H\n")


if __name__ == "__main__":
    process_files(sys.argv[1], sys.argv[2], sys.argv[3])
