def is_separator(ch):
    if ch.isspace() or ch == '(' or ch == ')':
        return True
    return False

def split_lexemes(line):
    lexemes = []
    tmpstr = ""
    for ch in line:
        if is_separator(ch):
            if not tmpstr.isspace() and tmpstr != "":
                lexemes.append(str(tmpstr))
            tmpstr = ""
        else:
            tmpstr += ch
        if ch == '(' or ch == ')':
            lexemes.append(str(ch))
    return lexemes

if __name__ == "__main__":
    lines = [
        "(print\n",
        "\t(+ 5 bla))"
    ]

    for line in lines:
        lst = split_lexemes(line)
        print(lst)
    print("done")