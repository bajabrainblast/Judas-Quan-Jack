def is_separator(ch):
    if ch.isspace() or ch == '(' or ch == ')':
        return True
    return False

def split_lexemes(line):
    lexemes = []
    tmpstr = ""
    for ch in line:
        if is_separator(ch):
            if not tmpstr.isspace():
                lexemes.append(tmpstr)
            tmpstr = ""
    return lexemes

if __name__ == "__main__":
    file_input = open("input.txt", "r")
    lines = file_input.readlines()
    file_input.close()

    for line in lines:
        lst = split_lexemes(line)
        print(lst)