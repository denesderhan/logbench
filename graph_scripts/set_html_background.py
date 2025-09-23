#Copyright © 2024, Dénes Derhán.
#Distributed under the MIT license (https://opensource.org/license/mit).

def set_html_background(file_path, color) :
    search_line = "      html, body {\n"
    new_line = "        background: " + color + ";\n"

    with open(file_path, "r") as file:
        lines = file.readlines()

    with open(file_path, "w") as file:
        for line in lines:
            file.write(line)
            if line == search_line:
                file.write(new_line)
